#include "testcls.h"
#include <iostream>

TestCls::TestCls() {}

void TestCls::Print() {
    std::cout << "TestCls Print" << std::endl;
    printf("--- %s:%d ---", __func__, __LINE__);
}
