project(Chip VERSION 1.0
    DESCRIPTION "eer soc controller"
    LANGUAGES C)

set(TEST_HAL_LIBS sys gpio)

include(../component.cmake)
