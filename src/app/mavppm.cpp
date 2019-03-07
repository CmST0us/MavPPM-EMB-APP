//
// Created by cmst0us on 19-3-5.
//

#include <iostream>

#include "mavppm.hpp"

int mavppm::MavPPM::USBMUXD_CONNECT_PORT = 17123;

mavppm::MavPPM::MavPPM() : _mavlinkProtocol{std::make_shared<mavppm::MavlinkProtocol>()}{
    auto mavlinkParseBind = std::bind(&mavppm::MavPPM::mavlinkProtocolParseHandler, this, std::placeholders::_1, std::placeholders::_2
    );
    auto mavlinkWriteBind = std::bind(&mavppm::MavPPM::mavlinkProtocolWriteHandler, this, std::placeholders::_1, std::placeholders::_2
    );
    _mavlinkProtocol->mParseHandler = mavlinkParseBind;
    _mavlinkProtocol->mWriterHandler = mavlinkWriteBind;
}

mavppm::MavPPM::~MavPPM() {

}

void mavppm::MavPPM::run() {
    std::cout<<"[MavPPM]: Start Running"<<std::endl;
    tryStartUsbmuxdListeningDevice();

    while (true) {
        ::sleep(1000);
    }
}

void mavppm::MavPPM::tryStartUsbmuxdListeningDevice() {
    try {
        startUsbmuxdListeningDevice();
    } catch (SocketException &e){
        std::cout<<"[MavPPM][USBMUXD]: start listening device failed, " << e.getExceptionDescription() << ", retry" <<std::endl;
        ::sleep(1);
        tryStartUsbmuxdListeningDevice();
    }
}

void mavppm::MavPPM::startUsbmuxdListeningDevice() {
    _deviceListener = std::make_shared<socketkit::UsbmuxdDeviceListener>();
    _deviceListener->mDeviceListenerHandler = [&](bool isAttach, socketkit::UsbmuxdDeviceRecord record) {
        if (isAttach) {
            if (_connectedDeviceSocket == nullptr ||
                _connectedDeviceSocket->stateMachine().state() != socketkit::CommunicatorState::Established) {
                // connect
                _connectedDeviceSocket = std::make_shared<socketkit::UsbmuxdSocket>();
                // bind socket event
                auto eventFuncBind = std::bind(&mavppm::MavPPM::usbmuxdSocketEventHandler, this, std::placeholders::_1, std::placeholders::_2);
                _connectedDeviceSocket->mEventHandler = eventFuncBind;
                // run and connect
                _connectedDeviceSocket->getRunloop()->run();
                auto deviceEndpoit = std::make_shared<socketkit::Endpoint>(std::to_string(record.deviceId), USBMUXD_CONNECT_PORT, false);
                _connectedDeviceSocket->connect(deviceEndpoit);
            }
        } else {
            std::cout<<"[MavPPM][USBMUXD]: Disconnect Device " << std::to_string(record.deviceId) << std::endl;
        }
    };
    _deviceListener->getRunloop()->run();
    _deviceListener->startListenDevice();
}

void mavppm::MavPPM::usbmuxdSocketEventHandler(socketkit::ICommunicator *communicator, socketkit::CommunicatorEvent event) {
    switch (event) {
        case socketkit::CommunicatorEvent::OpenCompleted: {
            std::cout<<"[MavPPM][USBMUXD]: Connect Device " << _connectedDeviceSocket->connectingEndpoint()->getEndpointIp() << " Success" << std::endl;
        }
            break;
        case socketkit::CommunicatorEvent::HasBytesAvailable: {
            auto readBind = std::bind(&mavppm::MavPPM::usbmuxdSocketReadHandler, this, std::placeholders::_1, std::placeholders::_2);
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

void mavppm::MavPPM::usbmuxdSocketReadHandler(socketkit::ICommunicator *,
                                              std::shared_ptr<socketkit::utils::Data> data) {
    // send to mavlink
    if (data > 0) {
        mavppm::utils::Data d(data->getDataSize());
        d.copy(data->getDataAddress(), data->getDataSize());
        _mavlinkProtocol->parse(d);
    }
}

void mavppm::MavPPM::mavlinkProtocolParseHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    std::string str = mavppm::MavlinkProtocol::messageDescription(message);
    std::cout<<str<<std::endl;

}

void mavppm::MavPPM::mavlinkProtocolWriteHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message) {
    auto data = mavppm::MavlinkProtocol::serialize(message);
    auto d = std::make_shared<socketkit::utils::Data>(data->getDataSize());
    d->copy(data->getDataAddress(), data->getDataSize());
    _connectedDeviceSocket->write(d);
}