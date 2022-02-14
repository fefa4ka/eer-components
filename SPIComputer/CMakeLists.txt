project(SPIComputer VERSION 1.0
    DESCRIPTION "eer spi computer/master"
    LANGUAGES C)

find_program(SIGROK sigrok-cli REQUIRED)

set(LIBS Bitbang)
set(TEST_HAL_LIBS gpio timer)

include(../component.cmake)
