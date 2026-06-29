cmake_minimum_required(VERSION 3.23)

project(
  testrunner
  VERSION 0.1.0
  DESCRIPTION "A simple C++ unit test framework"
  LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(testrunner src/testrunner.cc)
add_library(testrunner::testrunner ALIAS testrunner)

set_target_properties(testrunner PROPERTIES EXPORT_NAME testrunner)

target_compile_features(testrunner PUBLIC cxx_std_23)
target_include_directories(
  testrunner
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(testrunner_main src/testrunner_main.cc)
add_library(testrunner::main ALIAS testrunner_main)

set_target_properties(testrunner_main PROPERTIES EXPORT_NAME main)

target_compile_features(testrunner_main PUBLIC cxx_std_23)
target_link_libraries(testrunner_main PUBLIC testrunner::testrunner)

install(
  TARGETS testrunner testrunner_main
  EXPORT testrunnerTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/testrunner)

install(
  EXPORT testrunnerTargets
  FILE testrunnerTargets.cmake
  NAMESPACE testrunner::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/testrunner)

configure_package_config_file(
  cmake/testrunnerConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/testrunnerConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/testrunner)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/testrunnerConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/testrunnerConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/testrunnerConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/testrunner)
