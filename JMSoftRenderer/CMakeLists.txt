# CMakeList.txt: JMSoftRenderer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.14)

project ("JMSoftRenderer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenMP)
#find_package(UGM REQUIRED)


# 将源代码添加到此项目的可执行文件。
add_executable (JMSoftRenderer "Main.cpp" "FrameBuffer.cpp" "Pipeline.cpp" "Window.cpp")

# TODO: 如有需要，请添加测试并安装目标。
if(OpenMP_CXX_FOUND)
    target_link_libraries(JMSoftRenderer PUBLIC OpenMP::OpenMP_CXX)
endif()
#target_link_libraries(JMSoftRenderer PUBLIC Ubpa::UGM_core)