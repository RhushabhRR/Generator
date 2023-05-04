#include <iostream>

#include "CanTpGenerator.h"

int main()
{
    volatile uint8_t val = 56;
    std::cout << "Value = " << val << std::endl;

    return 0;
}
