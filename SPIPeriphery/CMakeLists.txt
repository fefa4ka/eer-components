project(SPIPeriphery VERSION 1.0
    DESCRIPTION "eer spi periphery/slave"
    LANGUAGES C)

set(LIBS lr)
set(TEST_LIBS SPIComputer)
set(TEST_HAL_LIBS gpio timer)

include(../component.cmake)
