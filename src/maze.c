#include "maze.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ASSERT_RUNTIME(c)                                                      \
    do {                                                                       \
        if (!(c)) {                                                            \
            fprintf(                                                           \
                stderr,                                                        \
                "%s:%d: %s: Assertion '%s' failed.\n",                         \
                __FILE__,                                                      \
                __LINE__,                                                      \
                __func__,                                                      \
                #c                                                             \
            );                                                                 \
            exit(1);                                                           \
        }                                                                      \
    } while (false)

#define IS_BIT_SET(v, n) ((v) & (0x1 << (n)))
#define SET_BIT(v, n)                                                          \
    do {                                                                       \
        (v) |= 0x1 << (n);                                                     \
    } while (false)
#define FLIP_BIT(v, n)                                                         \
    do {                                                                       \
        (v) ^= 0x1 << (n);                                                     \
    } while (false)

#define IS_BIT_SET_8ARRAY(v, n) IS_BIT_SET((v)[(n) / 0x8], (n) % 0x8)
#define SET_BIT_8ARRAY(v, n) SET_BIT((v)[(n) / 0x8], (n) % 0x8)
#define FLIP_BIT_8ARRAY(v, n) FLIP_BIT((v)[(n) / 0x8], (n) % 0x8)

struct maze {
    size_t rows;
    size_t columns;
    uint8_t *cells;
    uint8_t *horiz_walls;
    uint8_t *vert_walls;
};

struct maze *maze_create(size_t const rows, size_t const columns) {
    ASSERT_RUNTIME(rows > 0 && columns > 0);

    struct maze *const mz = malloc(sizeof(struct maze));
    ASSERT_RUNTIME(mz);

    mz->rows = rows;
    mz->columns = columns;
    mz->cells = calloc((rows * columns + 7) / 8, sizeof(uint8_t));
    mz->horiz_walls = malloc(((rows + 1) * columns + 7) / 8 * sizeof(uint8_t));
    mz->vert_walls = malloc((rows * (columns + 1) + 7) / 8 * sizeof(uint8_t));
    ASSERT_RUNTIME(mz->cells && mz->horiz_walls && mz->vert_walls);
    for (size_t i = 0; i < ((rows + 1) * columns + 7) / 8; i++) {
        mz->horiz_walls[i] = UINT8_MAX;
    }
    for (size_t i = 0; i < (rows * (columns + 1) + 7) / 8; i++) {
        mz->vert_walls[i] = UINT8_MAX;
    }

    return mz;
}

void maze_destroy(struct maze *const mz) {
    assert(mz);

    free(mz->cells);
    free(mz->horiz_walls);
    free(mz->vert_walls);

    free(mz);
}

