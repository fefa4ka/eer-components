project(Button VERSION 1.0
    DESCRIPTION "eer button with bounce filter"
    LANGUAGES C)

set(Button_LIBS Clock)
set(Button_test_LIBS IO)

include(../component.cmake)

