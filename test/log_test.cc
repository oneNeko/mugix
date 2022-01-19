#include <gtest/gtest.h>

#define private public
#include "../server/http/http_response.h"
#include "../server/log/log.h"

// GetTime
TEST(GetTime_test, GetTime_test_case0)
{
    string date, full_time;
    auto logger=Log::GetInstance();
    auto res = logger->GetTime(date, full_time);
    
    ASSERT_EQ(res, true);
}
#undef private