﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("VZOpenGL")

set(CMAKE_CXX_STANDARD 20)


# 包含子项目。
add_subdirectory (${PROJECT_SOURCE_DIR}/src)
