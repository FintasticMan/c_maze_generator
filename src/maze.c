#include "maze.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct maze {
    size_t rows;
    size_t columns;
    bool *cells;
    bool *horiz_walls;
    bool *vert_walls;
};

struct maze *maze_create(size_t const rows, size_t const columns) {
    assert(rows > 0 && columns > 0);

    struct maze *const mz = malloc(sizeof (struct maze));
    assert(mz);

    mz->rows = rows;
    mz->columns = columns;
    mz->cells = calloc(rows * columns, sizeof (bool));
    mz->horiz_walls = malloc((rows + 1) * columns * sizeof (bool));
    mz->vert_walls = malloc(rows * (columns + 1) * sizeof (bool));
    assert(mz->cells && mz->horiz_walls && mz->vert_walls);
    for (size_t i = 0; i < (rows + 1) * columns; i++) {
        mz->horiz_walls[i] = true;
    }
    for (size_t i = 0; i < rows * (columns + 1); i++) {
        mz->vert_walls[i] = true;
    }

    return mz;
}

void maze_destroy(struct maze *const mz) {
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
            if (row > 0 && !mz->cells[(row - 1) * mz->columns + column]) {
                if (!path == !mz->horiz_walls[row * mz->columns + column]) {
                    break;
                }
                if (!path) {
                    mz->horiz_walls[row * mz->columns + column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row - 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 1:
            if (column > 0 && !mz->cells[row * mz->columns + column - 1]) {
                if (!path == !mz->vert_walls[row * (mz->columns + 1) + column]) {
                    break;
                }
                if (!path) {
                    mz->vert_walls[row * (mz->columns + 1) + column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row;
                stack[*idx * 2 + 1] = column - 1;
                return true;
            }
            break;

        case 2:
            if (row + 1 < mz->rows && !mz->cells[(row + 1) * mz->columns + column]) {
                if (!path == !mz->horiz_walls[(row + 1) * mz->columns + column]) {
                    break;
                }
                if (!path) {
                    mz->horiz_walls[(row + 1) * mz->columns + column] = false;
                }
                (*idx)++;
                stack[*idx * 2] = row + 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 3:
            if (column + 1 < mz->columns && !mz->cells[row * mz->columns + column + 1]) {
                if (!path == !mz->vert_walls[row * (mz->columns + 1) + column + 1]) {
                    break;
                }
                if (!path) {
                    mz->vert_walls[row * (mz->columns + 1) + column + 1] = false;
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
    uint8_t *const chosen = malloc(mz->rows * mz->columns * sizeof (uint8_t));
    assert(stack && chosen);
    stack[0] = 0;
    stack[1] = 0;
    chosen[0] = 0;
    size_t idx = 0;
    size_t visited = 0;

    while (true) {
        size_t const row = stack[idx * 2];
        size_t const column = stack[idx * 2 + 1];

        if (!mz->cells[row * mz->columns + column]) {
            visited++;
        }
        mz->cells[row * mz->columns + column] = true;

        if (visited == mz->rows * mz->columns) {
            break;
        }

        bool found = false;
        while (!(chosen[idx] & 4)) {
            int r;
            do {
                r = rand();
            } while (r > RAND_MAX - (RAND_MAX % 4));
            uint8_t const choice = r % 4;

            if (!(chosen[idx] & (1 << (choice + 4)))) {
                chosen[idx] ^= 1 << (choice + 4);
                chosen[idx]++;

                if (maze_try_move(mz, stack, &idx, choice, false)) {
                    chosen[idx] = 0;
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

    for (size_t i = 0; i < mz->rows * mz->columns; i++) {
        mz->cells[i] = 0;
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

        mz->cells[row * mz->columns + column] = true;

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
            mz->cells[row * mz->columns + column] = false;
            idx--;
        }
    }

    free(stack);
    free(dir);
}

void maze_print(FILE *const fp, struct maze const *const mz) {
    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(fp, mz->horiz_walls[row * mz->columns + column] ? "+---" : "+   ");
        }
        fprintf(fp, "+\n");

        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(fp, mz->vert_walls[row * (mz->columns + 1) + column] ? "|" : " ");
            fprintf(fp, mz->cells[row * mz->columns + column] ? " . " : "   ");
        }
        fprintf(fp, mz->vert_walls[row * (mz->columns + 1) + mz->columns] ? "|\n" : " \n");
    }
    for (size_t column = 0; column < mz->columns; column++) {
        fprintf(fp, mz->horiz_walls[mz->rows * mz->columns + column] ? "+---" : "+   ");
    }
    fprintf(fp, "+\n");
}
