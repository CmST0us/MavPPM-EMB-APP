//
// Created by CmST0us on 2019/3/7.
//

#include <memory>

#include "link_manager.hpp"
#include "package_manager.hpp"

mavppm::LinkManager::LinkManager() {
    _heartbeatTimeoutTimer = std::make_shared<mavppm::utils::Timer>();
}

mavppm::LinkManager::~LinkManager() {
    _heartbeatTimeoutTimer = nullptr;
}

void mavppm::LinkManager::recvHeartbeat(mavlink_message_t &message) {
    std::string str = mavppm::MavlinkProtocol::messageDescription(message);
    std::cout<<str<<std::endl;
    _heartbeatCount ++;
    if (!_isConnected) {
        _heartbeat->start();
        _isConnected = true;
    }
}

void mavppm::LinkManager::open() {
    auto bind = std::bind(&mavppm::LinkManager::recvHeartbeat, this, std::placeholders::_1);
    _handler = std::make_shared<mavppm::MavlinkDispatcherMessageHandler>(bind);
    mavppm::MavlinkDispatcherMessageHandlerPtr p(_handler);
    mavppm::PackageManager::shared()->registerMessage(MAVLINK_MSG_ID_HEARTBEAT, p);
    _heartbeat = std::make_shared<mavppm::CubeHeartbeat>();
    _heartbeat->mHeatbeatInterval = mHeartbeatIntervalSecond;
    _heartbeatTimeoutTimer->startTimer(mHeartbeatLostCheckIntervalSecond * 1000, std::bind(&mavppm::LinkManager::checkTimeout, this));
}

void mavppm::LinkManager::close() {
    if (_heartbeat != nullptr) {
        _heartbeat->stop();
    }
    _handler = nullptr;
    _heartbeat = nullptr;
}

bool mavppm::LinkManager::isConnected() const {
    return _isConnected;
}

void mavppm::LinkManager::checkTimeout() {
    if (_lastHeartbeatCount == _heartbeatCount) {
        _heartbeatLostCount++;
        if (_heartbeatCount > mMaxHeartbeatLost) {
            this->close();
            mavppm::PackageManager::shared()->forceDisconnect();
        }
    } else {
        _lastHeartbeatCount = _heartbeatCount;
        _heartbeatLostCount = 0;
    }
}