add_library(mcu_if  mcuaccess.cpp isr.cpp)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/)
target_include_directories(mcu_if PUBLIC .)
target_compile_definitions(mcu_if PUBLIC -DSTM32F10X_MD=1)

target_link_libraries(mcu_if PUBLIC delegate)

add_library(drivers usart.cpp timer.cpp TimeSource.cpp)
target_link_libraries(drivers PRIVATE mcu_if)

if(BUILD_HOST EQUAL 1)
  add_executable(mcu_if_test mcuif_test.cpp israccess_test.cpp timer_test.cpp)
  target_link_libraries(mcu_if_test drivers mcu_if gtest gtest_main Threads::Threads delegate)
  add_test(mcu_if_test mcu_if_test)
else()
endif()

target_link_libraries(drivers PUBLIC speedcalc)
