include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED)
add_library(Transform
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeInfer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeCoercion.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/LambdaLifting.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ModuleTransform.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ValueVisitor.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/Resolve.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/TypeInfer.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/TypeCoercion.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/LambdaLifting.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/FunctionTransform.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/ModuleTransform.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Transform/ValueVisitor.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Externals.h")
set_target_properties(Transform PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
