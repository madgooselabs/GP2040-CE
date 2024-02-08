add_compile_options(-fdata-sections -ffunction-sections)
add_link_options(-Wl,--gc-sections)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Activate some compiler / linker options to aid us with diagnosing stack space issues in Debug builds
  add_compile_options(-fstack-usage -Wstack-usage=500)
  add_compile_definitions(PICO_USE_STACK_GUARDS=1)
endif()

add_library(GPGFX 
    drivers/obd_ssd1306.cpp
    drivers/tiny_ssd1306.cpp

    ui/elements/GPWidget.cpp
    ui/elements/GPButton.cpp
    ui/elements/GPLever.cpp
    ui/elements/GPLabel.cpp
    ui/elements/GPScreen.cpp
    ui/elements/GPSprite.cpp

    ui/screens/ButtonLayoutScreen.cpp
    ui/screens/ConfigScreen.cpp
    ui/screens/MainMenuScreen.cpp
    ui/screens/SplashScreen.cpp

    GPGFX.cpp
    GPGFX_UI.cpp
)
target_link_libraries(GPGFX PUBLIC 
    pico_stdlib
    tinyusb_host
    PicoPeripherals
    OneBitDisplay
    nanopb
)
target_include_directories(GPGFX PUBLIC
    ${PROTO_OUTPUT_DIR} 
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/headers/addons
    ${CMAKE_SOURCE_DIR}/headers/gamepad
    ${CMAKE_SOURCE_DIR}/headers/gamepad/descriptors
    ${CMAKE_SOURCE_DIR}/lib/TinyUSB_Gamepad/src
    ${CMAKE_SOURCE_DIR}/lib/TinyUSB_Gamepad/src/xid_driver
    ${CMAKE_SOURCE_DIR}/configs/${GP2040_BOARDCONFIG}
    ${PROTO_OUTPUT_DIR} 
    ${CMAKE_BINARY_DIR}/headers
    . 
    ui
    ui/elements
    ui/screens
    drivers
)
