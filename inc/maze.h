#ifndef MAZE_H
#define MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

struct maze;

struct maze *maze_create(size_t rows, size_t columns);
void maze_destroy(struct maze *mz);

void maze_gen(struct maze *mz);
void maze_find_path(struct maze *mz);
void maze_print(FILE *fp, struct maze const *mz);

#ifdef __cplusplus
}
#endif

#endif
