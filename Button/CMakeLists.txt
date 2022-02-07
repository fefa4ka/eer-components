project(Button VERSION 1.0
    DESCRIPTION "eer button with bounce filter"
    LANGUAGES C)

set(LIBS Clock)

set(TEST_LIBS IO)
set(TEST_HAL_LIBS gpio timer)

include(../component.cmake)

