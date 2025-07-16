package main

import (
	"fmt"
	"log"
	"os"
)

// readFileContent 读取指定路径的文件，并返回文件内容作为字符串
func readFileContent(filePath string) (string, error) {
	data, err := os.ReadFile(filePath) // 读取文件
	if err != nil {
		return "", fmt.Errorf("error reading file: %w", err)
	}
	return string(data), nil // 返回文件内容作为字符串
}

// writeFileContent 将字符串内容写入指定路径的文件
func writeFileContent(filePath string, content string) error {
	// 使用 os.WriteFile 函数直接写入文件
	// 0644 是文件权限，表示所有者有读写权限，群组和其他人有读权限
	err := os.WriteFile(filePath, []byte(content), 0644)
	if err != nil {
		return fmt.Errorf("error writing to file: %w", err)
	}
	return nil
}

// FileModeHandler 处理文件模式的输入
func FileModeHandler(inputPath string, outputPath string, show OutputMode) {
	input, err := readFileContent(inputPath)
	if err != nil {
		log.Fatal(err)
	}

	output, ok := RunTheCompiler(show, 0, input)

	if ok {
		err = writeFileContent(outputPath, output)
		if err != nil {
			log.Fatal(err)
			return
		}
		log.Println("File written successfully in:", outputPath)
	} else {
		err = writeFileContent(outputPath, output)
		if err != nil {
			log.Fatal(err)
			return
		}
		log.Println("Compile error. Message written in:", outputPath)
	}
}
