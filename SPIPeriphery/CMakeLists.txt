project(SPIPeriphery VERSION 1.0
    DESCRIPTION "eer spi periphery/slave"
    LANGUAGES C)

set(SPIPeriphery_LIBS lr)
set(TEST_LIBS SPIComputer)

include(../component.cmake)
