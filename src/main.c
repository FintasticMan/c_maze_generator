#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"

#define BASE 10

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    size_t rows = strtoull(argv[1], NULL, BASE);
    size_t columns = strtoull(argv[2], NULL, BASE);
    srand((argc == 4) ? strtoull(argv[3], NULL, BASE) : time(NULL));

    struct maze *mz = maze_create(rows, columns);

    maze_generate(mz);
    maze_find_path(mz);

    maze_print(stdout, mz);

    maze_destroy(mz);

    return 0;
}
