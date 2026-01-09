cmake_minimum_required(VERSION 3.28)
project(dnsrelay C)

set(CMAKE_C_STANDARD 11)


add_executable(dnsrelay
        src/dnsrelay.c
        src/args_handler.c
        src/file_reader.c
        src/network.c
        src/dns_parser.c
        src/mapping.c
        include/args_handler.h
        include/file_reader.h
        include/logs.h
        include/network.h
        include/structs.h
        include/consts.h
        include/dns_parser.h
        include/mapping.h)

# target_link_libraries(dnsrelay -largp)