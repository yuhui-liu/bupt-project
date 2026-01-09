#include "../include/args_handler.h"
#include "../include/network.h"
#include "../include/file_reader.h"

int main(int argc, char *argv[]) {
    args_init(argc, argv);
    load_file();
    network_init();
    main_process();
    return 0;
}
