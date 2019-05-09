//
// Created by CmST0us on 2019/3/7.
//

#include <mavlink.h>
#include "random.hpp"
#include "package_manager.hpp"
#include "cube_heartbeat.hpp"

mavppm::CubeHeartbeat::CubeHeartbeat() {
    uniqueID();
}

mavppm::CubeHeartbeat::~CubeHeartbeat() {
    mavppm::PackageManager::shared()->removeObserver(uniqueID());
}

void mavppm::CubeHeartbeat::start() {
    auto workHandler = [this](mavppm::utils::Runloop *runloop) {
        while (!runloop->isCanceled()) {
            mavlink_heartbeat_t heartbeat;
            mavlink_message_t msg;
            mavlink_msg_heartbeat_encode(mavppm::MavlinkProtocol::SYS_ID_EMB, mavppm::MavlinkProtocol::COMP_ID_EMB_APP, &msg, &heartbeat);
            mavppm::PackageManager::shared()->sendMessage(msg);
            usleep(mHeatbeatInterval * 1000000);
        }
    };
    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workHandler));
    _runloop->run();
}

void mavppm::CubeHeartbeat::stop() {
    if (_runloop != nullptr) {
        _runloop->stop();
        mavppm::PackageManager::shared()->removeObserver(uniqueID());
    }
}

const std::string mavppm::CubeHeartbeat::uniqueID() {
    if (_uniqueID.length() > 0) {
        return _uniqueID;
    }
    _uniqueID = mavppm::utils::random_string();
    return _uniqueID;
}