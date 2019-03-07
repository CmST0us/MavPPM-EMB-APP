//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include <functional>
#include <mavlink.h>
#include <map>
#include <vector>
#include <memory>
#include "nocopyable.hpp"

namespace mavppm {

using MavlinkDispatcherMessageHandler = std::function<void(mavlink_message_t &message)>;
using MavlinkDispatcherMessageHandlerPtr = std::weak_ptr<MavlinkDispatcherMessageHandler>;

class MavlinkDispatcher final : public utils::NoCopyable {

public:
    MavlinkDispatcher();
    virtual ~MavlinkDispatcher();

    void registerMessageId(int msgId, MavlinkDispatcherMessageHandlerPtr handler);
    void dispatchMessage(mavlink_message_t &message);

public:

private:
    std::map< int, std::vector<MavlinkDispatcherMessageHandlerPtr > > _listenerMap;

};

}

