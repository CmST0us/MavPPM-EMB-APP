//
// Created by CmST0us on 2019/3/8.
//

#pragma once

namespace mavppm {

namespace utils {

// 对象创建时先调用一次uniqueID
// 对象释放时释放uniqueID对应的资源
class ITrackable {
    virtual const std::string uniqueID() = 0;
};

}

}