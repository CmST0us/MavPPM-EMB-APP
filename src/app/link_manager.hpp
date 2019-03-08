//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include "mavlink_dispatcher.hpp"
#include "nocopyable.hpp"
#include "cube_heartbeat.hpp"

namespace mavppm {

class LinkManager final : public mavppm::utils::NoCopyable, public mavppm::utils::ITrackable {
public:
    LinkManager();
    virtual ~LinkManager();

    void open();
    void close();
    bool isConnected() const;

    virtual const std::string uniqueID() override;

private:
    std::string _uniqueID{""};
    std::shared_ptr<mavppm::MavlinkDispatcherMessageHandler> _handler;
    std::shared_ptr<mavppm::CubeHeartbeat> _heartbeat;

    int _heartBeatCount{0};
    bool _isConnected{false};

    void recvHeartbeat(mavlink_message_t &message);
};

}


