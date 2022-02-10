#include <stdlib.h>

#include "maze.h"

struct maze create_maze(const int rows, const int columns) {
    struct maze ret = {
        .rows = rows,
        .columns = columns,
        .visited = 0,
        .cells = malloc(rows * sizeof(bool *)),
        .horiz_walls = malloc((rows + 1) * sizeof(bool *)),
        .vert_walls = malloc(rows * sizeof(bool *))
    };
    for (int row = 0; row < rows; row++) {
        ret.cells[row] = calloc(columns, sizeof(bool));
        ret.horiz_walls[row] = malloc(columns * sizeof(bool));
        ret.vert_walls[row] = malloc((columns + 1) * sizeof(bool));
        for (int column = 0; column < columns; column++) {
            ret.horiz_walls[row][column] = true;
            ret.vert_walls[row][column] = true;
        }
        ret.vert_walls[row][columns] = true;
    }
    ret.horiz_walls[rows] = malloc(columns * sizeof(bool));
    for (int column = 0; column < columns; column++) {
        ret.horiz_walls[rows][column] = true;
    }

    return ret;
}

void destroy_maze(struct maze * mz) {
    for (int row = 0; row < mz->rows; row++) {
        free(mz->cells[row]);
        free(mz->horiz_walls[row]);
        free(mz->vert_walls[row]);
    }
    free(mz->horiz_walls[mz->rows]);
    free(mz->cells);
    free(mz->horiz_walls);
    free(mz->vert_walls);
}

bool generate_maze(struct maze * mz, const int row, const int column) {
    mz->visited++;
    mz->cells[row][column] = true;

    if (mz->visited == mz->rows * mz->columns) {
        return true;
    }

    bool chosen[4] = {false};
    int nb_chosen = 0;
    while (nb_chosen < 4) {
        int choice = rand() / ((RAND_MAX + 1u) / 4);
        if (!chosen[choice]) {
            chosen[choice] = true;
            nb_chosen++;

            if (try_move_maze(mz, row, column, choice)) {
                return true;
            }
        }
    }

    return false;
}

bool try_move_maze(struct maze * mz, const int row, const int column, const int choice) {
    switch (choice) {
        case 0:
            if (mz->horiz_walls[row][column] && row > 0
                && !mz->cells[row - 1][column]) {
                mz->horiz_walls[row][column] = false;
                if (generate_maze(mz, row - 1, column)) {
                    return true;
                }
            }
            break;

        case 1:
            if (mz->vert_walls[row][column] && column > 0
                && !mz->cells[row][column - 1]) {
                mz->vert_walls[row][column] = false;
                if (generate_maze(mz, row, column - 1)) {
                    return true;
                }
            }
            break;

        case 2:
            if (mz->horiz_walls[row + 1][column] && row + 1 < mz->rows
                && !mz->cells[row + 1][column]) {
                mz->horiz_walls[row + 1][column] = false;
                if (generate_maze(mz, row + 1, column)) {
                    return true;
                }
            }
            break;

        case 3:
            if (mz->vert_walls[row][column + 1] && column + 1 < mz->columns
                && !mz->cells[row][column + 1]) {
                mz->vert_walls[row][column + 1] = false;
                if (generate_maze(mz, row, column + 1)) {
                    return true;
                }
            }
    }

    return false;
}
