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
    _runloop = std::unique_ptr<mavppm::utils::Runloop>(new mavppm::utils::Runloop(std::bind(&mavppm::PackageManager::runloop, this, std::placeholders::_1)));
    _runloop->run();
}

void mavppm::PackageManager::runloop(mavppm::utils::Runloop *runloop) {
    while (!runloop->isCanceled()) {

        _dataSendQueueLock.lock();
        while (!_dataSendQueue.empty()) {
            auto d =_dataSendQueue.front();
            _dataSendQueue.pop();
            if (_connectedDeviceSocket->stateMachine().isWritable()) {
                _connectedDeviceSocket->write(d);
            }
        }
        _dataSendQueueLock.unlock();

        _dataRecvQueueLock.lock();
        while (!_dataRecvQueue.empty()) {
            auto data = _dataRecvQueue.front();
            _dataRecvQueue.pop();

            mavppm::utils::Data d(data->getDataSize());
            d.copy(data->getDataAddress(), data->getDataSize());
            _mavlinkProtocol->parse(d);
        }
        _dataRecvQueueLock.unlock();

        usleep(100000);
        runloop->dispatch();
    }
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
//            _reConnectTimer = std::make_shared<mavppm::utils::Timer>();
            if (_connectedDeviceSocket == nullptr ||
                _connectedDeviceSocket->stateMachine().state() != socketkit::CommunicatorState::Established) {
                // connect
                _currentAttachDeviceEndpoint = std::make_shared<socketkit::Endpoint>(std::to_string(record.deviceId), mRemotePort, false);
                tryConnect(_currentAttachDeviceEndpoint);
            }
        } else {
            std::cout<<"[MavPPM][USBMUXD]: Disconnect Device " << std::to_string(record.deviceId) << std::endl;
//            _reConnectTimer->expire();
//            _reConnectTimer = nullptr;
        }
    };
    _deviceListener->getRunloop()->run();
    _deviceListener->startListenDevice();
}

void mavppm::PackageManager::usbmuxdSocketEventHandler(socketkit::ICommunicator *communicator, socketkit::CommunicatorEvent event) {
    _reConnecting = false;
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
            clearQueue();
        }
            break;
        case socketkit::CommunicatorEvent::ErrorOccurred: {
//            std::cout<<"[MavPPM][USBMUXD]: Error" << std::endl;
            communicator->close();
            communicator->getRunloop()->stop();
            clearQueue();
        }
            break;
    }
}

void mavppm::PackageManager::usbmuxdSocketReadHandler(socketkit::ICommunicator *,
                                              std::shared_ptr<socketkit::utils::Data> data) {
    // send to mavlink
    if (data->getDataSize() > 0) {
        if (_dataRecvQueue.size() > 1024) {
            return;
        }
        _dataRecvQueueLock.lock();
        _dataRecvQueue.push(data);
        _dataRecvQueueLock.unlock();
    }
}

void mavppm::PackageManager::mavlinkProtocolParseHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    _dispatcher->dispatchMessage(message);
}

void mavppm::PackageManager::mavlinkProtocolWriteHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    if (_dataSendQueue.size() > 1024) {
        return;
    }

    auto data = mavppm::MavlinkProtocol::serialize(message);
    auto d = std::make_shared<socketkit::utils::Data>(data->getDataSize());
    d->copy(data->getDataAddress(), data->getDataSize());

    _dataSendQueueLock.lock();
    _dataSendQueue.push(d);
    _dataSendQueueLock.unlock();
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
    if (_reConnecting)
        return;
    _reConnecting = true;
    _runloop->post([this]() {
        if (_connectedDeviceSocket != nullptr &&
            _connectedDeviceSocket->stateMachine().state() == socketkit::CommunicatorState::Established) {
            return;
        }
        tryConnect(_currentAttachDeviceEndpoint);
    });
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

void mavppm::PackageManager::clearQueue() {
    _dataRecvQueueLock.lock();
    decltype(_dataRecvQueue) emptyRecv;
    std::swap(emptyRecv, _dataRecvQueue);
    _dataRecvQueueLock.unlock();

    _dataSendQueueLock.lock();
    decltype(_dataSendQueue) emptySend;
    std::swap(emptySend, _dataSendQueue);
    _dataSendQueueLock.unlock();
}
