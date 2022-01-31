project(Bitbang VERSION 1.0
    DESCRIPTION "eer gpio bitbang"
    LANGUAGES C)

find_program(SIGROK sigrok-cli REQUIRED)

set(Bitbang_LIBS Clock lr)
set(Bitbang_test_LIBS IO)

include(../component.cmake)

add_test(NAME ${PROJECT_NAME}_test_gpio_output
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_test.sh ${SIGROK} ${CMAKE_CURRENT_BINARY_DIR}/profiler.vcd
)
