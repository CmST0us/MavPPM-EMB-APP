//
// Created by CmST0us on 2019/3/7.
//

#pragma once

#include <functional>
#include "runloop.hpp"
#include "nocopyable.hpp"

namespace mavppm {

class CubeHeartbeat final : public mavppm::utils::NoCopyable {
public:
    CubeHeartbeat();
    virtual ~CubeHeartbeat();

    float mHeatbeatInterval{1.0};

    void start();
    void stop();

private:
    std::unique_ptr<mavppm::utils::Runloop> _runloop;
};

}
