project(Servo VERSION 1.0
    DESCRIPTION "eer gpio servo controller"
    LANGUAGES C)

find_program(SIGROK sigrok-cli REQUIRED)

set(TEST_LIBS Clock Scheduler)
set(TEST_HAL_LIBS gpio timer)

include(../component.cmake)
