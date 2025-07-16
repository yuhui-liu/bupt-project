package main

import (
	"bufio"
	"log"
	"os"
	"strings"
)

// captureInputUntil 读取输入并在碰到指定的终止字符串时停止，把所有内容拼接成一个字符串返回。
func captureInputUntil(terminator string) string {
	var result []string
	scanner := bufio.NewScanner(os.Stdin)

	log.Printf("Enter content line by line. Type '%s' on a single line to finish:\n", terminator)

	for scanner.Scan() {
		line := scanner.Text()
		if line == terminator { // 检测输入是否为指定值
			break
		}
		result = append(result, line)
	}

	if err := scanner.Err(); err != nil {
		log.Printf("error reading input: %v\n", err)
	}

	return strings.Join(result, "\n")
}

// CliModeHandler 处理命令行模式的输入
func CliModeHandler(terminator string, show OutputMode) {
	input := captureInputUntil(terminator)
	// 处理输入的代码
	output, ok := RunTheCompiler(show, 1, input)
	if ok {
		log.Println("Compilation successful.")
		log.Printf("Output:\n%s", output)
	} else {
		log.Println("Compilation failed.")
		log.Printf("Error message:\n%s", output)
	}
}
