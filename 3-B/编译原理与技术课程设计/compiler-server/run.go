package main

import (
	"bytes"
	"fmt"
	"log"
	"os"
	"os/exec"

	"github.com/joho/godotenv"
)

// OutputMode 定义了编译器的输出模式
type OutputMode int

// OutputMode 的常量定义，表示不同的输出模式
// 包含词法分析器输出、语法分析器输出和 C 代码输出
const (
	LEXER_RES OutputMode = iota
	PARSER_RES
	C_CODE
)

// RunTheCompiler 函数运行编译器并返回输出结果
func RunTheCompiler(m OutputMode, useColor int, src string) (string, bool) {
	// 加载环境变量
	err := godotenv.Load()
	if err != nil {
		log.Println("No .env file found.")
	}
	// 获取可执行文件名，默认为COMPILER
	binary := os.Getenv("BINARY_NAME")
	if binary == "" {
		binary = "COMPILER"
	}

	log.Println("Compiling using command", binary, "...")

	// 创建命令以及输入、输出、错误
	cmd := exec.Command("./" + binary)
	var input bytes.Buffer
	var output bytes.Buffer
	var stderr bytes.Buffer
	// 准备输入
	fmt.Fprintf(&input, "%d %d\n%s", m, useColor, src)
	// 连接标准输入、标准输出、标准错误
	cmd.Stdin = &input
	cmd.Stdout = &output
	cmd.Stderr = &stderr
	// 运行命令
	err = cmd.Run()

	if err != nil {
		return stderr.String(), false
	}

	return output.String(), true
}
