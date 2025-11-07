# 最小要求的 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(GoodOldDaysDiary C)


# 添加可执行文件 (项目名, 源代码文件)
add_executable(GoodOldDaysDiary ./C_Core/nostalgia_manager.c)

set_target_properties(GoodOldDaysDiary PROPERTIES
        OUTPUT_NAME "manager"
)


# ===============================================
# 🛠 解决中文乱码的关键编译选项
# ===============================================

## 告诉 GCC 编译器，源代码文件是 UTF-8 编码
#target_compile_options(GoodOldDaysDiary PRIVATE
#        -finput-charset=GBK
#        -fexec-charset=GBK
#)

# ===============================================
