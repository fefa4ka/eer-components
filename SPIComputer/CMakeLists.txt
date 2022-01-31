project(SPIComputer VERSION 1.0
    DESCRIPTION "eer spi computer/master"
    LANGUAGES C)

find_program(SIGROK sigrok-cli REQUIRED)

set(SPIComputer_LIBS Bitbang)

include(../component.cmake)
