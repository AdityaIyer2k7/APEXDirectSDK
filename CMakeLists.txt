cmake_minimum_required(VERSION 3.30)
project(APEXDirectSDK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find all files, and remove testmain.cpp if 
file(GLOB_RECURSE SOURCES "src/*.cpp")
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    message(STATUS "USING RELEASE VERSION: WILL NOT GENERATE TESTS NOR EXECUTABLE")
    list(REMOVE_ITEM SOURCES "*/src/tests.cpp")
else()
    add_executable(APEXDirectSDK ${SOURCES})
endif()



find_package(Boost REQUIRED COMPONENTS system)
find_package(yaml-cpp REQUIRED)

if(TARGET APEXDirectSDK)
    target_link_libraries(APEXDirectSDK PRIVATE 
        Boost::system
        yaml-cpp::yaml-cpp
        Python::Python
    )
endif()



# Python needed for pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

pybind11_add_module(APEXDirectPySDK ${SOURCES})

# Link  dependencies to Python library
target_link_libraries(APEXDirectPySDK PRIVATE 
    Boost::system 
    yaml-cpp::yaml-cpp
    Python::Python
)
