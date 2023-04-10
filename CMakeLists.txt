cmake_minimum_required(VERSION 3.15)

# Set the project name to your project name, my project isn't very descriptive
project(Tutorial VERSION 1.0 LANGUAGES CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)


# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# iwyu
#include(cmake/Iwyu.cmake)
# enable cache system
#include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" OFF)



add_subdirectory(include)

option(ENABLE_UNITY "Enable Unity builds of projects" OFF)
if(ENABLE_UNITY)
  # Add for any project you want to apply unity builds for
  set_target_properties(game24 PROPERTIES UNITY_BUILD ON)
endif()


# control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")


if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()


# add the MathFunctions library
#add_subdirectory(MathFunctions)
#add_subdirectory(TutorialDir)
#
#
## add the install targets
#install(TARGETS Tutorial DESTINATION bin)
#install(FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"
#  DESTINATION include
#  )
#
## enable testing
if(ENABLE_TESTING)
  add_subdirectory(test)
endif()
#enable_testing()
## set(VCPKG_ROOT "/home/rob/openSrc/vcpkg")
#
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
#include(CPack)
#
## install the configuration targets
#install(EXPORT MathFunctionsTargets
#  FILE MathFunctionsTargets.cmake
#  DESTINATION lib/cmake/MathFunctions
#)
#
#include(CMakePackageConfigHelpers)
## generate the config file that is includes the exports
#configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#  "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfig.cmake"
#  INSTALL_DESTINATION "lib/cmake/example"
#  NO_SET_AND_CHECK_MACRO
#  NO_CHECK_REQUIRED_COMPONENTS_MACRO
#  )
## generate the version file for the config file
#write_basic_package_version_file(
#  "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfigVersion.cmake"
#  VERSION "${Tutorial_VERSION_MAJOR}.${Tutorial_VERSION_MINOR}"
#  COMPATIBILITY AnyNewerVersion
#)
#
## install the configuration file
#install(FILES
#  ${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfig.cmake
#  DESTINATION lib/cmake/MathFunctions
#  )
#
## generate the export targets for the build tree
## needs to be after the install(TARGETS ) command
#export(EXPORT MathFunctionsTargets
#  FILE "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsTargets.cmake"
#)
