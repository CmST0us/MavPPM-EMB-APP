//
// Created by CmST0us on 2019/3/14.
//

#pragma once

#include <mavlink.h>
#include <string>
#include "random.hpp"
#include "imessage_handler.hpp"

namespace mavppm {
namespace message_handler {

class CommandLong final : public mavppm::message_handler::IMessageHandler {
public:
    CommandLong();
    virtual ~CommandLong();

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
    std::string _uniqueID{""};
};

}

}

