add_library(PCF8575 PCF8575.cpp)
target_link_libraries(PCF8575 PUBLIC PicoPeripherals)
target_include_directories(PCF8575 INTERFACE .)
target_include_directories(PCF8575 PUBLIC . PicoPeripherals)
