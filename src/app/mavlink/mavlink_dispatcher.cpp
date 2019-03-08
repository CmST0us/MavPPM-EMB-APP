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
        MavlinkDispatcherMessageHandlerMap::iterator handlerIt = it->second.begin();
        while (handlerIt != it->second.end()) {
            MavlinkDispatcherHandlerVector v = handlerIt->second;
            for (int i = 0; i < v.size(); ++i) {
                MavlinkDispatcherMessageHandler h = v[i];
                if (h != nullptr) {
                    h(message);
                }
            }
            handlerIt++;
        }
        it++;
    }
    _mapLock.unlock();
}
