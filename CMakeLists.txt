cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(fcalls VERSION 1.0.0)

include(CPack)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

file(GLOB SOURCES "src/*.cc")
add_executable(fcalls ${SOURCES})

target_link_libraries(fcalls PRIVATE ${LLVM_AVAILABLE_LIBS})
target_link_libraries(fcalls PRIVATE clang clang-cpp)

install(TARGETS fcalls DESTINATION bin)
