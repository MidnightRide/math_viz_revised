#include "filesystem.h"

#ifdef __unix__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void filesystem_get_dir(char *dir, u32 length) {
    getcwd(dir, length * sizeof(char));
}

void filesystem_change_dir(char *path) {
    chdir(path);
}

void filesystem_create_dir(char *path) {
    mkdir(path, 0700);
}

b8 filesystem_dir_exists(char *path) {
    struct stat st = {0};
    return stat(path, &st) == -1; 
}

b8 filesystem_file_exists(char *path) {
    return access(path, 0) == 0;
}

#endif
