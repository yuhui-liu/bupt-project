# 客服机器人
本项目是一个基于DSL（Domain Specific Language）的客服机器人，可以根据用户输入的问题，自动回答问题。
用户可以通过DSL文件配置机器人的回答规则。
## 项目结构
```
┌── CMakeLists.txt
├── src
│   ├── main.cpp
│   ├── <...src files...>
├── include
│   ├── <...header files...>
├── test
│   ├── CMakelists.txt
│   ├── <...test files...>
└── doc
    └── <...doc files...>
```
## Requirements
- [spdlog](https://github.com/gabime/spdlog)
- [boost](https://www.boost.org/)
- [GoogleTest](https://github.com/google/googletest)
- [sqlite](https://www.sqlite.org/index.html)

## Build
```shell
cmake -S . -B build
cmake --build build
```

## 语法
EBNF：
```
program         ::= { constant-define } { procedure }

constant-define ::= id "=" string
procedure       ::= "procedure" id "{" { statement } "}"

statement       ::= say | listen | lookup | exit | id

say             ::= "say" id | string { "," id|string }
listen          ::= "listen" "timelimit" number "{" { has | timeout | default | anything } "}"
lookup          ::= "lookup" id "in" id ":" id

anything        ::= "anything" "?" id | writeto
has             ::= "has" id | string "?" id | writeto
timeout         ::= "timeout" "?" id | writeto
default         ::= "default" "?" id | writeto

writeto         ::= "writeto" id ":" id ":" id

string          ::= '"' { allcharacters - '"' } '"'
id              ::= [a-zA-Z] id | [a-zA-Z]

allcharacters   ::= ? all visible ascii characters ?
```
