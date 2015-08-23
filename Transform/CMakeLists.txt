include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED)
add_library(Transform
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeInfer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeCoercion.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/LambdaLifting.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Resolve.cpp")
set_target_properties(Transform PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(Transform
  Pass)