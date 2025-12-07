#include "core/farm_os.h"
#include <iostream>

namespace farmos {

    void helloWorld()
    {
        std::cout << "Hello world!" << std::endl;
    }

    int add(int a, int b)
    {
        return a + b;
    }

} // namespace farmos
