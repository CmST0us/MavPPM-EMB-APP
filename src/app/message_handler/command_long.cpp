//
// Created by CmST0us on 2019/3/14.
//

#include "command_long.hpp"
#include "user_default.hpp"
#include "package_manager.hpp"
#include "channel_map.hpp"

mavppm::message_handler::CommandLong::CommandLong() {
    uniqueID();
}

mavppm::message_handler::CommandLong::~CommandLong() {
    mavppm::PackageManager::shared()->removeObserver(uniqueID());
}

void mavppm::message_handler::CommandLong::start() {
    auto p = mavppm::PackageManager::shared();
    auto b = std::bind(&mavppm::message_handler::CommandLong::handleCommandLongMessage, this, std::placeholders::_1);
    p->registerMessage(MAVLINK_MSG_ID_COMMAND_LONG, uniqueID(), b);
}

void mavppm::message_handler::CommandLong::stop() {
    mavppm::PackageManager::shared()->removeObserver(uniqueID(), MAVLINK_MSG_ID_COMMAND_LONG);
}

void mavppm::message_handler::CommandLong::handleCommandLongMessage(mavlink_message_t &message) {
    mavlink_command_long_t commandLong = {0};
    mavlink_msg_command_long_decode(&message, &commandLong);
    if (commandLong.command == MAV_CMD_DO_SET_PARAMETER) {
        this->handleSetParamCommand(commandLong);
    }
}

void mavppm::message_handler::CommandLong::handleSetParamCommand(mavlink_command_long_t &message) {
    auto p = mavppm::ChannelMap::shared();
    switch ((int)message.param1) {
        case MAVPPM_DO_SET_THROTTLE_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Throttle);
        }
            break;

        case MAVPPM_DO_SET_ROLL_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Roll);
        }
            break;

        case MAVPPM_DO_SET_PITCH_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Pitch);
        }
            break;

        case MAVPPM_DO_SET_YAW_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Yaw);
        }
            break;

        case MAVPPM_DO_SET_BUTTON1_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Button1);
        }
            break;

        case MAVPPM_DO_SET_BUTTON2_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Button2);
        }
            break;

        case MAVPPM_DO_SET_BUTTON3_CHANNEL: {
            p->bind((mavppm::ChannelMap::ChannelNumber)message.param2, mavppm::ChannelMap::ChannelType::Button3);
        }
            break;

        case MAVPPM_DO_SAVE_CHANNEL: {
            p->saveConfig();
        }
            break;
        case MAVPPM_DO_RESET_LAST_CHANNEL: {
            p->loadChannelConfig();
        }
            break;
    }
}