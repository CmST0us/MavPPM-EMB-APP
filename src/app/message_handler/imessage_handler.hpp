//
// Created by CmST0us on 2019/3/8.
//

#pragma once

#include "nocopyable.hpp"
#include "itrackable.hpp"

namespace mavppm {

namespace message_handler {
class IMessageHandler : public mavppm::utils::NoCopyable, public mavppm::utils::ITrackable {
    virtual void start() = 0;
    virtual void stop() = 0;
};

}

}