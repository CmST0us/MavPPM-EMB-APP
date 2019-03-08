//
// Created by cmst0us on 19-3-5.
//

#pragma once

#include "application.hpp"
#include "link_manager.hpp"
#include "manual_control.hpp"

namespace mavppm {

class MavPPM final : public Application {
public:
    static int USBMUXD_CONNECT_PORT;
    MavPPM();
    virtual ~MavPPM();
    virtual void run() override;

private:
    std::shared_ptr<mavppm::LinkManager> _linkManager{nullptr};

#pragma mark - Message Handler
    std::shared_ptr<mavppm::message_handler::ManualControl> _manualControl{nullptr};
};

}
