


add_library(mcu_if  mcuaccess.cpp isr.cpp)

target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3)
target_include_directories(mcu_if PUBLIC ../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/)
target_include_directories(mcu_if PUBLIC .)
target_compile_definitions(mcu_if PUBLIC -DSTM32F10X_MD=1)


if(BUILD_HOST EQUAL 1)
  add_executable(mcu_if_test mcuif_test.cpp)
  target_link_libraries(mcu_if_test mcu_if gtest gtest_main Threads::Threads delegate)
  add_test(mcu_if_test mcu_if_test)
else()
  add_library(arm_startup_code  startup_stm32f10x_md.s system_stm32f10x.c)
  target_include_directories(arm_startup_code PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport)
  target_include_directories(arm_startup_code PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x)
  target_compile_definitions(arm_startup_code PRIVATE -DSTM32F10X_MD=1)
  

  add_library(mcu_if_app main.cpp App.cpp usart.cpp Lcd.cpp timer.cpp)
  target_link_libraries(mcu_if_app PUBLIC mcu_if arm_startup_code)
  target_include_directories(mcu_if PUBLIC ../display)
  target_include_directories(mcu_if PUBLIC ../display/u8g2/csrc)
  target_include_directories(mcu_if PUBLIC ../display/u8g2/cppsrc)
  
endif()

target_link_libraries(mcu_if PUBLIC speedcalc)
