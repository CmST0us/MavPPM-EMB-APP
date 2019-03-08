//
// Created by CmST0us on 2019/3/7.
//

#include <memory>

#include "random.hpp"
#include "link_manager.hpp"
#include "package_manager.hpp"

mavppm::LinkManager::LinkManager() {
    uniqueID();
}

mavppm::LinkManager::~LinkManager() {
    mavppm::PackageManager::shared()->removeObserver(uniqueID());
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
    mavppm::PackageManager::shared()->registerMessage(MAVLINK_MSG_ID_HEARTBEAT, uniqueID(), bind);
    _heartbeat = std::make_shared<mavppm::CubeHeartbeat>();
}

void mavppm::LinkManager::close() {
    if (_heartbeat != nullptr) {
        _heartbeat->stop();
        _heartbeat = nullptr;
        _isConnected = false;
    }
}

bool mavppm::LinkManager::isConnected() const {
    return _isConnected;
}

const std::string mavppm::LinkManager::uniqueID() {
    if (_uniqueID.length() > 0) {
        return _uniqueID;
    }
    _uniqueID = mavppm::utils::random_string();
    return _uniqueID;
}