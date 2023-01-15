cmake_minimum_required(VERSION 3.1)

project(eeReactLego VERSION 1.0
    DESCRIPTION "Embedded Event-driven React Component Library"
    LANGUAGES C)

set(EER_PATH "/eer")
include(eer/options.cmake)

add_subdirectory(third_party/linked_ring)
add_subdirectory(eer)

# Periphery
add_subdirectory(Chip)
add_subdirectory(IO)

# Abstract
add_subdirectory(Clock)
add_subdirectory(Button)
add_subdirectory(Serial)
add_subdirectory(Scheduler)
add_subdirectory(Menu)
add_subdirectory(Bitbang)

# Interfaces
add_subdirectory(SPIComputer)
add_subdirectory(SPIPeriphery)

# Controllers
add_subdirectory(Servo)
