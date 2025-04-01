#include "daily.h"


void test(const Env &e) {
    env = e;

    auto clicker = new Clicker("测试/123.png", {});
    clicker->locate({
                            .startUntilList={new Image("测试/123.png", {})}
                    })->end();
}