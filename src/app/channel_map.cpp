//
// Created by CmST0us on 2019/3/14.
//

#include <memory>
#include "channel_map.hpp"
#include "user_default.hpp"

mavppm::ChannelMap* mavppm::ChannelMap::shared() {
    static std::shared_ptr<mavppm::ChannelMap> shared = nullptr;
    if (shared) {
        return shared.get();
    }
    shared = std::make_shared<mavppm::ChannelMap>();
    return shared.get();
}

void mavppm::ChannelMap::loadChannelConfig() {
    plist_t bindDict = mavppm::utils::UserDefault::shared()->get(kUserDefaultChannelBindProfileKey);
    if (bindDict == nullptr) {
        this->useDefaultConfig();
        return;
    }

    // Throttle
    uint64_t v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
    plist_t t = nullptr;

    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileThrottleKey);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Throttle] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Roll
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileRollKey);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Roll] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Pitch
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfilePitchKey);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Pitch] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Yaw
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileYawKey);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Yaw] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Button 1
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileButton1Key);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Button1] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Button 2
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileButton2Key);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Button2] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }

    // Button 3
    t = plist_dict_get_item(bindDict, kUserDefaultChannelBindProfileButton3Key);
    if (t != nullptr) {
        plist_get_uint_val(t, &v);
        _channelMap[mavppm::ChannelMap::ChannelType::Button3] = (mavppm::ChannelMap::ChannelNumber)(mavppm::ChannelMap::ChannelNumber)v;
        v = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;
        t = nullptr;
    }
}

void mavppm::ChannelMap::useDefaultConfig() {
    _channelMap[mavppm::ChannelMap::ChannelType::Throttle] = mavppm::ChannelMap::ChannelNumber::_4;
    _channelMap[mavppm::ChannelMap::ChannelType::Pitch] = mavppm::ChannelMap::ChannelNumber::_1;
    _channelMap[mavppm::ChannelMap::ChannelType::Roll] = mavppm::ChannelMap::ChannelNumber::_2;
    _channelMap[mavppm::ChannelMap::ChannelType::Yaw] = mavppm::ChannelMap::ChannelNumber::_3;
    _channelMap[mavppm::ChannelMap::ChannelType::Button1] = mavppm::ChannelMap::ChannelNumber::_5;
    _channelMap[mavppm::ChannelMap::ChannelType::Button2] = mavppm::ChannelMap::ChannelNumber::_6;
    _channelMap[mavppm::ChannelMap::ChannelType::Button3] = mavppm::ChannelMap::ChannelNumber::_7;

    saveConfig();
}

void mavppm::ChannelMap::saveConfig() {
    plist_t channelMapPlistObj = plist_new_dict();
    plist_t channelNumberPlistObj = nullptr;

    uint64_t channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Throttle];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileThrottleKey, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Roll];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileRollKey, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Pitch];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfilePitchKey, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Yaw];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileYawKey, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Button1];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileButton1Key, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Button2];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileButton2Key, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    channelNumber = (uint64_t)_channelMap[mavppm::ChannelMap::ChannelType::Button3];
    channelMapPlistObj = plist_new_uint(channelNumber);
    plist_dict_set_item(channelMapPlistObj, kUserDefaultChannelBindProfileButton3Key, channelMapPlistObj);
    channelNumber = (uint64_t)mavppm::ChannelMap::ChannelNumber::Unbind;

    mavppm::utils::UserDefault::shared()->set(kUserDefaultChannelBindProfileKey, channelMapPlistObj);
    mavppm::utils::UserDefault::shared()->commit();
}

void mavppm::ChannelMap::bind(ChannelNumber number, ChannelType type) {
    _channelMap[type] = number;
}

mavppm::ChannelMap::ChannelNumber mavppm::ChannelMap::channel(ChannelType type) {
    return _channelMap[type];
}