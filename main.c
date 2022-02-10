#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"

int main(int argc, char * * argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    int rows = atoi(argv[1]);
    int columns = atoi(argv[2]);
    srand((argc == 4) ? atoi(argv[3]) : time(NULL));

    if (rows <= 0 || columns <= 0) {
        fprintf(stderr, "Invalid size argument(s).\n");
        return 2;
    }

    struct maze mz = create_maze(rows, columns);

    generate_maze(&mz, 0, 0);

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            printf(mz.horiz_walls[row][column] ? "+---" : "+   ");
        }
        printf("+\n");

        for (int column = 0; column < columns; column++) {
            printf(mz.vert_walls[row][column] ? "|" : " ");
            printf(mz.cells[row][column] ? " . " : "   ");
        }
        printf(mz.vert_walls[row][columns] ? "|\n" : " \n");
    }
    for (int column = 0; column < columns; column++) {
        printf(mz.horiz_walls[rows][column] ? "+---" : "+   ");
    }
    printf("+\n");

    destroy_maze(&mz);

    return 0;
}
