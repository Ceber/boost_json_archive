cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Version
set(boost_json_archive_VERSION_MAJOR 0)
set(boost_json_archive_VERSION_MINOR 0)
set(boost_json_archive_VERSION_PATCH 2)
set(boost_json_archive_VERSION ${boost_json_archive_VERSION_MAJOR}.${boost_json_archive_VERSION_MINOR}.${boost_json_archive_VERSION_PATCH})
message(STATUS "####### boost_json_archive_VERSION:            "   ${boost_json_archive_VERSION})

project(boost_json_archive VERSION ${boost_json_archive_VERSION} DESCRIPTION "C++ boost::json_archive 2nd party lib - is a libary built on top of Boost::Serialization that enables Json format (using Boost::Json).")
enable_language(CXX)

# Module and Packages
include(CMakePackageConfigHelpers)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Config)

# Boost
find_package(Boost 1.75.0 REQUIRED COMPONENTS system serialization json)
add_compile_definitions("BOOST_NO_STD_WSTRING") # I needed this for Android...if I remember well ;)
if (Boost_FOUND)
    message(STATUS "####### BOOST Package Found !")
    message(STATUS "####### Boost_VERSION:                      "   ${Boost_VERSION})
    message(STATUS "####### Boost_INCLUDE_DIR:                  "   ${Boost_INCLUDE_DIR})
    message(STATUS "####### Boost_DIR:                          "   ${Boost_DIR})
else()
    message(ERROR "####### BOOST Package not Found !")
endif()


# Source files
file(GLOB_RECURSE boost_json_archive_SRC
     "src/*.cpp"
)
# Source files
file(GLOB_RECURSE boost_json_archive_HEADERS
     "include/*.h*"
)

include_directories(${PROJECT_SOURCE_DIR}/include/)

add_library(${PROJECT_NAME} SHARED ${boost_json_archive_SRC})

target_link_libraries(${PROJECT_NAME}
	Boost::serialization
	Boost::json
)


if(BUILD_TESTS)
     enable_testing()
     # GTest
     find_package(GTest REQUIRED)
     # ... CMake code to create tests ...
     # Source files
     set(TESTS_EXE ${PROJECT_NAME}_unit-tests)
     file(GLOB_RECURSE ${PROJECT_NAME}_TESTS
          "unit-tests/**.h*"
          "unit-tests/**.cpp"
     )

     add_executable(${TESTS_EXE} ${${PROJECT_NAME}_TESTS})
     target_link_libraries(${TESTS_EXE} ${PROJECT_NAME} Boost::serialization GTest::gtest)
     set_target_properties (${TESTS_EXE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_OUTPUT_DIR})
     add_test(NAME ${TESTS_EXE} COMMAND $<TARGET_FILE:${TESTS_EXE}>)
endif(BUILD_TESTS)


############################################################################
# Settings + Directories + Export Install + CPack
############################################################################
# Install public include for developpers
install(DIRECTORY "include/" # source directory
        DESTINATION "${INSTALL_INCLUDE_DIR}" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h*" # select header files
)

include(Lib)

############################################################################
# Options Summary
############################################################################
message(STATUS "############## ${PROJECT_NAME} OPTIONS SUMMARY ##############")
message(STATUS "####### BUILD_TESTS:                        " 	${BUILD_TESTS})
message(STATUS "####### BUILD_DEMOS:                        " 	${BUILD_DEMOS})
message(STATUS "####### BUILD_DOC:                          "    ${BUILD_DOC})
message(STATUS "####### CODE_COVERAGE:                      " 	${CODE_COVERAGE})
message(STATUS "####### CONAN_BUILD:                        " 	${CONAN_BUILD})
message(STATUS "####### ASAN:                               " 	${ASAN})
message(STATUS "####### TSAN:                               " 	${TSAN})
