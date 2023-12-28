add_library(GPGFX 
    GPGFX.cpp
    drivers/obd_ssd1306.cpp
    drivers/tiny_ssd1306.cpp
)
target_link_libraries(GPGFX PUBLIC 
    pico_stdlib 
    PicoPeripherals
    OneBitDisplay
)
target_include_directories(GPGFX INTERFACE . drivers)
target_include_directories(GPGFX PUBLIC 
    . 
    drivers
    PicoPeripherals
)
