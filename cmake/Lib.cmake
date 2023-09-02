############################################################################
# Export Install
############################################################################
list(PREPEND ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
# Add all targets to the build-tree export set
export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
  	   
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${PROJECT_NAME})

# Create the CommonsConfig.cmake and CommonsConfigVersion.cmake files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIR "${INSTALL_INCLUDE_DIR}")
set(CONF_LIB_DIR     "${INSTALL_LIB_DIR}")
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_LIB_DIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
  VERSION "${${CMAKE_PROJECT_NAME}_VERSION}"
  COMPATIBILITY SameMajorVersion )
  
# Install the ${PROJECT_NAME}Config.cmake and ${PROJECT_NAME}ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ${PROJECT_NAME}Targets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install
install(
	TARGETS ${PROJECT_NAME}
	# IMPORTANT: Add the library to the "export-set"
  	EXPORT ${PROJECT_NAME}Targets
  	RUNTIME DESTINATION  ${INSTALL_BIN_DIR}             COMPONENT dev
  	LIBRARY DESTINATION  ${INSTALL_LIB_DIR}   		      COMPONENT dev
  	ARCHIVE  DESTINATION  ${INSTALL_LIB_DIR}   		      COMPONENT dev
    PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}    COMPONENT dev
)
# Install include for developpers
install(DIRECTORY "include/" # source directory
        DESTINATION "${INSTALL_INCLUDE_DIR}" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h*" # select header files
)


############################################################################
# CPack
############################################################################
set(CPACK_GENERATOR "STGZ")
set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_LIBRARY_ARCHITECTURE}-${${PROJECT_NAME}_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "ceber")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_NAME} - ${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")
if (CMAKE_CROSSCOMPILING)
  set(CPACK_PACKAGING_INSTALL_PREFIX  "/opt/${CMAKE_LIBRARY_ARCHITECTURE}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${BASE_INSTALL_DIR}")
else()
  set(CPACK_PACKAGING_INSTALL_PREFIX  "/opt/")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${BASE_INSTALL_DIR}")
endif()
# This must always be last...okay...!
include(CPack)




############################################################################
# Configuration Summary
############################################################################
message(STATUS "############## ${CMAKE_PROJECT_NAME} CONFIG SUMMARY ##############")
message(STATUS "####### CMAKE_SYSTEM_PROCESSOR:             "   ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "####### CMAKE_LIBRARY_ARCHITECTURE:         "   ${CMAKE_LIBRARY_ARCHITECTURE})
message(STATUS "####### CMAKE_BINARY_DIR:                   "   ${CMAKE_BINARY_DIR})
message(STATUS "####### CMAKE_SOURCE_DIR:                   "   ${CMAKE_SOURCE_DIR})
message(STATUS "####### CMAKE_LIBRARY_OUTPUT_DIRECTORY:     " 	${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "####### CMAKE_RUNTIME_OUTPUT_DIRECTORY:     " 	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "####### INSTALL_INCLUDE_DIR:                " 	${INSTALL_INCLUDE_DIR})
message(STATUS "####### INSTALL_LIB_DIR:                    " 	${INSTALL_LIB_DIR})
message(STATUS "####### ${PROJECT_NAME}_LIBRARIES:           " 	${${PROJECT_NAME}_LIBRARIES})
message(STATUS "####### INSTALL_CMAKE_DIR:                  " 	${INSTALL_CMAKE_DIR})
message(STATUS "####### UNIT_TEST_OUTPUT_DIR:               "	  ${UNIT_TEST_OUTPUT_DIR})
message(STATUS "####### INTEGRATION_TEST_OUTPUT_DIR:        " 	${INTEGRATION_TEST_OUTPUT_DIR}) 
