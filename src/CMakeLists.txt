add_library(speedcalc TickPoint.cpp Strings.cpp SlotTracker.cpp SignalChain.cpp RawSignalCondition.cpp DistanceCalc.cpp)

target_link_libraries(speedcalc PUBLIC delegate)
target_include_directories(speedcalc PUBLIC .)

add_library(lcd Lcd.cpp)
    target_include_directories(lcd PRIVATE ${CMAKE_SOURCE_DIR}/display)
    target_include_directories(lcd PRIVATE ${CMAKE_SOURCE_DIR}/display/u8g2/csrc)
    target_include_directories(lcd PRIVATE ${CMAKE_SOURCE_DIR}/display/u8g2/cppsrc)


target_link_libraries(lcd mcu_if speedcalc)

add_library(app  App.cpp)
target_link_libraries(app lcd speedcalc drivers mcu_if)


if(BUILD_HOST EQUAL 1)
  add_executable(speedcalc_test SignalChain_test.cpp TickPoint_test.cpp)
  target_link_libraries(speedcalc_test speedcalc gtest gtest_main Threads::Threads delegate)
  add_test(speedcalc_test speedcalc_test)
else()
  target_link_libraries(app lcd display speedcalc)
  #target_link_libraries(lcd display speedcalc)
endif()
