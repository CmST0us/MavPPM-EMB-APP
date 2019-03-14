//
// Created by CmST0us on 2019/3/14.
//

#pragma once

#include <map>
#include "nocopyable.hpp"

namespace mavppm {

class ChannelMap final : public mavppm::utils::NoCopyable {
public:
    enum class ChannelNumber {
        _1 = 1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,

        Count,
        Unbind = -1
    };

    enum class ChannelType {
        Throttle = 1,
        Roll,
        Pitch,
        Yaw,
        Button1,
        Button2,
        Button3,

        Count,
        Unbind = -1
    };

public:
    static ChannelMap *shared();

    void bind(ChannelNumber number, ChannelType type);
    ChannelNumber channel(ChannelType type);

    void loadChannelConfig();
    void saveConfig();
private:
    std::map<ChannelType, ChannelNumber> _channelMap;
    void useDefaultConfig();
};

}
