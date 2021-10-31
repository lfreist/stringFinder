// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include "../src/CpuTimer.hpp"

// ____________________________________________________________________________
TEST(CpuTimerTest, cpuTimerStartStop) {
    CpuTimer timer;
    {
        timer.start();
        ASSERT_TRUE(timer._running);
        ASSERT_TRUE(timer._started);
    }
    {
        timer.stop();
        ASSERT_FALSE(timer._running);
        ASSERT_TRUE(timer._started);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