static inline bool maze_try_move(
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
            if (row > 0
                && !IS_BIT_SET_8ARRAY(
                    mz->cells,
                    (row - 1) * mz->columns + column
                )) {
                if (!path
                    == !IS_BIT_SET_8ARRAY(
                        mz->horiz_walls,
                        row * mz->columns + column
                    )) {
                    break;
                }
                if (!path) {
                    FLIP_BIT_8ARRAY(
                        mz->horiz_walls,
                        row * mz->columns + column
                    );
                }
                (*idx)++;
                stack[*idx * 2] = row - 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 1:
            if (column > 0
                && !IS_BIT_SET_8ARRAY(
                    mz->cells,
                    row * mz->columns + column - 1
                )) {
                if (!path
                    == !IS_BIT_SET_8ARRAY(
                        mz->vert_walls,
                        row * (mz->columns + 1) + column
                    )) {
                    break;
                }
                if (!path) {
                    FLIP_BIT_8ARRAY(
                        mz->vert_walls,
                        row * (mz->columns + 1) + column
                    );
                }
                (*idx)++;
                stack[*idx * 2] = row;
                stack[*idx * 2 + 1] = column - 1;
                return true;
            }
            break;

        case 2:
            if (row + 1 < mz->rows
                && !IS_BIT_SET_8ARRAY(
                    mz->cells,
                    (row + 1) * mz->columns + column
                )) {
                if (!path
                    == !IS_BIT_SET_8ARRAY(
                        mz->horiz_walls,
                        (row + 1) * mz->columns + column
                    )) {
                    break;
                }
                if (!path) {
                    FLIP_BIT_8ARRAY(
                        mz->horiz_walls,
                        (row + 1) * mz->columns + column
                    );
                }
                (*idx)++;
                stack[*idx * 2] = row + 1;
                stack[*idx * 2 + 1] = column;
                return true;
            }
            break;

        case 3:
            if (column + 1 < mz->columns
                && !IS_BIT_SET_8ARRAY(
                    mz->cells,
                    row * mz->columns + column + 1
                )) {
                if (!path
                    == !IS_BIT_SET_8ARRAY(
                        mz->vert_walls,
                        row * (mz->columns + 1) + column + 1
                    )) {
                    break;
                }
                if (!path) {
                    FLIP_BIT_8ARRAY(
                        mz->vert_walls,
                        row * (mz->columns + 1) + column + 1
                    );
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
    assert(mz);

    size_t *const stack = malloc(mz->rows * mz->columns * 2 * sizeof(size_t));
    uint8_t *const chosen = malloc(mz->rows * mz->columns * sizeof(uint8_t));
    ASSERT_RUNTIME(stack && chosen);
    stack[0] = 0;
    stack[1] = 0;
    chosen[0] = 0;
    size_t idx = 0;
    size_t visited = 0;

    while (true) {
        size_t const row = stack[idx * 2];
        size_t const column = stack[idx * 2 + 1];

        if (!IS_BIT_SET_8ARRAY(mz->cells, row * mz->columns + column)) {
            visited++;
        }
        SET_BIT_8ARRAY(mz->cells, row * mz->columns + column);

        if (visited == mz->rows * mz->columns) {
            break;
        }

        bool found = false;
        while (!(chosen[idx] & 0x4)) {
            int r;
            do {
                r = rand();
            } while (r > RAND_MAX - (RAND_MAX % 4));
            uint8_t const choice = r % 4;

            if (!IS_BIT_SET(chosen[idx], choice + 0x4)) {
                SET_BIT(chosen[idx], choice + 0x4);
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

    for (size_t i = 0; i < (mz->rows * mz->columns + 7) / 8; i++) {
        mz->cells[i] = 0x0;
    }
}

void maze_find_path(struct maze *const mz) {
    assert(mz);

    size_t *const stack = malloc(mz->rows * mz->columns * 2 * sizeof(size_t));
    uint8_t *const dir = malloc(mz->rows * mz->columns * sizeof(uint8_t));
    ASSERT_RUNTIME(stack && dir);
    stack[0] = 0;
    stack[1] = 0;
    dir[0] = 0;
    size_t idx = 0;

    while (true) {
        size_t const row = stack[idx * 2];
        size_t const column = stack[idx * 2 + 1];

        SET_BIT_8ARRAY(mz->cells, row * mz->columns + column);

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
            FLIP_BIT_8ARRAY(mz->cells, row * mz->columns + column);
            idx--;
        }
    }

    free(stack);
    free(dir);
}

void maze_print(FILE *const fp, struct maze const *const mz) {
    assert(fp && mz);

    for (size_t row = 0; row < mz->rows; row++) {
        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(
                fp,
                IS_BIT_SET_8ARRAY(mz->horiz_walls, row * mz->columns + column)
                    ? "+---"
                    : "+   "
            );
        }
        fprintf(fp, "+\n");

        for (size_t column = 0; column < mz->columns; column++) {
            fprintf(
                fp,
                IS_BIT_SET_8ARRAY(
                    mz->vert_walls,
                    row * (mz->columns + 1) + column
                )
                    ? "|"
                    : " "
            );
            fprintf(
                fp,
                IS_BIT_SET_8ARRAY(mz->cells, row * mz->columns + column) ? " . "
                                                                         : "   "
            );
        }
        fprintf(
            fp,
            IS_BIT_SET_8ARRAY(
                mz->vert_walls,
                row * (mz->columns + 1) + mz->columns
            )
                ? "|\n"
                : " \n"
        );
    }
    for (size_t column = 0; column < mz->columns; column++) {
        fprintf(
            fp,
            IS_BIT_SET_8ARRAY(mz->horiz_walls, mz->rows * mz->columns + column)
                ? "+---"
                : "+   "
        );
    }
    fprintf(fp, "+\n");
}
