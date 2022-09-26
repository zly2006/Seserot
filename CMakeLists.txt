cmake_minimum_required(VERSION 3.21)
project(Seserot)

set(CMAKE_CXX_STANDARD 20)

add_executable(Seserot
        main.cpp
        Lexer.cpp
        ErrorTable.cpp
        CompilerError.cpp
        Parser.cpp
        BuildIn.cpp
        AbstractSyntaxTreeNode.cpp
        ByteCodeWriter.cpp
        Symbol.cpp src/utils/ByteOrder.h)

