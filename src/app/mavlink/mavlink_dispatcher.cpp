//
// Created by CmST0us on 2019/3/7.
//

#include "mavlink_dispatcher.hpp"

mavppm::MavlinkDispatcher::MavlinkDispatcher() {

}

mavppm::MavlinkDispatcher::~MavlinkDispatcher() {

}

void mavppm::MavlinkDispatcher::registerMessageId(int msgId, std::string observerKey,
                                                  MavlinkDispatcherMessageHandler handler) {
    _mapLock.lock();
    if (_observerMap.count(observerKey) > 0) {
        MavlinkDispatcherMessageHandlerMap messageMap = _observerMap[observerKey];
        if (messageMap.count(msgId) > 0) {
            MavlinkDispatcherHandlerVector v = messageMap[msgId];
            v.push_back(handler);
        } else {
            MavlinkDispatcherHandlerVector v;
            v.push_back(handler);
            messageMap[msgId] = v;
        }
    } else {
        MavlinkDispatcherMessageHandlerMap messageMap;
        MavlinkDispatcherHandlerVector v;
        v.push_back(handler);
        messageMap[msgId] = v;
        _observerMap[observerKey] = messageMap;
    }
    _mapLock.unlock();
}

void mavppm::MavlinkDispatcher::removeObserver(std::string observerKey) {
    _mapLock.lock();
    if (_observerMap.count(observerKey) > 0) {
        _observerMap.erase(observerKey);
    }
    _mapLock.unlock();
}

void mavppm::MavlinkDispatcher::removeObserver(std::string observerKey, int msgId) {
    _mapLock.lock();
    if (_observerMap.count(observerKey) > 0) {
        MavlinkDispatcherMessageHandlerMap m = _observerMap[observerKey];
        if (m.count(msgId) > 0) {
            m.erase(msgId);
        }
    }
    _mapLock.unlock();
}

void mavppm::MavlinkDispatcher::dispatchMessage(mavlink_message_t &message) {
    _mapLock.lock();
    MavlinkDispatcherObserverMap::iterator it = _observerMap.begin();
    while (it != _observerMap.end()) {
        MavlinkDispatcherMessageHandlerMap handlerMap = it->second;
        if (handlerMap.count(message.msgid) > 0) {
            MavlinkDispatcherHandlerVector handlerVector = handlerMap[message.msgid];
            auto handlerVectorIt = handlerVector.begin();
            while (handlerVectorIt != handlerVector.end()) {
                auto h = *handlerVectorIt;
                if (h != nullptr) {
                    h(message);
                }
                handlerVectorIt++;
            }
        }
        it++;
    }
    _mapLock.unlock();
}
