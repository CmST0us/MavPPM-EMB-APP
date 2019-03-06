//
// Created by eric3u on 19-3-7.
//

#pragma once

#include <functional>
#include <memory>

#include <mavlink.h>
#include <mavlink_helpers.h>
#include <data.hpp>

#include "nocopyable.hpp"

#pragma mark - SYSTEM ID
#define MAVPPM_SYSTEM_ID_IOS 230
#define MAVPPM_SYSTEM_ID_EMB 231

#pragma mark - COMPONENT ID

#pragma mark IOS
#define MAVPPM_COMPONENT_ID_IOS_APP 10

#pragma mark EMB
#define MAVPPM_COMPONENT_ID_EMB_APP 11

namespace mavppm {

class MavlinkProtocol final : public utils::NoCopyable {
public:
    MavlinkProtocol();
    virtual ~MavlinkProtocol();

    using MavlinkParseHandler = std::function<void(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message)>;
    using MavlinkWriterHandler = std::function<void(mavppm::MavlinkProtocol *protocol, mavlink_message_t &message)>;
    MavlinkParseHandler mParseHandler;
    MavlinkWriterHandler mWriterHandler;

    void parse(utils::Data &data);
    void parse(std::shared_ptr<utils::Data> data);

    void write(mavlink_message_t &message);
public:
    static int SYS_ID_IOS;
    static int SYS_ID_EMB;
    static int COMP_ID_IOS_APP;
    static int COMP_ID_EMB_APP;

    static mavlink_message_info_t mavlinkMessageInfo(int messageId);
    static std::shared_ptr<utils::Data> serialize(mavlink_message_t &message);
    static std::string messageDescription(mavlink_message_t &message);
private:

};

}

