#!/bin/bash
# 本地测试脚本
if command -v build/SUBMIT >/dev/null 2>&1; then
  echo "找到SUBMIT可执行文件，开始测试"
else
  echo "未找到可执行文件，请先编译"
fi

FOLDER_NAME="output_for_open_set"
if [[ -d "$FOLDER_NAME" ]]; then
  echo "文件夹 '$FOLDER_NAME' 存在，正在删除..."
  rm -rf "$FOLDER_NAME"
  echo "文件夹 '$FOLDER_NAME' 已删除。"
else
  echo "文件夹 '$FOLDER_NAME' 不存在。"
fi
mkdir output_for_open_set

for testcase in ./open_set/*.pas
do
  echo "Testing $testcase"
  base_name=$(basename "$testcase" .pas)
  if ! ./build/SUBMIT -i "$testcase" 2> "output_for_open_set/${base_name}.log"; then
    echo -e "\e[31m编译失败，输出文件为 output_for_open_set/${base_name}.log\e[0m"
  else
    mv open_set/${base_name}.c output_for_open_set/${base_name}.c
    echo -e "\e[32m编译成功，输出文件为 output_for_open_set/${base_name}.c\e[0m"
    echo -e "尝试使用gcc编译输出文件...\n"
    if gcc -o "output_for_open_set/${base_name}.out" "output_for_open_set/${base_name}.c"; then
      echo -e "\e[32mgcc编译成功，输出文件为 output_for_open_set/${base_name}.out\e[0m"
      echo -e "尝试运行输出文件...\n"
      if [[ -f "open_set/${base_name}.in" ]]; then
        echo -e "输入文件为 open_set/${base_name}.in\n"
        if timeout 5s ./output_for_open_set/${base_name}.out < "open_set/${base_name}.in" > "output_for_open_set/${base_name}.result"; then
          echo -e "\e[32m运行成功，结果文件为 output_for_open_set/${base_name}.result\e[0m"
        else
          echo -e "\e[31m运行失败或超时，结果文件为 output_for_open_set/${base_name}.errresult\e[0m"
        fi
      else
        echo -e "输入文件不存在，尝试直接运行输出文件...\n"
        if timeout 5s ./output_for_open_set/${base_name}.out > "output_for_open_set/${base_name}.result"; then
          echo -e "\e[32m运行成功，结果文件为 output_for_open_set/${base_name}.result\e[0m"
        else
          echo -e "\e[31m运行失败或超时，结果文件为 output_for_open_set/${base_name}.result\e[0m"
        fi
      fi
    else
      echo -e "\e[31mgcc编译失败，输出文件为 output_for_open_set/${base_name}.errout\e[0m"
    fi
  fi
  echo "-----------------------------------"
done