cmake_minimum_required(VERSION 3.20)
project(FirewallManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# FetchContent for other dependencies
include(FetchContent)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# nlohmann_json
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Source files
set(SOURCES
    src/main.cpp
    src/firewall/firewall.cpp
    src/firewall/firewall.h
    src/monitoring/monitor.cpp
    src/monitoring/monitor.h
    src/logging/logger.cpp
    src/logging/logger.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/mainwindow.ui
    resources/resources.qrc
)

# Windows Manifest for UAC elevation
if(WIN32)
    set(MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.manifest")
endif()

add_executable(FirewallManager ${SOURCES} ${MANIFEST})

target_link_libraries(FirewallManager PRIVATE
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(FirewallManager PRIVATE
        ole32
        oleaut32
        iphlpapi
    )
endif()

target_include_directories(FirewallManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Testing
enable_testing()
add_subdirectory(tests)
