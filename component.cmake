add_library(${PROJECT_NAME} STATIC ${PROJECT_NAME}.c)

if(CMAKE_CROSSCOMPILING AND CMAKE_BUILD_TYPE MATCHES Debug)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND grep "^[^\\#].*\$ " ${PROJECT_NAME}.i > ${PROJECT_NAME}.eu.c
        COMMENT "Cleaning preprocessed file"
        DEPENDS ${PROJECT_NAME}.i
        )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND clang-format ${PROJECT_NAME}.eu.c > ${PROJECT_NAME}.e.c
        COMMENT "Cleaning preprocessed file"
        DEPENDS ${PROJECT_NAME}.eu.c
        )
endif()

target_link_libraries(${PROJECT_NAME} eer ${LIBS})
target_include_directories(${PROJECT_NAME} INTERFACE .)


add_custom_target(${PROJECT_NAME}.size
    ${CMAKE_NM} --print-size --size-sort -t d lib${PROJECT_NAME}.a > ${PROJECT_NAME}.size
    DEPENDS ${PROJECT_NAME})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_test.c" AND NOT CMAKE_CROSSCOMPILING)
    set(HAL_LIBS_DEFS ${TEST_HAL_LIBS})
    list(TRANSFORM HAL_LIBS_DEFS PREPEND -DHAL_)
    list(TRANSFORM TEST_HAL_LIBS PREPEND ${EER_HAL_LIB}_)
    add_definitions(${HAL_LIBS_DEFS})

    add_executable(${PROJECT_NAME}_test ${PROJECT_NAME}_test.c)
    target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} ${LIBS} ${TEST_LIBS} ${TEST_HAL_LIBS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})



    add_test(NAME ${PROJECT_NAME}_test
        COMMAND ${PROJECT_NAME}_test)
endif()
