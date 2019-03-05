//
// Created by cmst0us on 19-3-5.
//

#pragma once

#include <memory>

#include <libSocketKit/UsbmuxdSocket.hpp>
#include <libSocketKit/UsbmuxdDeviceListener.hpp>

#include <MPPPMEncoder/UARTPPMEncoder.hpp>

#include "application.hpp"

namespace mavppm {

class MavPPM final : public Application {
public:
    static int USBMUXD_CONNECT_PORT;

    MavPPM();
    ~MavPPM();
    virtual void run() override;

#pragma mark - Usbmuxd
private:
    std::shared_ptr<socketkit::UsbmuxdDeviceListener> _deviceListener{nullptr};
    std::shared_ptr<socketkit::UsbmuxdSocket> _connectedDeviceSocket{nullptr};

    void tryStartUsbmuxdListeningDevice();
    void startUsbmuxdListeningDevice();

#pragma mark - Bind
private:
    void usbmuxdSocketEventHandler(socketkit::ICommunicator *, socketkit::CommunicatorEvent event);
    void usbmuxdSocketReadHandler(socketkit::ICommunicator *, std::shared_ptr<socketkit::utils::Data> data);
};

}