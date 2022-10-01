cmake_minimum_required(VERSION 3.21)
project(Seserot)

set(CMAKE_CXX_STANDARD 23)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(Seserot
        src/main.cpp
        src/Lexer.cpp
        src/ErrorTable.cpp
        src/CompilerError.cpp
        src/Parser.cpp
        src/BuildIn.cpp
        src/AbstractSyntaxTreeNode.cpp
        src/ByteCodeWriter.cpp
        src/Symbol.cpp
        src/ast/FloatingConstantNode.cpp
        src/ast/IntegerConstantNode.cpp src/ast/BinaryOperatorNode.cpp src/ast/BinaryOperatorNode.h src/ast/UnaryOperatorNode.cpp src/ast/UnaryOperatorNode.h)

# Link against LLVM libraries
target_link_libraries(Seserot ${llvm_libs})

