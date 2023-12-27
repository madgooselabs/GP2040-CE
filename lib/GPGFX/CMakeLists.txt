add_library(GPGFX 
    GPGFX.cpp
)
target_link_libraries(GPGFX PUBLIC 
    pico_stdlib 
    PicoPeripherals
    OneBitDisplay
)
target_include_directories(GPGFX INTERFACE .)
target_include_directories(GPGFX PUBLIC . PicoPeripherals)
