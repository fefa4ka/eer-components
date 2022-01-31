add_library(${PROJECT_NAME} STATIC ${PROJECT_NAME}.c)
target_link_libraries(${PROJECT_NAME} eer ${${PROJECT_NAME}_LIBS})
target_include_directories(${PROJECT_NAME} INTERFACE .)

configure_file(${EER_LIB_PATH}/include/version.h.in version.h)

add_custom_target(${PROJECT_NAME}.size
    ${CMAKE_NM} --print-size --size-sort -t d lib${PROJECT_NAME}.a > ${PROJECT_NAME}.size
    DEPENDS ${PROJECT_NAME})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_test.c" AND NOT CMAKE_CROSSCOMPILING)
    add_executable(${PROJECT_NAME}_test ${PROJECT_NAME}_test.c)
    target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} ${${PROJECT_NAME}_LIBS} ${${PROJECT_NAME}_test_LIBS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

    add_test(NAME ${PROJECT_NAME}_test
        COMMAND ${PROJECT_NAME}_test)
endif()
