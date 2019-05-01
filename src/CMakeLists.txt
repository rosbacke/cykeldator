add_library(speedcalc TickPoint.cpp Strings.cpp SlotTracker.cpp SignalChain.cpp RawSignalCondition.cpp, DistanceCalc.cpp)

target_include_directories(speedcalc PRIVATE ../../delegate/include)
