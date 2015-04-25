FIND_PROGRAM(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
if (${IBTOOL} STREQUAL "IBTOOL-NOTFOUND")
    MESSAGE(SEND_ERROR "ibtool can not be found")
ENDIF()
add_executable(RhineCo MACOSX_BUNDLE
    AppDelegate.h
    AppDelegate.m
    Main.xib
    main.m
    Info.plist)
set_target_properties(RhineCo PROPERTIES
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
add_custom_command(TARGET RhineCo POST_BUILD
  COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/RhineCo.app/Contents/Resources" &&
  ${IBTOOL} --errors --warnings --notices --output-format human-readable-text 
  --compile "${CMAKE_CURRENT_BINARY_DIR}/RhineCo.app/Contents/Resources/Main.nib"
  "${CMAKE_CURRENT_SOURCE_DIR}/Main.xib")
target_link_libraries(RhineCo
    "-framework Cocoa"
    "-framework AppKit"
    "-framework CoreData"
    "-framework Foundation")