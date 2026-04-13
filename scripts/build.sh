#!/bin/bash

# 高校班级事务管理系统构建脚本

set -e

echo "=== 高校班级事务管理系统构建脚本 ==="

# 配置变量
BUILD_DIR="build"
INSTALL_DIR="install"
BUILD_TYPE="Release"

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  -h, --help           显示此帮助信息"
    echo "  -t, --type TYPE      构建类型 (Debug, Release, RelWithDebInfo)"
    echo "  -i, --install        构建后安装"
    echo "  -c, --clean          清理构建目录"
    echo "  --tests              构建测试"
    echo "  --docs               构建文档"
    echo
    echo "示例:"
    echo "  $0 --type Debug"
    echo "  $0 --type Release --install"
}

# 解析命令行参数
CLEAN_BUILD=false
INSTALL_BUILD=false
BUILD_TESTS=false
BUILD_DOCS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -i|--install)
            INSTALL_BUILD=true
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        --tests)
            BUILD_TESTS=true
            shift
            ;;
        --docs)
            BUILD_DOCS=true
            shift
            ;;
        *)
            echo "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: CMake未安装，请先安装CMake"
    echo "Ubuntu/Debian: sudo apt-get install cmake"
    echo "CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

# 检查编译工具链
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "错误: 未找到C++编译器"
    echo "Ubuntu/Debian: sudo apt-get install build-essential"
    echo "CentOS/RHEL: sudo yum groupinstall 'Development Tools'"
    exit 1
fi

# 检查依赖库
echo "检查依赖库..."

# 检查MySQL开发库
if ! pkg-config --exists mysqlclient; then
    echo "警告: MySQL开发库未找到"
    echo "Ubuntu/Debian: sudo apt-get install libmysqlclient-dev"
    echo "CentOS/RHEL: sudo yum install mysql-devel"
    echo "继续构建可能会失败..."
fi

# 检查OpenSSL
try_openssl=false
if pkg-config --exists openssl; then
    try_openssl=true
    echo "✓ OpenSSL开发库已安装"
else
    echo "警告: OpenSSL开发库未找到"
    echo "Ubuntu/Debian: sudo apt-get install libssl-dev"
    echo "CentOS/RHEL: sudo yum install openssl-devel"
fi

# 清理构建目录
if [ "$CLEAN_BUILD" = true ]; then
    echo "清理构建目录..."
    rm -rf $BUILD_DIR
    rm -rf $INSTALL_DIR
    echo "构建目录已清理"
    exit 0
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 配置CMake
echo "配置CMake (构建类型: $BUILD_TYPE)..."
CMAKE_OPTIONS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DCMAKE_INSTALL_PREFIX=../$INSTALL_DIR"
)

if [ "$BUILD_TESTS" = true ]; then
    CMAKE_OPTIONS+=("-DBUILD_TESTS=ON")
else
    CMAKE_OPTIONS+=("-DBUILD_TESTS=OFF")
fi

if [ "$BUILD_DOCS" = true ]; then
    CMAKE_OPTIONS+=("-DBUILD_DOCS=ON")
else
    CMAKE_OPTIONS+=("-DBUILD_DOCS=OFF")
fi

if [ "$try_openssl" = true ]; then
    CMAKE_OPTIONS+=("-DOPENSSL_ROOT_DIR=$(pkg-config --variable=prefix openssl)")
fi

cmake "${CMAKE_OPTIONS[@]}" ..

# 构建项目
echo "构建项目..."
cmake --build . --config $BUILD_TYPE --parallel $(nproc)

echo "构建完成!"

# 运行测试
if [ "$BUILD_TESTS" = true ]; then
    echo "运行测试..."
    ctest --output-on-failure
fi

# 安装
if [ "$INSTALL_BUILD" = true ]; then
    echo "安装项目..."
    cmake --install .
    echo "安装完成!"
    echo "可执行文件已安装到: $INSTALL_DIR/bin"
fi

cd ..

echo "=== 构建完成 ==="
echo "可执行文件位置: $BUILD_DIR/class_management_system"
echo "日志文件位置: logs/class_management_system.log"
echo "配置文件位置: config/config.json"
echo
echo "启动命令:"
echo "./$BUILD_DIR/class_management_system --host 0.0.0.0 --port 8080"
echo
echo "API文档: http://localhost:8080/api/docs"