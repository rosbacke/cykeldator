add_library(speedcalc TickPoint.cpp Strings.cpp SlotTracker.cpp SignalChain.cpp RawSignalCondition.cpp DistanceCalc.cpp)

target_link_libraries(speedcalc PUBLIC delegate)
target_include_directories(speedcalc PUBLIC .)
