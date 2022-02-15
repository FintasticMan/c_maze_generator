#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"

#define BASE 10

int main(int argc, char ** argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    size_t rows    = strtoull(argv[1], NULL, BASE);
    size_t columns = strtoull(argv[2], NULL, BASE);
    srand((argc == 4) ? strtoull(argv[3], NULL, BASE) : time(NULL));

    if (rows == 0U || columns == 0U) {
        fprintf(stderr, "Invalid size argument(s).\n");
        return 2;
    }

    struct maze * mz = create_maze(rows, columns);

    generate_maze(mz);
    path_maze(mz);

    print_maze(mz);

    destroy_maze(mz);

    return 0;
}
