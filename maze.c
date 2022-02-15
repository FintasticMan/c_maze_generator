#include <stdio.h>
#include <stdlib.h>

#include "maze.h"

struct maze {
        size_t     rows;
        size_t     columns;
        size_t     visited;
        bool * * cells;
        bool * * horiz_walls;
        bool * * vert_walls;
};

bool gen_maze(struct maze * mz, size_t row, size_t column);
bool try_move_maze(struct maze * mz,
                   size_t     row,
                   size_t     column,
                   unsigned short     choice);
bool find_path_maze(struct maze * mz, size_t row, size_t column);

struct maze * create_maze(const size_t rows, const size_t columns) {
    struct maze * ret = malloc(sizeof(struct maze));

    ret->rows        = rows;
    ret->columns     = columns;
    ret->visited     = 0U;
    ret->cells       = malloc(rows * sizeof(bool *));
    ret->horiz_walls = malloc((rows + 1U) * sizeof(bool *));
    ret->vert_walls  = malloc(rows * sizeof(bool *));
    for (size_t row = 0U; row < rows; row++) {
        ret->cells[row]       = calloc(columns, sizeof(bool));
        ret->horiz_walls[row] = malloc(columns * sizeof(bool));
        ret->vert_walls[row]  = malloc((columns + 1) * sizeof(bool));
        for (size_t column = 0U; column < columns; column++) {
            ret->horiz_walls[row][column] = true;
            ret->vert_walls[row][column]  = true;
        }
        ret->vert_walls[row][columns] = true;
    }
    ret->horiz_walls[rows] = malloc(columns * sizeof(bool));
    for (size_t column = 0U; column < columns; column++) {
        ret->horiz_walls[rows][column] = true;
    }

    return ret;
}

void destroy_maze(struct maze * mz) {
    for (size_t row = 0U; row < mz->rows; row++) {
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

void generate_maze(struct maze * mz) {
    gen_maze(mz, 0U, 0U);
    for (size_t row = 0U; row < mz->rows; row++) {
        for (size_t column = 0U; column < mz->columns; column++) {
            mz->cells[row][column] = false;
        }
    }
}

bool gen_maze(struct maze * mz, const size_t row, const size_t column) {
    mz->visited++;
    mz->cells[row][column] = true;

    if (mz->visited == mz->rows * mz->columns) {
        return true;
    }

    bool chosen[4U] = {false};
    unsigned short  nb_chosen = 0U;
    while (nb_chosen < 4U) {
        unsigned short choice = rand() / ((RAND_MAX + 1U) / 4U);
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

bool try_move_maze(struct maze * mz,
                   const size_t     row,
                   const size_t     column,
                   const unsigned short choice) {
    switch (choice) {
        case 0U:
            if (mz->horiz_walls[row][column] && row > 0U
                && !mz->cells[row - 1U][column]) {
                mz->horiz_walls[row][column] = false;
                if (gen_maze(mz, row - 1U, column)) {
                    return true;
                }
            }
            break;

        case 1U:
            if (mz->vert_walls[row][column] && column > 0U
                && !mz->cells[row][column - 1U]) {
                mz->vert_walls[row][column] = false;
                if (gen_maze(mz, row, column - 1U)) {
                    return true;
                }
            }
            break;

        case 2U:
            if (mz->horiz_walls[row + 1U][column] && row + 1U < mz->rows
                && !mz->cells[row + 1U][column]) {
                mz->horiz_walls[row + 1U][column] = false;
                if (gen_maze(mz, row + 1U, column)) {
                    return true;
                }
            }
            break;

        case 3U:
            if (mz->vert_walls[row][column + 1U] && column + 1U < mz->columns
                && !mz->cells[row][column + 1U]) {
                mz->vert_walls[row][column + 1U] = false;
                if (gen_maze(mz, row, column + 1U)) {
                    return true;
                }
            }
    }

    return false;
}

void path_maze(struct maze * mz) {
    find_path_maze(mz, 0U, 0U);
}

bool find_path_maze(struct maze * mz, const size_t row, const size_t column) {
    mz->cells[row][column] = true;

    if (row == mz->rows - 1U && column == mz->columns - 1U) {
        return true;
    }

    if (!mz->horiz_walls[row][column] && row > 0U && !mz->cells[row - 1U][column]
        && find_path_maze(mz, row - 1U, column)) {
        return true;
    }

    if (!mz->vert_walls[row][column] && column > 0U
        && !mz->cells[row][column - 1U] && find_path_maze(mz, row, column - 1U)) {
        return true;
    }

    if (!mz->horiz_walls[row + 1U][column] && row + 1U < mz->rows
        && !mz->cells[row + 1U][column] && find_path_maze(mz, row + 1U, column)) {
        return true;
    }

    if (!mz->vert_walls[row][column + 1U] && column + 1U < mz->columns
        && !mz->cells[row][column + 1U] && find_path_maze(mz, row, column + 1U)) {
        return true;
    }

    mz->cells[row][column] = false;

    return false;
}

void print_maze(struct maze * mz) {
    for (size_t row = 0U; row < mz->rows; row++) {
        for (size_t column = 0U; column < mz->columns; column++) {
            printf(mz->horiz_walls[row][column] ? "+---" : "+   ");
        }
        printf("+\n");

        for (size_t column = 0U; column < mz->columns; column++) {
            printf(mz->vert_walls[row][column] ? "|" : " ");
            printf(mz->cells[row][column] ? " . " : "   ");
        }
        printf(mz->vert_walls[row][mz->columns] ? "|\n" : " \n");
    }
    for (size_t column = 0U; column < mz->columns; column++) {
        printf(mz->horiz_walls[mz->rows][column] ? "+---" : "+   ");
    }
    printf("+\n");
}
