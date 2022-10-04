# Seserot

[![Contributors](https://img.shields.io/github/contributors/zly2006/Seserot)](https://github.com/zly2006/Seserot/graphs/contributors)
[![Github stars](https://img.shields.io/github/stars/zly2006/Seserot)](https://github.com/zly2006/Seserot/stargazers)
[![GitHub issues by-label](https://img.shields.io/github/issues/zly2006/Seserot)](https://github.com/zly2006/Seserot/issues?q=is%3Aissue+is%3Aopen)
[![CMake](https://github.com/zly2006/Seserot/actions/workflows/cmake.yml/badge.svg)](https://github.com/zly2006/Seserot/actions/workflows/cmake.yml)
[![CodeQL](https://github.com/zly2006/Seserot/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/zly2006/Seserot/actions/workflows/codeql-analysis.yml)
[![GitHub license](https://img.shields.io/github/license/zly2006/Seserot)](https://github.com/zly2006/Seserot/tree/master/LICENSE)
[![Github Release](https://img.shields.io/github/v/release/zly2006/Seserot)](https://github.com/zly2006/Seserot/releases)
[![GitHub top language](https://img.shields.io/github/languages/top/zly2006/Seserot)]()

My toy compiler

[国内加速repo](https://gitee.com/zhaoliyan/Seserot)
[Github](https://github.com/zly2006/Seserot)

## Introduction

This is a toy compiler for a simple language, which is a project for me to learn compilers.

This compiler compiles [the Seserot programming language](docs/language/index.md) to LLVM IR, then uses LLVM to 
compile to machine 
code.

## How to Build

This compiler is based on [LLVM](https://llvm.org), so you need to [install llvm](https://llvm.org/docs/CMake.html)
before building this compiler.

Then, clone this repo and build it:

```bash
git clone https://github.com/zly2006/Seserot.git
cd Seserot
mkdir build
cd build
cmake ..
make
```

## To-Do List

| Feature  | Status   |
|----------|----------|
| Lexer    | Done     |
| Parser   | WIP      |
| AST      | WIP      |
| LLVM IR  | WIP      |
| Debugger | Planning |