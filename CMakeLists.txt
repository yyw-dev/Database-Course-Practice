cmake_minimum_required(VERSION 3.10)
project(ClassManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 项目选项
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

# 查找依赖包
find_package(PkgConfig REQUIRED)
find_package(MySQL REQUIRED)
find_package(OpenSSL REQUIRED)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/crow/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann/single_include)

# 添加子目录
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
install(DIRECTORY include/ DESTINATION include)
install(TARGETS class_management_system DESTINATION bin)

# CPack配置
set(CPACK_PACKAGE_NAME "ClassManagementSystem")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_PACKAGE_DESCRIPTION "高校班级事务管理系统")
include(CPack)