//
// Created by cmst0us on 19-3-5.
//

#include <iostream>
#include "mavppm.hpp"
#include "package_manager.hpp"
#include "timer.hpp"
#include "user_default.hpp"
#include "channel_map.hpp"

int mavppm::MavPPM::USBMUXD_CONNECT_PORT = 17123;

mavppm::MavPPM::MavPPM() {
    _linkManager = std::make_shared<mavppm::LinkManager>();
}

mavppm::MavPPM::~MavPPM() {

}

void mavppm::MavPPM::setupParam(int argc, char *argv[]) {
    if (argc <= 1) return;

    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        std::string argvStr = arg;
        if (argvStr.compare("-d") == 0) {
            // Device
            char *device = argv[i + 1];
            std::string deviceStr = device;
            _PPMDevice = deviceStr;
        }
        if (argvStr.compare("-b") == 0) {
            // bandrate
            char *bandrate = argv[i + 1];
            _bandrate = atoi(bandrate);
        }
    }
}

void mavppm::MavPPM::run(int argc, char *argv[]) {
    setupParam(argc, argv);

    // start UserDeafult
    mavppm::utils::UserDefault::shared();

    // start ChannelMap;
    mavppm::ChannelMap::shared()->loadChannelConfig();

    // start PackagerManager
    std::cout<<"[MavPPM]: Start Running"<<std::endl;
    mavppm::PackageManager::shared()->setupPackageManager(mavppm::MavPPM::USBMUXD_CONNECT_PORT);

    auto p = mavppm::PackageManager::shared();
    p->mConnectingHandler = [this, p](bool isConnected) {
        if (isConnected) {
            _linkManager->open();
        } else {
            _linkManager->close();
        }
    };

    _manualControl = std::make_shared<mavppm::message_handler::ManualControl>();
    _manualControl->mPPMDevice = _PPMDevice;
    _manualControl->mBandrate = _bandrate;
    _manualControl->start();

    _commandLong = std::make_shared<mavppm::message_handler::CommandLong>();
    _commandLong->start();

    while (true) {
        ::sleep(1000);
    }
}
