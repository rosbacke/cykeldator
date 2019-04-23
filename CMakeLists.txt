cmake_minimum_required(VERSION 3.10)


add_subdirectory(src)



add_executable(interpreter src/interpreter.cpp)

target_include_directories(interpreter PRIVATE ../delegate/include)

target_link_libraries(interpreter PRIVATE speedcalc fmt)
