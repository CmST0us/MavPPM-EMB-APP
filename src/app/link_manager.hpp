//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include "timer.hpp"
#include "mavlink_dispatcher.hpp"
#include "nocopyable.hpp"
#include "cube_heartbeat.hpp"

namespace mavppm {

class LinkManager final : public mavppm::utils::NoCopyable {
public:
    LinkManager();
    virtual ~LinkManager();

    int mMaxHeartbeatLost{5};
    int mHeartbeatIntervalSecond{1};
    int mHeartbeatLostCheckIntervalSecond{1};

    void open();
    void close();
    bool isConnected() const;
private:
    std::shared_ptr<mavppm::MavlinkDispatcherMessageHandler> _handler;
    std::shared_ptr<mavppm::CubeHeartbeat> _heartbeat;
    std::shared_ptr<mavppm::utils::Timer> _heartbeatTimeoutTimer;
    int _heartbeatCount{0};
    int _lastHeartbeatCount{0};
    int _heartbeatLostCount{0};
    bool _isConnected{false};

    void recvHeartbeat(mavlink_message_t &message);
    void checkTimeout();
};

}


