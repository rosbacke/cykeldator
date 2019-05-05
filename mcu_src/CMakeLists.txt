

add_executable(mcu_if mcuif_test.cpp mcuaccess.cpp)

target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/)
target_compile_definitions(mcu_if PRIVATE -DSTM32F10X_MD=1)

target_link_libraries(mcu_if gtest gtest_main Threads::Threads delegate)

add_test(mcu_if mcu_if)
