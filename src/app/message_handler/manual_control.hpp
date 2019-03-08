//
// Created by CmST0us on 2019/3/8.
//

#pragma once

#include <mavlink.h>
#include "random.hpp"
#include "imessage_handler.hpp"

namespace mavppm {
namespace message_handler {

class ManualControl final : public mavppm::message_handler::IMessageHandler {
public:
    ManualControl();
    virtual ~ManualControl();

    virtual const std::string uniqueID() override {
        if (_uniqueID.length() > 0) {
            return _uniqueID;
        }
        _uniqueID = mavppm::utils::random_string();
        return _uniqueID;
    }

    virtual void start() override;
    virtual void stop() override;

private:
    void handleManualControl(mavlink_message_t &message);
    std::string _uniqueID{""};
};

}
}

