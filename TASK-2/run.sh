
#!/usr/bin/env bash 
 #使用bash

# 项目路径
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)" 
 #找到我这个run脚本所在文件路径
BUILD_DIR="$PROJECT_DIR/build"
EXEC="$BUILD_DIR/app" 
 #可执行程序路径


# 编译函数
build_project() {
    if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
        echo "正在配置 CMake..."

        cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" || return 1
        
    fi
        cmake --build "$BUILD_DIR" || return 1
    echo "编译成功！"
}

# 主命令分发
case "${1:-}" in
    build)
        build_project || exit 1
        ;;

    run)
    #shift参数前移
        shift

        # 忽略分隔符 --，剩下的参数传给程序
        if [[ "${1:-}" == "--" ]]; then
            shift
        fi

        build_project || exit 1

        echo "运行程序..."
        #执行程序
        "$EXEC" "$@"
        exit $?
        ;;

    clean)
        echo "清理编译产物..."
        rm -rf -- "$BUILD_DIR"
        echo "清理完成！"
        ;;

    *)
        echo "遇到未知子命令 $1" >&2
        exit 1
        ;;
esac
