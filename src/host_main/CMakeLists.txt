if(BUILD_HOST EQUAL 1)
  add_executable(host_main main.cpp)
  target_link_libraries(host_main app lcd speedcalc drivers mcu_if fmt)  
endif()
