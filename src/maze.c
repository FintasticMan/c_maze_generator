#include "maze.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct maze {
    size_t rows;
    size_t columns;
    size_t visited;
    bool **cells;
    bool **horiz_walls;
    bool **vert_walls;
};

bool maze_gen(struct maze *mz, size_t row, size_t column);
bool maze_try_move(
    struct maze *mz,
    size_t row,
    size_t column,
    uint8_t choice
);
bool maze_path(struct maze *mz, size_t row, size_t column);

struct maze *maze_create(size_t const rows, size_t const columns) {
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

void maze_destroy(struct maze *const mz) {
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

void maze_generate(struct maze *const mz) {
    maze_gen(mz, 0, 0);
    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            mz->cells[row][column] = false;
        }
    }
}

bool maze_gen(struct maze *const mz, size_t const row, size_t const column) {
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

            if (maze_try_move(mz, row, column, choice)) {
                return true;
            }
        }
    }

    return false;
}

bool maze_try_move(
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
                if (maze_gen(mz, row - 1, column)) {
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
                if (maze_gen(mz, row, column - 1)) {
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
                if (maze_gen(mz, row + 1, column)) {
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
                if (maze_gen(mz, row, column + 1)) {
                    return true;
                }
            }
            break;
    }

    return false;
}

void maze_find_path(struct maze *const mz) {
    maze_path(mz, 0, 0);
}

bool maze_path(struct maze *const mz, size_t const row, size_t const column) {
    mz->cells[row][column] = true;

    if (row == mz->rows - 1 && column == mz->columns - 1) {
        return true;
    }

    if (
        !mz->horiz_walls[row][column] && row > 0
        && !mz->cells[row - 1][column]
        && maze_path(mz, row - 1, column)
    ) {
        return true;
    }

    if (
        !mz->vert_walls[row][column] && column > 0
        && !mz->cells[row][column - 1]
        && maze_path(mz, row, column - 1)
    ) {
        return true;
    }

    if (
        !mz->horiz_walls[row + 1][column] && row + 1 < mz->rows
        && !mz->cells[row + 1][column]
        && maze_path(mz, row + 1, column)
    ) {
        return true;
    }

    if (
        !mz->vert_walls[row][column + 1] && column + 1 < mz->columns
        && !mz->cells[row][column + 1]
        && maze_path(mz, row, column + 1)
    ) {
        return true;
    }

    mz->cells[row][column] = false;

    return false;
}

void maze_print(FILE *const fp, struct maze const *const mz) {
    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(fp, mz->horiz_walls[row][column] ? "+---" : "+   ");
        }
        fprintf(fp, "+\n");

        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(fp, mz->vert_walls[row][column] ? "|" : " ");
            fprintf(fp, mz->cells[row][column] ? " . " : "   ");
        }
        fprintf(fp, mz->vert_walls[row][mz->columns] ? "|\n" : " \n");
    }
    for (size_t column = 0; column < mz->columns; column++) {
        fprintf(fp, mz->horiz_walls[mz->rows][column] ? "+---" : "+   ");
    }
    fprintf(fp, "+\n");
}
