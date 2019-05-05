cmake_minimum_required(VERSION 3.10)


add_subdirectory(src)

add_library(delegate INTERFACE)
target_include_directories(delegate INTERFACE thirdparty/delegate/include)

if(BUILD_TARGET)

else()
	add_executable(interpreter src/interpreter.cpp)
	target_link_libraries(interpreter PRIVATE delegate speedcalc fmt)
endif()
