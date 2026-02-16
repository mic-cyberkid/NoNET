#include <gtest/gtest.h>
#include "firewall/firewall.h"
#include "logging/logger.h"

class FirewallTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::init();
    }
};

TEST_F(FirewallTest, CanInstantiate) {
    Firewall firewall;
    // Basic instantiation test
}

#ifndef _WIN32
TEST_F(FirewallTest, ReturnsNotSupportedOnLinux) {
    Firewall firewall;
    auto err = firewall.setDefaultOutboundBlock();
    EXPECT_EQ(err, std::errc::not_supported);

    err = firewall.addAllowRule(L"test", L"test");
    EXPECT_EQ(err, std::errc::not_supported);

    err = firewall.removeRule(L"test");
    EXPECT_EQ(err, std::errc::not_supported);

    auto rules = firewall.listRules();
    EXPECT_TRUE(rules.empty());
}
#endif

TEST_F(FirewallTest, LoggerWorks) {
    Logger::info("Test log message");
    EXPECT_NE(Logger::get(), nullptr);
}
