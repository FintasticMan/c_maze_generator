#include "maze.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct maze {
    size_t rows;
    size_t columns;
    size_t visited;
    bool **cells;
    bool **horiz_walls;
    bool **vert_walls;
};

bool gen_maze(struct maze *mz, size_t row, size_t column);
bool try_move_maze(
    struct maze *mz,
    size_t row,
    size_t column,
    uint8_t choice
);
bool find_path_maze(struct maze *mz, size_t row, size_t column);

struct maze *create_maze(size_t const rows, size_t const columns) {
    assert(rows > 0 && columns > 0);

    struct maze *const mz = malloc(sizeof (struct maze));
    assert(mz);

    mz->rows = rows;
    mz->columns = columns;
    mz->visited = 0;
    mz->cells = malloc(rows * sizeof (bool *));
    mz->horiz_walls = malloc((rows + 1) * sizeof (bool *));
    mz->vert_walls = malloc(rows * sizeof (bool *));
    assert(mz->cells && mz->horiz_walls && mz->vert_walls);
    for (size_t row = 0; row < rows; row++) {
        mz->cells[row] = calloc(columns, sizeof (bool));
        mz->horiz_walls[row] = malloc(columns * sizeof (bool));
        mz->vert_walls[row] = malloc((columns + 1) * sizeof (bool));
        assert(mz->cells[row] && mz->horiz_walls[row] && mz->vert_walls[row]);
        for (size_t column = 0; column < columns; column++) {
            mz->horiz_walls[row][column] = true;
            mz->vert_walls[row][column] = true;
        }
        mz->vert_walls[row][columns] = true;
    }
    mz->horiz_walls[rows] = malloc(columns * sizeof (bool));
    assert(mz->horiz_walls[rows]);
    for (size_t column = 0; column < columns; column++) {
        mz->horiz_walls[rows][column] = true;
    }

    return mz;
}

void destroy_maze(struct maze *const mz) {
    for (size_t row = 0; row < mz->rows; row++) {
        free(mz->cells[row]);
        free(mz->horiz_walls[row]);
        free(mz->vert_walls[row]);
    }
    free(mz->horiz_walls[mz->rows]);
    free(mz->cells);
    free(mz->horiz_walls);
    free(mz->vert_walls);

    free(mz);
}

void generate_maze(struct maze *const mz) {
    gen_maze(mz, 0, 0);
    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            mz->cells[row][column] = false;
        }
    }
}

bool gen_maze(struct maze *const mz, size_t const row, size_t const column) {
    mz->visited++;
    mz->cells[row][column] = true;

    if (mz->visited == mz->rows * mz->columns) {
        return true;
    }

    bool chosen[4] = {false};
    uint8_t nb_chosen = 0;
    while (nb_chosen < 4) {
        uint8_t choice;
        int r;
        do {
            r = rand();
        } while (r > RAND_MAX - (RAND_MAX % 4));
        choice = r % 4;

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

bool try_move_maze(
    struct maze *const mz,
    size_t const row,
    size_t const column,
    uint8_t const choice
) {
    switch (choice) {
        case 0:
            if (
                mz->horiz_walls[row][column] && row > 0
                && !mz->cells[row - 1][column]
            ) {
                mz->horiz_walls[row][column] = false;
                if (gen_maze(mz, row - 1, column)) {
                    return true;
                }
            }
            break;

        case 1:
            if (
                mz->vert_walls[row][column] && column > 0
                && !mz->cells[row][column - 1]
            ) {
                mz->vert_walls[row][column] = false;
                if (gen_maze(mz, row, column - 1)) {
                    return true;
                }
            }
            break;

        case 2:
            if (
                mz->horiz_walls[row + 1][column] && row + 1 < mz->rows
                && !mz->cells[row + 1][column]
            ) {
                mz->horiz_walls[row + 1][column] = false;
                if (gen_maze(mz, row + 1, column)) {
                    return true;
                }
            }
            break;

        case 3:
            if (
                mz->vert_walls[row][column + 1] && column + 1 < mz->columns
                && !mz->cells[row][column + 1]
            ) {
                mz->vert_walls[row][column + 1] = false;
                if (gen_maze(mz, row, column + 1)) {
                    return true;
                }
            }
            break;
    }

    return false;
}

void path_maze(struct maze *const mz) {
    find_path_maze(mz, 0, 0);
}

bool find_path_maze(struct maze *const mz, size_t const row, size_t const column) {
    mz->cells[row][column] = true;

    if (row == mz->rows - 1 && column == mz->columns - 1) {
        return true;
    }

    if (
        !mz->horiz_walls[row][column] && row > 0
        && !mz->cells[row - 1][column]
        && find_path_maze(mz, row - 1, column)
    ) {
        return true;
    }

    if (
        !mz->vert_walls[row][column] && column > 0
        && !mz->cells[row][column - 1]
        && find_path_maze(mz, row, column - 1)
    ) {
        return true;
    }

    if (
        !mz->horiz_walls[row + 1][column] && row + 1 < mz->rows
        && !mz->cells[row + 1][column]
        && find_path_maze(mz, row + 1, column)
    ) {
        return true;
    }

    if (
        !mz->vert_walls[row][column + 1] && column + 1 < mz->columns
        && !mz->cells[row][column + 1]
        && find_path_maze(mz, row, column + 1)
    ) {
        return true;
    }

    mz->cells[row][column] = false;

    return false;
}

void print_maze(struct maze *const mz) {
    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            printf(mz->horiz_walls[row][column] ? "+---" : "+   ");
        }
        printf("+\n");

        for (size_t column = 0; column < mz->columns; column++) {
            printf(mz->vert_walls[row][column] ? "|" : " ");
            printf(mz->cells[row][column] ? " . " : "   ");
        }
        printf(mz->vert_walls[row][mz->columns] ? "|\n" : " \n");
    }
    for (size_t column = 0; column < mz->columns; column++) {
        printf(mz->horiz_walls[mz->rows][column] ? "+---" : "+   ");
    }
    printf("+\n");
}
