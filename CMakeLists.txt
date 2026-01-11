cmake_minimum_required(VERSION 3.10)
project(SketchAlgo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra -Wno-deprecated-declarations")

find_library(MATH_LIB m)

set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(INPUT_FILE "destination_before_integrate_test_flow.txt")
set(OUTPUT_FILE "destination_after_integrate_test_flow.txt")

add_definitions(
    -DDATA_DIR="${DATA_DIR}"
    -DINPUT_FILE="${INPUT_FILE}"
    -DOUTPUT_FILE="${OUTPUT_FILE}"
)

file(GLOB SRC_FILES "src/*.c")

foreach(SRC_FILE ${SRC_FILES})
    get_filename_component(EXEC_NAME ${SRC_FILE} NAME_WE)
    add_executable(${EXEC_NAME} ${SRC_FILE})
    target_link_libraries(${EXEC_NAME} ${MATH_LIB})
    install(TARGETS ${EXEC_NAME} DESTINATION bin)
    message(STATUS "Add executable: ${EXEC_NAME} <- ${SRC_FILE}")
endforeach()
