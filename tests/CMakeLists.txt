add_executable(FirewallManagerTests
    test_firewall.cpp
    ../src/firewall/firewall.cpp
    ../src/firewall/firewall.h
    ../src/logging/logger.cpp
    ../src/logging/logger.h
)

target_link_libraries(FirewallManagerTests PRIVATE
    GTest::gtest_main
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Qt6::Widgets
)

if(WIN32)
    target_link_libraries(FirewallManagerTests PRIVATE
        ole32
        oleaut32
    )
endif()

target_include_directories(FirewallManagerTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

include(GoogleTest)
gtest_discover_tests(FirewallManagerTests)
