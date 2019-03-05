//
// Created by CmST0us on 2019/3/5.
//

#pragma once

#include "nocopyable.hpp"

namespace mavppm {

class Application : public utils::NoCopyable {
public:
    Application();
    virtual ~Application();
    virtual void run();
};

}

