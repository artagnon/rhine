# Find Clang
#
# It defines the following variables
# CLANG_FOUND        - True if Clang found.
# CLANG_INCLUDE_DIRS - where to find Clang include files
# CLANG_LIBS         - list of clang libs

if(NOT LLVM_INCLUDE_DIRS OR NOT LLVM_LIBRARY_DIRS)
   message(FATAL_ERROR "No LLVM and Clang support requires LLVM")
else()
  macro(FIND_AND_ADD_CLANG_LIB _libname_)
    find_library(CLANG_${_libname_}_LIB ${_libname_} ${LLVM_LIBRARY_DIRS})
    set(CLANG_LDFLAGS "-L${CLANG_LIBRARY_DIRS}")
    if(CLANG_${_libname_}_LIB)
      set(CLANG_LIBS ${CLANG_LIBS} ${CLANG_${_libname_}_LIB})
    endif()
  endmacro()

  # Clang shared library provides just the limited C interface, so it
  # can not be used.  We look for the static libraries.
  FIND_AND_ADD_CLANG_LIB(clangFrontend)
  FIND_AND_ADD_CLANG_LIB(clangDriver)
  FIND_AND_ADD_CLANG_LIB(clangCodeGen)
  FIND_AND_ADD_CLANG_LIB(clangEdit)
  FIND_AND_ADD_CLANG_LIB(clangSema)
  FIND_AND_ADD_CLANG_LIB(clangChecker)
  FIND_AND_ADD_CLANG_LIB(clangAnalysis)
  FIND_AND_ADD_CLANG_LIB(clangRewrite)
  FIND_AND_ADD_CLANG_LIB(clangAST)
  FIND_AND_ADD_CLANG_LIB(clangParse)
  FIND_AND_ADD_CLANG_LIB(clangLex)
  FIND_AND_ADD_CLANG_LIB(clangBasic)

  find_path(CLANG_SOURCE_INCLUDE_DIRS clang/Basic/Version.h HINTS "${CLANG_ROOT_DIR}/include")
  find_path(CLANG_BUILD_INCLUDE_DIRS clang/Basic/Version.inc HINTS "${LLVM_ROOT_DIR}/tools/clang/include")
  set(CLANG_INCLUDE_DIRS "${CLANG_SOURCE_INCLUDE_DIRS} ${CLANG_BUILD_INCLUDE_DIRS}")

  if(NOT CLANG_SOURCE_INCLUDE_DIRS OR NOT CLANG_BUILD_INCLUDE_DIRS)
    MESSAGE(FATAL_ERROR "Could not find clang include directories")
  endif()

  if(CLANG_LIBS AND CLANG_INCLUDE_DIRS)
    message(STATUS "Found Clang: ${CLANG_INCLUDE_DIRS}")
    set(CLANG_CXXFLAGS "-I${CLANG_SOURCE_INCLUDE_DIRS} -I${CLANG_BUILD_INCLUDE_DIRS}")
  else()
    if(CLANG_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Clang")
    endif()
  endif()
endif()