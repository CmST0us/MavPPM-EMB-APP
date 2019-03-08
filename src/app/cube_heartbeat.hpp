//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include <string>
#include <functional>
#include "runloop.hpp"
#include "nocopyable.hpp"
#include "itrackable.hpp"

namespace mavppm {

class CubeHeartbeat final : public mavppm::utils::NoCopyable, public mavppm::utils::ITrackable {
public:
    CubeHeartbeat();
    virtual ~CubeHeartbeat();

    float mHeatbeatInterval{1.0};

    void start();
    void stop();

    virtual const std::string uniqueID() override;

private:
    std::string _uniqueID{""};
    std::unique_ptr<mavppm::utils::Runloop> _runloop;
};

}
