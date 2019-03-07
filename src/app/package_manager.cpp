//
// Created by CmST0us on 2019/3/7.
//

#include <memory>
#include "package_manager.hpp"

mavppm::PackageManager* mavppm::PackageManager::shared() {
    static std::shared_ptr<mavppm::PackageManager> manager = {nullptr};
    if (manager) {
        return manager.get();
    }
    manager = std::make_shared<mavppm::PackageManager>();
    return manager.get();
}

void mavppm::PackageManager::setupPackageManager(int remotePort) {
    mRemotePort = remotePort;

    _mavlinkProtocol = std::make_shared<mavppm::MavlinkProtocol>();
    _dispatcher = std::make_shared<mavppm::MavlinkDispatcher>();

    auto mavlinkParseBind = std::bind(&mavppm::PackageManager::mavlinkProtocolParseHandler, this, std::placeholders::_1, std::placeholders::_2
    );
    auto mavlinkWriteBind = std::bind(&mavppm::PackageManager::mavlinkProtocolWriteHandler, this, std::placeholders::_1, std::placeholders::_2
    );

    _mavlinkProtocol->mParseHandler = mavlinkParseBind;
    _mavlinkProtocol->mWriterHandler = mavlinkWriteBind;

    tryStartUsbmuxdListeningDevice();
}

void mavppm::PackageManager::forceDisconnect() {
    if (_connectedDeviceSocket) {
        _connectedDeviceSocket->close();
        _connectedDeviceSocket->getRunloop()->stop();
    }
}

void mavppm::PackageManager::tryStartUsbmuxdListeningDevice() {
    try {
        startUsbmuxdListeningDevice();
    } catch (SocketException &e){
        std::cout<<"[MavPPM][USBMUXD]: start listening device failed, " << e.getExceptionDescription() << ", retry" <<std::endl;
        ::sleep(1);
        tryStartUsbmuxdListeningDevice();
    }
}

void mavppm::PackageManager::startUsbmuxdListeningDevice() {
    _deviceListener = std::make_shared<socketkit::UsbmuxdDeviceListener>();
    _deviceListener->mDeviceListenerHandler = [&](bool isAttach, socketkit::UsbmuxdDeviceRecord record) {
        if (isAttach) {
            if (_connectedDeviceSocket == nullptr ||
                _connectedDeviceSocket->stateMachine().state() != socketkit::CommunicatorState::Established) {
                // connect
                _currentAttachDeviceEndpoint = std::make_shared<socketkit::Endpoint>(std::to_string(record.deviceId), mRemotePort, false);
                tryConnect(_currentAttachDeviceEndpoint);
            }
        } else {
            std::cout<<"[MavPPM][USBMUXD]: Disconnect Device " << std::to_string(record.deviceId) << std::endl;
        }
    };
    _deviceListener->getRunloop()->run();
    _deviceListener->startListenDevice();
}

void mavppm::PackageManager::usbmuxdSocketEventHandler(socketkit::ICommunicator *communicator, socketkit::CommunicatorEvent event) {
    switch (event) {
        case socketkit::CommunicatorEvent::OpenCompleted: {
            std::cout<<"[MavPPM][USBMUXD]: Connect Device " << _connectedDeviceSocket->connectingEndpoint()->getEndpointIp() << " Success" << std::endl;
            notifyDeviceConnected();
        }
            break;
        case socketkit::CommunicatorEvent::HasBytesAvailable: {
            auto readBind = std::bind(&mavppm::PackageManager::usbmuxdSocketReadHandler, this, std::placeholders::_1, std::placeholders::_2);
            communicator->read(readBind);
        }
            break;
        case socketkit::CommunicatorEvent::EndEncountered: {
//            std::cout<<"[MavPPM][USBMUXD]: EOF" << std::endl;
            communicator->close();
            communicator->getRunloop()->stop();
        }
            break;
        case socketkit::CommunicatorEvent::ErrorOccurred: {
//            std::cout<<"[MavPPM][USBMUXD]: Error" << std::endl;
            communicator->close();
            communicator->getRunloop()->stop();
        }
            break;
    }
}

void mavppm::PackageManager::usbmuxdSocketReadHandler(socketkit::ICommunicator *,
                                              std::shared_ptr<socketkit::utils::Data> data) {
    // send to mavlink
    if (data->getDataSize() > 0) {
        mavppm::utils::Data d(data->getDataSize());
        d.copy(data->getDataAddress(), data->getDataSize());
        _mavlinkProtocol->parse(d);
    }
}

void mavppm::PackageManager::mavlinkProtocolParseHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    _dispatcher->dispatchMessage(message);
}

void mavppm::PackageManager::mavlinkProtocolWriteHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    auto data = mavppm::MavlinkProtocol::serialize(message);
    auto d = std::make_shared<socketkit::utils::Data>(data->getDataSize());
    d->copy(data->getDataAddress(), data->getDataSize());

    _connectedDeviceSocket->write(d);
}

void mavppm::PackageManager::sendMessage(mavlink_message_t &msg) {
    _mavlinkProtocol->write(msg);
}

void mavppm::PackageManager::registerMessage(int msgId, MavlinkDispatcherMessageHandlerPtr handlerPtr) {
    _dispatcher->registerMessageId(msgId, handlerPtr);
}

void mavppm::PackageManager::notifyDeviceConnected() {
    if (mConnectedHandler) {
        mConnectedHandler();
    }
}

void mavppm::PackageManager::reConnect() {
    if (_connectedDeviceSocket != nullptr &&
        _connectedDeviceSocket->stateMachine().state() == socketkit::CommunicatorState::Established) {
        return;
    }
    tryConnect(_currentAttachDeviceEndpoint);
}

std::shared_ptr<socketkit::UsbmuxdSocket> mavppm::PackageManager::createSocket() {
    auto socket = std::make_shared<socketkit::UsbmuxdSocket>();
    // bind socket event
    auto eventFuncBind = std::bind(&mavppm::PackageManager::usbmuxdSocketEventHandler, this, std::placeholders::_1, std::placeholders::_2);
    socket->mEventHandler = eventFuncBind;
    // run and connect
    socket->getRunloop()->run();
    return socket;
}

void mavppm::PackageManager::tryConnect(std::shared_ptr<socketkit::Endpoint> endpoint) {
    _connectedDeviceSocket = createSocket();
    _connectedDeviceSocket->connect(endpoint);
}
