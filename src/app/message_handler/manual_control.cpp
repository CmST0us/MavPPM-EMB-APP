//
// Created by CmST0us on 2019/3/8.
//

#include "package_manager.hpp"
#include "manual_control.hpp"
#include "channel_map.hpp"

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
    _encoder = ppm::PPMEncoderFactory::createPPMEncoder(ppm::SystemType::EMB_NanoPi, mPPMDevice.c_str(), mBandrate);
    _encoder->setup();
}

void mavppm::message_handler::ManualControl::stop() {
    auto p = mavppm::PackageManager::shared();
    p->removeObserver(uniqueID(), MAVLINK_MSG_ID_MANUAL_CONTROL);
    _encoder = nullptr;
}

void mavppm::message_handler::ManualControl::handleManualControl(mavlink_message_t &message) {
    mavlink_manual_control_t control = {0};
    mavlink_msg_manual_control_decode(&message, &control);
    std::cout<<mavppm::MavlinkProtocol::messageDescription(message)<<std::endl;
    auto p = mavppm::ChannelMap::shared();

    int channel[] = {
            (int)p->channel(mavppm::ChannelMap::ChannelType::Throttle) - 1, //Throtter
            (int)p->channel(mavppm::ChannelMap::ChannelType::Yaw) - 1, // Yaw
            (int)p->channel(mavppm::ChannelMap::ChannelType::Roll) - 1, // Roll
            (int)p->channel(mavppm::ChannelMap::ChannelType::Pitch) - 1, // Pitch
            (int)p->channel(mavppm::ChannelMap::ChannelType::Button1) - 1, // Button 1
            (int)p->channel(mavppm::ChannelMap::ChannelType::Button2) - 1, // Button 2,
            (int)p->channel(mavppm::ChannelMap::ChannelType::Button3) - 1, // Button 3,
    };

    int value[] = {
        control.z,
        control.r,
        control.y,
        control.x,
        control.buttons & 0x01 ? _encoder->maxChannelValue() : _encoder->minChannelValue(),
        control.buttons & 0x02 ? _encoder->maxChannelValue() : _encoder->minChannelValue(),
        control.buttons & 0x03 ? _encoder->maxChannelValue() : _encoder->minChannelValue(),
    };

    _encoder->writeValue(channel, value, sizeof(channel));
}

