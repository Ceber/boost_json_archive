cmake_minimum_required(VERSION 3.13)
enable_language(CXX)

############################################################################
# Build Settings
############################################################################
set(VERBOSE 1)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(UNIT_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/unit-tests)
set(INTEGRATION_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/integration-tests)
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${UNIT_TEST_OUTPUT_DIR})
file(MAKE_DIRECTORY ${INTEGRATION_TEST_OUTPUT_DIR})

############################################################################
# Install Settings
############################################################################
if (CMAKE_CROSSCOMPILING)
  set(BASE_INSTALL_DIR   /opt/${CMAKE_LIBRARY_ARCHITECTURE}/2ndParty/${PROJECT_NAME} CACHE PATH "Base Installation directory")
else()
  set(BASE_INSTALL_DIR   /opt/2ndParty/${PROJECT_NAME}                               CACHE PATH "Base Installation directory")
endif()
set(INSTALL_LIB_DIR      ${BASE_INSTALL_DIR}/lib                                     CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR      ${BASE_INSTALL_DIR}/bin                                     CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR  ${BASE_INSTALL_DIR}/include                                 CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR ${BASE_INSTALL_DIR}/CMake/)
else()
  set(DEF_INSTALL_CMAKE_DIR ${INSTALL_LIB_DIR}/CMake/)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")


############################################################################
# Options
############################################################################
option(BUILD_DEMOS      "Build Demos."                           ON)
option(BUILD_SHARED     "Build shared if ON, static if OFF."     ON)  
option(DEBUG_MODE       "Enable 'debug mode' support."           OFF)
option(BUILD_TESTS      "Build Tests."                           ON)
option(BUILD_TOOLS      "Build Tools"                            ON)
option(BUILD_DOC        "Build documentation."                   OFF)
option(CODE_COVERAGE    "Enable code coverage testing support."  OFF)
option(CONAN_BUILD      "Building from conan."                   OFF)
option(ASAN             "Enable Address Satinizer."              OFF)
option(TSAN             "Enable Thread Sanitizer."               OFF)
option(UBSAN            "Enable UndefinedBehavior Sanitizer."    OFF)
option(IWYU             "Enable 'Include-what-you-use'."         OFF)


############################################################################
# Code Format
############################################################################
include(clang-format)

############################################################################
# Code coverage
############################################################################
if (CODE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	include(CodeCoverage)
	append_coverage_compiler_flags()
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE ctest -R tests --verbose
        EXCLUDE "/usr/include/*" "/opt/*" "*-tests/*" "tools/**")
elseif(CODE_COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "####### Code Coverage option ignored : Only available in Debug build.")
endif()


############################################################################
# Doc
############################################################################
if (BUILD_DOC)
	# Doxygen Documentation
	find_package(Doxygen
	             REQUIRED dia
	             OPTIONAL_COMPONENTS dot)
	             
	add_custom_target(doc
					  COMMAND cd ${CMAKE_SOURCE_DIR} && doxygen Doxyfile && cd ${CMAKE_BINARY_DIR})
endif()

############################################################################
# Configure and Find Packages
############################################################################
### Required Packages ###
# Threading
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

############################################################################
# Compiler Settings and Flags
############################################################################
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Warnings Compile Options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") #-Werror")
# Build type settings
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fstack-protector-strong")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG_MODE ON)
endif()
if (DEBUG_MODE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fexceptions -fnon-call-exceptions -DDEBUG_MODE")
endif()
   
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fstack-protector-strong -DSERIALIZATION_BINARY")

if (ASAN AND TSAN) 
  message(FATAL_ERROR "You can not have ASAN and TSAN at the same time ! CMake will exit." )
endif()

if(ASAN)
  add_compile_options("-fsanitize=address")
  link_libraries("-fsanitize=address")
endif()

if(TSAN)
  add_compile_options("-fsanitize=thread")
  link_libraries("-fsanitize=thread")
endif()
