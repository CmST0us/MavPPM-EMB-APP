//
// Created by cmst0us on 19-3-5.
//

#include <iostream>
#include "mavppm.hpp"
#include "package_manager.hpp"
#include "timer.hpp"

int mavppm::MavPPM::USBMUXD_CONNECT_PORT = 17123;

mavppm::MavPPM::MavPPM() {
    _linkManager = std::make_shared<mavppm::LinkManager>();
}

mavppm::MavPPM::~MavPPM() {

}


void mavppm::MavPPM::run() {
    std::cout<<"[MavPPM]: Start Running"<<std::endl;
    mavppm::PackageManager::shared()->setupPackageManager(mavppm::MavPPM::USBMUXD_CONNECT_PORT);

    auto p = mavppm::PackageManager::shared();
    p->mConnectedHandler = [this, p]() {
        _linkManager->open();
    };

    while (true) {
        ::sleep(1000);
    }
}
