cmake_minimum_required(VERSION 3.21)
project(Seserot_gen0)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        main.cpp
        Lexer.cpp Lexer.h
        ErrorTable.cpp ErrorTable.h
        CompilerError.cpp CompilerError.h
        Parser.cpp Parser.h
        BuildIn.cpp BuildIn.h
        AbstractSyntaxTreeNode.cpp AbstractSyntaxTreeNode.h
        ByteCodeWriter.cpp ByteCodeWriter.h
        Symbol.cpp)

