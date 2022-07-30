#include "filesystem.h"

#if defined(_WIN32) || defined(WIN32)
#include <io.h>
#include <direct.h>
#include <windows.h>

void filesystem_get_dir(char *dir, u32 length) {
    _getcwd(dir, length * sizeof(char));
}

void filesystem_change_dir(char *path) {
    _chdir(path);
}

void filesystem_create_dir(char *path) {
    CreateDirectory(path, NULL);
}

b8 filesystem_dir_exists(char *path) {
    return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}

b8 filesystem_file_exists(char *path) {
    return _access(path, 0) == 0;
}
#endif
