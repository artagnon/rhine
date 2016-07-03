find_package(LLVM REQUIRED)

add_executable(CLI
"${CMAKE_CURRENT_SOURCE_DIR}/Rhine.cpp")

add_custom_command(TARGET CLI
POST_BUILD COMMAND "xcrun" "llvm-profdata" "merge"
"-o" "${PROJECT_BINARY_DIR}/Coverage.profdata"
"${PROJECT_BINARY_DIR}/default.profraw")

set_target_properties(CLI PROPERTIES
OUTPUT_NAME "rhine"
RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set_target_properties(CLI PROPERTIES
LINKER_LANGUAGE CXX
COMPILE_FLAGS "${LLVM_CXXFLAGS}")

target_link_libraries(CLI
Toplevel)
