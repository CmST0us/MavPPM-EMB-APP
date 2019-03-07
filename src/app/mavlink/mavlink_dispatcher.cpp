//
// Created by CmST0us on 2019/3/7.
//

#include "mavlink_dispatcher.hpp"

mavppm::MavlinkDispatcher::MavlinkDispatcher() {

}

mavppm::MavlinkDispatcher::~MavlinkDispatcher() {

}

void mavppm::MavlinkDispatcher::registerMessageId(int msgId,
                                                 mavppm::MavlinkDispatcherMessageHandlerPtr handler) {
    if (_listenerMap.count(msgId) > 0) {
        std::vector<mavppm::MavlinkDispatcherMessageHandlerPtr > v = _listenerMap[msgId];
        v.push_back(handler);
    } else {
        std::vector<mavppm::MavlinkDispatcherMessageHandlerPtr> v;
        v.push_back(handler);
        _listenerMap[msgId] = v;
    }
}

void mavppm::MavlinkDispatcher::dispatchMessage(mavlink_message_t &message) {
    if (_listenerMap.count(message.msgid) == 0) {
        return;
    }

    std::vector<mavppm::MavlinkDispatcherMessageHandlerPtr > v = _listenerMap[message.msgid];
    for (int i = 0; i < v.size(); ++i) {
        mavppm::MavlinkDispatcherMessageHandlerPtr p = v[i];
        if (p.use_count() > 0) {
            if (std::shared_ptr<mavppm::MavlinkDispatcherMessageHandler> h = p.lock()) {
                (*h.get())(message);
            }
        }
    }
}