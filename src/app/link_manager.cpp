//
// Created by CmST0us on 2019/3/7.
//

#include <memory>

#include "link_manager.hpp"
#include "package_manager.hpp"

mavppm::LinkManager::LinkManager() {

}

mavppm::LinkManager::~LinkManager() {

}

void mavppm::LinkManager::recvHeartbeat(mavlink_message_t &message) {
    std::string str = mavppm::MavlinkProtocol::messageDescription(message);
    std::cout<<str<<std::endl;
    _heartBeatCount ++;
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