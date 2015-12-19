find_package(LLVM REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/src/Parse")
include_directories("${PROJECT_SOURCE_DIR}/gtest/include")
include_directories("${LLVM_INCLUDE_DIRS}")

add_executable(UnitTest
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IRTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/FunctionTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeInferTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeCoerceTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Scope2BlockTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ResolveTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BinaryArithTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/DollarOperatorTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/CallTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/FunctionPointerTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ExternalsTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BasicBlockTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/StoreTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ModuleTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/LambdaTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/DiagnosticTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IfTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ReturnTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/UseTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BitcodeTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ValgrindTest.cpp")
set_target_properties(UnitTest PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(UnitTest
  gtest_main
  Toplevel)
