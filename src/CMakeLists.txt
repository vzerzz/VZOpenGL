# CMakeList.txt: Shadows 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 头文件查找目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/imgui
)

# 把src目录下所有源文件写入变量DIR_SRCS
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/imgui GUI_SRCS)

# 设置可执行文件输出路径
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin) 

# 目标文件链接的库
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME} ${DIR_SRCS} ${GUI_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)


# TODO: 如有需要，请添加测试并安装目标。
