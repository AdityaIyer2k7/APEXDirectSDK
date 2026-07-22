cmake_minimum_required(VERSION 3.30)
project(APEXDirectSDK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(APEXDirectSDK ${SOURCES})

# boost::asio is imported via Boost::system 
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(APEXDirectSDK PRIVATE Boost::system)

find_package(yaml-cpp REQUIRED)
target_link_libraries(APEXDirectSDK PRIVATE yaml-cpp::yaml-cpp)
