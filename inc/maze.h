#ifndef MAZE_H
#define MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

struct maze;

struct maze *create_maze(size_t rows, size_t columns);
void destroy_maze(struct maze *mz);

void generate_maze(struct maze *mz);
void path_maze(struct maze *mz);
void print_maze(struct maze *mz);

#ifdef __cplusplus
}
#endif

#endif
