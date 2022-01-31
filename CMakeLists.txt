cmake_minimum_required(VERSION 3.1)

project(eeReactLego VERSION 1.0
    DESCRIPTION "Embedded Event-driven React Component Library"
    LANGUAGES C)

set(EER_PATH "/eer")
include(eer/options.cmake)

add_subdirectory(eer)

add_subdirectory(IO)
add_subdirectory(Clock)
add_subdirectory(Scheduler)
add_subdirectory(Button)
add_subdirectory(Serial)
add_subdirectory(Bitbang)
add_subdirectory(SPIComputer)
add_subdirectory(SPIPeriphery)
