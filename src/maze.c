#include "maze.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct maze {
    size_t rows;
    size_t columns;
    bool **cells;
    bool **horiz_walls;
    bool **vert_walls;
};

struct maze *maze_create(size_t const rows, size_t const columns) {
    assert(rows > 0 && columns > 0);

    struct maze *const mz = malloc(sizeof (struct maze));
    assert(mz);

    mz->rows = rows;
    mz->columns = columns;
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

bool maze_try_move(
    struct maze *const mz,
    size_t *const stack,
    size_t *const idx,
    uint8_t const choice,
    bool const path
) {
    size_t const row = stack[*idx * 2];
    size_t const column = stack[*idx * 2 + 1];

    switch (choice) {
        case 0:
            if (row > 0 && !mz->cells[row - 1][column]) {
                if (!path == !mz->horiz_walls[row][column]) {
                    break;
                }
                if (!path) {
                    mz->horiz_walls[row][column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row - 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 1:
            if (column > 0 && !mz->cells[row][column - 1]) {
                if (!path == !mz->vert_walls[row][column]) {
                    break;
                }
                if (!path) {
                    mz->vert_walls[row][column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row;
                stack[*idx * 2 + 1] = column - 1;
                return true;
            }
            break;

        case 2:
            if (row + 1 < mz->rows && !mz->cells[row + 1][column]) {
                if (!path == !mz->horiz_walls[row + 1][column]) {
                    break;
                }
                if (!path) {
                    mz->horiz_walls[row + 1][column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row + 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 3:
            if (column + 1 < mz->columns && !mz->cells[row][column + 1]) {
                if (!path == !mz->vert_walls[row][column + 1]) {
                    break;
                }
                if (!path) {
                    mz->vert_walls[row][column + 1] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row;
                stack[*idx * 2 + 1] = column + 1;
                return true;
            }
            break;
    }

    return false;
}

void maze_gen(struct maze *const mz) {
    size_t *const stack = malloc(mz->rows * mz->columns * 2 * sizeof (size_t));
    bool *const chosen = calloc(mz->rows * mz->columns * 4, sizeof (bool));
    uint8_t *const nb_chosen = calloc(mz->rows * mz->columns, sizeof (uint8_t));
    assert(stack && chosen && nb_chosen);
    stack[0] = 0;
    stack[1] = 0;
    size_t idx = 0;
    size_t visited = 0;

    while (true) {
        size_t const row = stack[idx * 2];
        size_t const column = stack[idx * 2 + 1];

        if (!mz->cells[row][column]) {
            visited++;
        }
        mz->cells[row][column] = true;

        if (visited == mz->rows * mz->columns) {
            break;
        }

        bool found = false;
        while (nb_chosen[idx] < 4) {
            int r;
            do {
                r = rand();
            } while (r > RAND_MAX - (RAND_MAX % 4));
            uint8_t const choice = r % 4;

            if (!chosen[idx * 4 + choice]) {
                chosen[idx * 4 + choice] = true;
                nb_chosen[idx]++;

                if (maze_try_move(mz, stack, &idx, choice, false)) {
                    for (uint8_t i = 0; i < 4; i++) {
                        chosen[idx * 4 + i] = false;
                    }
                    nb_chosen[idx] = 0;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            idx--;
        }
    }

    free(stack);
    free(chosen);
    free(nb_chosen);

    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            mz->cells[row][column] = false;
        }
    }
}

void maze_find_path(struct maze *const mz) {
    size_t *const stack = malloc(mz->rows * mz->columns * 2 * sizeof (size_t));
    uint8_t *const dir = malloc(mz->rows * mz->columns * sizeof (uint8_t));
    assert(stack && dir);
    stack[0] = 0;
    stack[1] = 0;
    dir[0] = 0;
    size_t idx = 0;

    while (true) {
        size_t const row = stack[idx * 2];
        size_t const column = stack[idx * 2 + 1];

        mz->cells[row][column] = true;

        if (row == mz->rows - 1 && column == mz->columns - 1) {
            break;
        }

        bool found = false;
        for (uint8_t choice = dir[idx]; choice < 4; choice++) {
            if (maze_try_move(mz, stack, &idx, choice, true)) {
                dir[idx - 1] = choice + 1;
                dir[idx] = 0;
                found = true;
                break;
            }
        }

        if (!found) {
            mz->cells[row][column] = false;
            idx--;
        }
    }

    free(stack);
    free(dir);
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
