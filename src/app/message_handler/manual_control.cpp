//
// Created by CmST0us on 2019/3/8.
//

#include "package_manager.hpp"
#include "manual_control.hpp"

mavppm::message_handler::ManualControl::ManualControl() {
    uniqueID();
}

mavppm::message_handler::ManualControl::~ManualControl() {
    mavppm::PackageManager::shared()->removeObserver(uniqueID());
}

void mavppm::message_handler::ManualControl::start() {
    auto p = mavppm::PackageManager::shared();
    auto b = std::bind(&mavppm::message_handler::ManualControl::handleManualControl, this, std::placeholders::_1);
    p->registerMessage(MAVLINK_MSG_ID_MANUAL_CONTROL, uniqueID(), b);
}

void mavppm::message_handler::ManualControl::stop() {
    auto p = mavppm::PackageManager::shared();
    p->removeObserver(uniqueID(), MAVLINK_MSG_ID_MANUAL_CONTROL);
}

void mavppm::message_handler::ManualControl::handleManualControl(mavlink_message_t &message) {
    mavlink_manual_control_t control;
    mavlink_msg_manual_control_decode(&message, &control);
    std::cout<<mavppm::MavlinkProtocol::messageDescription(message)<<std::endl;
}

