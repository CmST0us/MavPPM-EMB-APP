//
// Created by cmst0us on 19-3-5.
//

#pragma once

#include <string>
#include "application.hpp"
#include "link_manager.hpp"
#include "manual_control.hpp"

namespace mavppm {

class MavPPM final : public Application {
public:
    static int USBMUXD_CONNECT_PORT;
    MavPPM();
    virtual ~MavPPM();
    virtual void run(int argc, char *argv[]) override;

private:
    std::string _PPMDevice{"/dev/ttyS1"};
    int _bandrate{4800};

    void setupParam(int argc, char *argv[]);

private:
    std::shared_ptr<mavppm::LinkManager> _linkManager{nullptr};

#pragma mark - Message Handler
    std::shared_ptr<mavppm::message_handler::ManualControl> _manualControl{nullptr};
};

}
