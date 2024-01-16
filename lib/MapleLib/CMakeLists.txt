set(lib_name MapleLib)
set(lib_src ${CMAKE_CURRENT_LIST_DIR}/MaplePad/src)
add_library(${lib_name} )

pico_generate_pio_header(${lib_name} ${lib_src}/maple.pio)

target_sources(${lib_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/maplelib.cpp ${lib_src}/state_machine.c ${lib_src}/format.c ${lib_src}/menu.c)

target_include_directories(${lib_name} PUBLIC
        .
        ${lib_src}
)

target_link_libraries(${lib_name} PRIVATE
        pico_stdlib
        pico_multicore
        pico_time
        hardware_adc
        hardware_pio
        hardware_dma
        hardware_pwm
        hardware_timer
        hardware_i2c
        hardware_spi
        hardware_flash
)



