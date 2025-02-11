# include "sleep.h"

void sleep(float seconds) {
    if (seconds <= 0) return;

    auto sleepDuration = std::chrono::duration<float>(seconds);
    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - startTime < sleepDuration) {
        if (state.stopFlag.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}