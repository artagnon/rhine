find_package(LLVM REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/src/Parse")
include_directories("${PROJECT_SOURCE_DIR}/gtest/include")
include_directories("${LLVM_INCLUDE_DIRS}")

add_executable(UnitTest
  "${CMAKE_CURRENT_SOURCE_DIR}/tParse.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tIR.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tFunction.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tTypeInfer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tTypeCoerce.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tScope2Block.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tResolve.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tBinaryArith.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tPointer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tDollarOperator.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tCall.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tFunctionPointer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tExternals.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tBasicBlock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tStore.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tModule.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tLambda.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tUnbound.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tDiagnostic.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tOvershadow.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tIf.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tReturn.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tUse.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tOverload.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tTensor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tBitcode.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tCLI.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tValgrind.cpp")

set_target_properties(UnitTest PROPERTIES
  OUTPUT_NAME "unittest"
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")

target_link_libraries(UnitTest
  gtest_main
  Toplevel)
