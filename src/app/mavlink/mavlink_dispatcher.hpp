//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include <functional>
#include <mavlink.h>
#include <map>
#include <iterator>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "nocopyable.hpp"

namespace mavppm {

using MavlinkDispatcherMessageHandler = std::function<void(mavlink_message_t &message)>;

class MavlinkDispatcher final : public utils::NoCopyable {

public:
    MavlinkDispatcher();
    virtual ~MavlinkDispatcher();

    void registerMessageId(int msgId, std::string observerKey, MavlinkDispatcherMessageHandler handler);
    void removeObserver(std::string observerKey);
    void removeObserver(std::string observerKey, int msgId);

    void dispatchMessage(mavlink_message_t &message);

public:

private:
    using MavlinkDispatcherHandlerVector = std::vector<MavlinkDispatcherMessageHandler>;
    using MavlinkDispatcherMessageHandlerMap = std::map<int, MavlinkDispatcherHandlerVector>;
    using MavlinkDispatcherObserverMap = std::map<std::string, MavlinkDispatcherMessageHandlerMap>;

    std::mutex _mapLock;
    MavlinkDispatcherObserverMap _observerMap;

};

}

