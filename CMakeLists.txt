project(bike-computer C CXX ASM)
cmake_minimum_required(VERSION 3.10)

if(BUILD_TARGET EQUAL 1)
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Os -g -mthumb -mcpu=cortex-m3 -nostdlib -ffunction-sections -fdata-sections")
    SET(CMAKE_ASM_FLAGS  "${CMAKE_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -T${CMAKE_SOURCE_DIR}/src/target_main/stm32_flash.ld")

    add_library(display_st STATIC IMPORTED)
    set_target_properties(display_st PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/display/display.a)

    add_library(display src/display.cpp)
    target_link_libraries(display display_st)
    target_include_directories(display PUBLIC ${CMAKE_SOURCE_DIR}/display)
    target_include_directories(display PUBLIC ${CMAKE_SOURCE_DIR}/display/u8g2/csrc)
    target_include_directories(display PUBLIC ${CMAKE_SOURCE_DIR}/display/u8g2/cppsrc)

    add_subdirectory(src/target_main)

else()
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Og -g")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

    enable_testing()
    set(GTEST_ROOT /usr/local)
    find_package(GTest REQUIRED)
    set(THREADS_PREFER_PTHREAD_FLAG 1)
    find_package( Threads REQUIRED )
    
    add_subdirectory(src/host_main)
endif()

add_subdirectory(src/interpreter)
add_subdirectory(src)
add_subdirectory(mcu_src)

add_library(delegate INTERFACE)
target_include_directories(delegate INTERFACE thirdparty/delegate/include)
