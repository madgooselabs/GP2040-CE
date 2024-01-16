set(lib_name MapleLib)
set(lib_src ${CMAKE_CURRENT_LIST_DIR}/MaplePad/src)
add_library(${lib_name} )

pico_generate_pio_header(${lib_name} ${lib_src}/maple.pio)

target_sources(${lib_name} PRIVATE ${lib_src}/maple.c ${lib_src}/state_machine.c ${lib_src}/format.c ${lib_src}/display.c ${lib_src}/sh8601.c ${lib_src}/ssd1331.c ${lib_src}/ssd1306.c ${lib_src}/st7789.c ${lib_src}/font.c ${lib_src}/menu.c)


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



