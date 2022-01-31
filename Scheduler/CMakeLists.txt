project(Scheduler VERSION 1.0
    DESCRIPTION "eer priority queue scheduler based on timeout"
    LANGUAGES C)

set(Scheduler_test_LIBS Clock)
include(../component.cmake)
