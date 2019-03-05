//
// Created by cmst0us on 19-3-5.
//

#pragma once

namespace mavppm {

namespace utils {

class NoCopyable {
protected:
    NoCopyable() {};

    ~NoCopyable() {};
private:
    NoCopyable(const NoCopyable &);

    const NoCopyable &operator=(const NoCopyable &);
};

}

}