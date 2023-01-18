#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"

int main(int const argc, char const *const *const argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    size_t const rows = strtoull(argv[1], NULL, 0);
    size_t const columns = strtoull(argv[2], NULL, 0);
    srand(argc == 4 ? strtoul(argv[3], NULL, 0) : time(NULL));

    struct maze *const mz = maze_create(rows, columns);

    maze_gen(mz);
    maze_find_path(mz);

    maze_print(stdout, mz);

    maze_destroy(mz);

    return 0;
}
