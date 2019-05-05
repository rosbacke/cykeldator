cmake_minimum_required(VERSION 3.10)
project ( Test C CXX )

if(BUILD_TARGET)

else()
	enable_testing()
	set(GTEST_ROOT /usr/local)
	find_package(GTest REQUIRED)
	set(THREADS_PREFER_PTHREAD_FLAG 1)
	find_package( Threads REQUIRED )
endif()

add_subdirectory(src)
add_subdirectory(mcu_src)

add_library(delegate INTERFACE)
target_include_directories(delegate INTERFACE thirdparty/delegate/include)


if(BUILD_TARGET)

else()
	add_executable(interpreter src/interpreter.cpp)
	target_link_libraries(interpreter PRIVATE delegate speedcalc fmt)
endif()


