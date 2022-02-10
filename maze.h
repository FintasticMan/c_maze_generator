#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

struct maze {
    int rows;
    int columns;
    int visited;
    bool * * cells;
    bool * * horiz_walls;
    bool * * vert_walls;
};

struct maze create_maze(const int rows, const int columns);
void destroy_maze(struct maze * mz);

void generate_maze(struct maze * mz);
bool gen_maze(struct maze * mz, const int row, const int column);
bool try_move_maze(struct maze * mz, const int row, const int column, const int choice);
bool find_path_maze(struct maze * mz, const int row, const int column);

#endif
