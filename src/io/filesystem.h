/* date = July 11th 2022 7:23 pm */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../util/util.h"

void filesystem_get_dir(char *dir, u32 length);
void filesystem_change_dir(char *path);
void filesystem_create_dir(char *path);

b8 filesystem_dir_exists(char *path);
b8 filesystem_file_exists(char *path);

#endif // FILE_H
