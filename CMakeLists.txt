cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)

project(fcalls VERSION 1.0.0)

include(CPack)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(LLVM_LINK_COMPONENTS Support)

file(GLOB SOURCES "src/*.cc")
add_executable(fcalls ${SOURCES})

target_link_libraries(fcalls PRIVATE ${LLVM_LIBS})
target_link_libraries(fcalls PRIVATE clangTooling)

install(TARGETS fcalls DESTINATION bin)
