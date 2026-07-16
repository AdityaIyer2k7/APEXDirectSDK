cmake_minimum_required(VERSION 3.30)
project(APEXDirectSDK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(APEXDirectSDK ${SOURCES})

# Find Boost (for Boost.Asio) and link Boost.System
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
	target_include_directories(APEXDirectSDK PRIVATE ${Boost_INCLUDE_DIRS})
	if(TARGET Boost::system)
		target_link_libraries(APEXDirectSDK PRIVATE Boost::system)
	else()
		target_link_libraries(APEXDirectSDK PRIVATE ${Boost_LIBRARIES})
	endif()
else()
	message(FATAL_ERROR "Boost not found. Please install Boost and try again.")
endif()
