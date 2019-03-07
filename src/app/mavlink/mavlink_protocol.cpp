//
// Created by eric3u on 19-3-7.
//

#include <strstream>
#include "mavlink_protocol.hpp"

int mavppm::MavlinkProtocol::SYS_ID_IOS = MAVPPM_SYSTEM_ID_IOS;
int mavppm::MavlinkProtocol::SYS_ID_EMB = MAVPPM_SYSTEM_ID_EMB;
int mavppm::MavlinkProtocol::COMP_ID_IOS_APP = MAVPPM_COMPONENT_ID_IOS_APP;
int mavppm::MavlinkProtocol::COMP_ID_EMB_APP = MAVPPM_COMPONENT_ID_EMB_APP;

mavppm::MavlinkProtocol::MavlinkProtocol() {

}

mavppm::MavlinkProtocol::~MavlinkProtocol() {

}

mavlink_message_info_t mavppm::MavlinkProtocol::mavlinkMessageInfo(int messageId) {
    static mavlink_message_info_t infos[] = MAVLINK_MESSAGE_INFO;
    mavlink_message_info_t ret;
    if (messageId > sizeof(infos) ||
        messageId < 0) {
        ret.num_fields = -1;
    }
    ret = infos[messageId];
    return ret;
}

void mavppm::MavlinkProtocol::parse(mavppm::utils::Data &data) {
    int dataLen = data.getDataSize();
    uint8_t *dataAddr = (uint8_t *)data.getDataAddress();
    mavlink_message_t msg;
    mavlink_status_t status;
    for (int i = 0; i < dataLen; ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, dataAddr[i], &msg, &status)) {
            // parse success
            if (this->mParseHandler) {
                this->mParseHandler(this, msg);
            }
        }
    }
}

void mavppm::MavlinkProtocol::parse(std::shared_ptr<mavppm::utils::Data> data) {
    this->parse(*(data.get()));
}


void mavppm::MavlinkProtocol::write(mavlink_message_t &message) {
    if (this->mWriterHandler) {
        this->mWriterHandler(this, message);
    }
}

std::shared_ptr<mavppm::utils::Data> mavppm::MavlinkProtocol::serialize(mavlink_message_t &message) {
    auto data = std::make_shared<mavppm::utils::Data>(MAVLINK_MAX_PACKET_LEN);
    int len = mavlink_msg_to_send_buffer((uint8_t *)data->getDataAddress(), &message);
    data->setDataSize(len);
    return data;
}

std::string mavppm::MavlinkProtocol::messageDescription(mavlink_message_t &message) {
    mavlink_message_info_t info = mavppm::MavlinkProtocol::mavlinkMessageInfo(message.msgid);
    std::ostrstream sstr;
    sstr<<info.name<<":"<<std::endl;
    for (int i = 0; i < info.num_fields; ++i) {
        mavlink_field_info_t field = info.fields[i];
        sstr << "\t" << field.name << ": ";
        uint8_t *valueAddr = (uint8_t *) message.payload64;
        valueAddr += field.wire_offset;
        switch (field.type) {
            case MAVLINK_TYPE_CHAR: {
                char *v = (char *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_UINT8_T: {
                uint8_t *v = (uint8_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_INT8_T: {
                int8_t *v = (int8_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_UINT16_T: {
                uint16_t *v = (uint16_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_INT16_T: {
                int16_t *v = (int16_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_UINT32_T: {
                uint32_t *v = (uint32_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_INT32_T: {
                int32_t *v = (int32_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_UINT64_T: {
                uint64_t *v = (uint64_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_INT64_T: {
                int64_t *v = (int64_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_FLOAT: {
                float_t *v = (float_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
            case MAVLINK_TYPE_DOUBLE: {
                double_t *v = (double_t *) valueAddr;
                sstr<<std::to_string(*v);
            }
                break;
        }
        sstr << std::endl;
    }
    return std::string(sstr.str());
}