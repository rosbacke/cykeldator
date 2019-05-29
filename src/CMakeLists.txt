add_library(speedcalc TickPoint.cpp Strings.cpp SlotTracker.cpp SignalChain.cpp RawSignalCondition.cpp DistanceCalc.cpp)

target_link_libraries(speedcalc PUBLIC delegate)
target_include_directories(speedcalc PUBLIC .)

if(BUILD_HOST EQUAL 1)
  add_executable(speedcalc_test SignalChain_test.cpp TickPoint_test.cpp)
  target_link_libraries(speedcalc_test speedcalc gtest gtest_main Threads::Threads delegate)
  add_test(speedcalc_test speedcalc_test)
else()
endif()
