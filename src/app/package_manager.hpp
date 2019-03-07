//
// Created by CmST0us on 2019/3/7.
//

#pragma once
#include <memory>

#include <libSocketKit/UsbmuxdSocket.hpp>
#include <libSocketKit/UsbmuxdDeviceListener.hpp>

#include <MPPPMEncoder/UARTPPMEncoder.hpp>

#include <mavlink.h>

#include "runloop.hpp"
#include "application.hpp"
#include "mavlink_protocol.hpp"
#include "mavlink_dispatcher.hpp"
#include "mavlink_dispatcher.hpp"
#include "timer.hpp"

namespace mavppm {

class PackageManager final : public mavppm::utils::NoCopyable {
public:
    static PackageManager* shared();

    int mRemotePort{0};

    void setupPackageManager(int remotePort);

    void sendMessage(mavlink_message_t &msg);
    void registerMessage(int msgId, MavlinkDispatcherMessageHandlerPtr handlerPtr);
    void forceDisconnect();
    void reConnect();

    using PackageManagerDeviceConnectingHandler = std::function<void(bool isConnected)>;
    PackageManagerDeviceConnectingHandler mConnectingHandler;

private:
#pragma mark - Mavlink
    std::shared_ptr<mavppm::MavlinkProtocol> _mavlinkProtocol{nullptr};
    std::shared_ptr<mavppm::MavlinkDispatcher> _dispatcher{nullptr};

#pragma mark - Usbmuxd
    std::shared_ptr<socketkit::UsbmuxdDeviceListener> _deviceListener{nullptr};
    std::shared_ptr<socketkit::UsbmuxdSocket> _connectedDeviceSocket{nullptr};

    void tryStartUsbmuxdListeningDevice();
    void startUsbmuxdListeningDevice();

#pragma mark - Bind
    void usbmuxdSocketEventHandler(socketkit::ICommunicator *, socketkit::CommunicatorEvent event);
    void usbmuxdSocketReadHandler(socketkit::ICommunicator *, std::shared_ptr<socketkit::utils::Data> data);
    void mavlinkProtocolParseHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message);
    void mavlinkProtocolWriteHandler(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message);

#pragma mark - Link
    std::shared_ptr<socketkit::Endpoint> _currentAttachDeviceEndpoint;
    std::shared_ptr<mavppm::utils::Timer> _reConnectTimer;

    void tryConnect(std::shared_ptr<socketkit::Endpoint> endpoint);
    std::shared_ptr<socketkit::UsbmuxdSocket> createSocket();
    void notifyDeviceConnecting(bool isConnected);
};

}


