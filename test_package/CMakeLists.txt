cmake_minimum_required(VERSION 3.23)

project(testrunner_package_test LANGUAGES CXX)

find_package(testrunner REQUIRED)

add_executable(package_test package_test.cc)
target_compile_features(package_test PRIVATE cxx_std_23)
target_link_libraries(package_test PRIVATE testrunner::main)
