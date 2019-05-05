if(BUILD_HOST EQUAL 1)
else()
  add_library(arm_startup_code  startup_stm32f10x_md.s system_stm32f10x.c)
  target_include_directories(arm_startup_code PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport)
  target_include_directories(arm_startup_code PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x)
  target_compile_definitions(arm_startup_code PUBLIC -DSTM32F10X_MD=1)
endif()

add_library(mcu_if  mcuaccess.cpp isr.cpp)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/)
target_include_directories(mcu_if PUBLIC .)
target_compile_definitions(mcu_if PUBLIC -DSTM32F10X_MD=1)
target_link_libraries(mcu_if PUBLIC delegate)

add_library(drivers usart.cpp timer.cpp)
target_link_libraries(drivers PRIVATE mcu_if)


if(BUILD_HOST EQUAL 1)
  add_executable(mcu_if_test mcuif_test.cpp)
  target_link_libraries(mcu_if_test mcu_if drivers gtest gtest_main Threads::Threads delegate)
  add_test(mcu_if_test mcu_if_test)
else()
  add_library(mcu_if_app main.cpp App.cpp Lcd.cpp Lcd.cpp)
  target_link_libraries(mcu_if_app PUBLIC mcu_if drivers arm_startup_code display)
endif()

target_link_libraries(drivers PUBLIC speedcalc)
