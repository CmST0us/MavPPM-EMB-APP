//
// Created by CmST0us on 2019/3/14.
//

#pragma once

#include <string>
#include <iostream>
#include <plist/plist.h>
#include <memory>
#include <fstream>
#include "nocopyable.hpp"

#define kUserDefaultPlistRootKey "root"
#define kUserDefaultConfigFileName "config.plist"

#define kUserDefaultChannelBindProfileKey "kUserDefaultChannelBindProfileKey"
#define kUserDefaultChannelBindProfileThrottleKey "Throttle"
#define kUserDefaultChannelBindProfileRollKey "Roll"
#define kUserDefaultChannelBindProfilePitchKey "Pitch"
#define kUserDefaultChannelBindProfileYawKey "Yaw"
#define kUserDefaultChannelBindProfileButton1Key "Button1"
#define kUserDefaultChannelBindProfileButton2Key "Button2"
#define kUserDefaultChannelBindProfileButton3Key "Button3"

namespace mavppm {
namespace utils {

class UserDefault : public mavppm::utils::NoCopyable {
protected:
    void setup() {
        // try open config.plist
        std::fstream f;
        f.open(kUserDefaultConfigFileName, std::fstream::out | std::fstream::in);
        if (!f.is_open()) {
            f.close();

            std::ofstream of(kUserDefaultConfigFileName, std::fstream::out);
            std::cout<<"[MavPPM][UserDeafult]: Could not find config.plist, create"<<std::endl;

            of.close();

            setup();
            return;
        }

        f.seekg(0, std::ios::end);
        int fileLen = f.tellg();
        f.seekg(0, std::ios::beg);
        if (fileLen == 0) {
            _rootNode = plist_new_dict();
            plist_t rootDict = plist_new_dict();
            plist_dict_set_item(_rootNode, kUserDefaultPlistRootKey, rootDict);
            char *xmlBuf = nullptr;
            uint32_t len = 0;
            plist_to_xml(_rootNode, &xmlBuf, &len);
            f.write(xmlBuf, len);
            delete(xmlBuf);
            f.close();
        } else {
            auto buffer = std::unique_ptr<uint8_t>(new uint8_t[fileLen]);
            f.read((char *)buffer.get(), fileLen);

            plist_t rootNode = nullptr;
            plist_from_xml((const char *)buffer.get(), fileLen, &rootNode);
            if (rootNode == nullptr) {
                std::cout<<"[MavPPM][UserDefault]: Could not parse config.plist"<<std::endl;
                _rootNode = plist_new_dict();
                plist_t rootDict = plist_new_dict();
                plist_dict_set_item(_rootNode, kUserDefaultPlistRootKey, rootDict);
            } else {
                _rootNode = rootNode;
            }

            f.close();
        }
//        _modifyNode = plist_new_dict();
    }

public:
    static UserDefault *shared() {
        static std::shared_ptr<mavppm::utils::UserDefault> userDefault = {nullptr};
        if (userDefault) {
            return userDefault.get();
        }
        userDefault= std::make_shared<mavppm::utils::UserDefault>();
        userDefault->setup();
        return userDefault.get();
    }

    void commit() {
//        plist_t dict = nullptr;
//        plist_dict_get_item(dict, mavppm::utils::UserDefault::PLIST_ROOT_KEY);
//        plist_dict_merge(&dict, _modifyNode);

        std::ofstream f(kUserDefaultConfigFileName, std::ofstream::out);
        char *xmlBuf = nullptr;
        uint32_t len = 0;
        plist_to_xml(_rootNode, &xmlBuf, &len);
        f.write(xmlBuf, len);
        delete(xmlBuf);
        f.close();

//        if (_modifyNode) {
//            plist_free(_modifyNode);
//        }
//        _modifyNode = plist_new_dict();
    }

    /* [TODO]: 确定临时修改没有commit 读取方案
    void revert() {
        if (_modifyNode) {
            plist_free(_modifyNode);
        }
        _modifyNode = plist_new_dict();
    }*/

    void set(std::string key, plist_t obj) {
        plist_t dict = nullptr;
        dict = plist_dict_get_item(_rootNode, kUserDefaultPlistRootKey);
        plist_dict_set_item(dict, key.c_str(), obj);
    }

    plist_t get(std::string key) {
        plist_t dict = nullptr;
        dict = plist_dict_get_item(_rootNode, kUserDefaultPlistRootKey);
        if (dict == nullptr) {
            return nullptr;
        }
        return plist_dict_get_item(dict, key.c_str());
    }

private:
    plist_t  _rootNode{nullptr};
//    plist_t  _modifyNode{nullptr};

};

}
}
