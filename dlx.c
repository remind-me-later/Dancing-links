#include "dlx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define FOREACH(ITER, NODE, DIRECTION, LAMBDA) do { \
		struct Data *ITER; \
		for(ITER = (NODE)->DIRECTION; \
		    ITER != (NODE); ITER = ITER->DIRECTION) { \
			{LAMBDA} \
		} \
	} while (0)

#define FOREACH_SUBSET(ITER, NODE, LAMBDA) do { \
		struct Data *(ITER) = (NODE); \
		do { \
			{LAMBDA} \
			(ITER) = (ITER)->right;\
		} while ((ITER) != (NODE)); \
	} while (0)

#define SELF_LR(NODE) do {\
		(NODE)->right = (NODE); \
		(NODE)->left = (NODE); \
	} while (0)

#define SELF_UD(NODE) do {\
		(NODE)->up = (NODE); \
		(NODE)->down = (NODE); \
	} while (0)

#define APPEND_LR(NODE, LEFT) do { \
		(NODE)->right = (LEFT)->right; \
		(NODE)->left = (LEFT); \
		(LEFT)->right->left = (NODE); \
		(LEFT)->right = (NODE); \
	} while (0)

#define APPEND_UD(NODE, UP) do { \
		(NODE)->down = (UP)->down; \
		(NODE)->up = (UP); \
		(UP)->down->up = (NODE); \
		(UP)->down = (NODE); \
	} while (0)

/*
 * Structs
 */

struct Data {
	struct Data *up, *down, *left, *right;
	char *name;

	union {
		struct Data *column;
		unsigned int size;
	};
};

struct Vector {
	struct Data **at;
	unsigned int len, size;
};

struct Universe {
	struct Data *root;
	struct Vector allocated;
	struct Vector solution;
	unsigned int solutions;
};

/*
 * Vector functions
 */
struct Vector
new_vector(unsigned int size)
{
	struct Vector vector;

	vector.at = malloc(size * sizeof(*vector.at));
	vector.size = size;
	vector.len = 0;

	return vector;
}

void
resize(struct Vector *vector)
{
	vector->size *= 2;
	vector->at =
	        realloc(vector->at, vector->size * sizeof(*vector->at));
}

void
push(struct Vector *vector, struct Data *data)
{
	if (vector->len + 1 == vector->size)
		resize(vector);

	vector->at[vector->len++] = data;
}

struct Data *
pop(struct Vector *vector)
{
	return vector->len == 0 ? NULL : vector->at[--vector->len];
}

/*
 * Functions
 */

void
dlx_print_universe(struct Universe *universe)
{
	unsigned int i;

	printf("U = {");
	for (i = 1; i < universe->root->size; ++i) {
		printf("%s", universe->root[i].name);
		if (i + 1 != universe->root->size)
			printf(", ");
	}
	printf("}\n\n");

	for (i = 0; i < universe->allocated.len; ++i) {
		printf("%s = {", universe->allocated.at[i]->name);
		FOREACH_SUBSET(it, universe->allocated.at[i],
		               printf("%s", it->column->name);
		               if (it->right != universe->allocated.at[i])
		               printf(", ");
		              );
		puts("}");
	}
}

void
dlx_destroy_universe(struct Universe *universe)
{
	unsigned int i;

	free(universe->root);

	for (i = 0; i < universe->allocated.len; ++i)
		free(universe->allocated.at[i]);

	free(universe);
}

struct Universe *
dlx_create_universe(unsigned int subsets,
                    unsigned int primary_elements,
                    unsigned int secondary_elements,
                    char **primary, char **secondary)
{
	unsigned int i;
	struct Universe *universe = malloc(sizeof(*universe));
	struct Data *root = malloc((primary_elements + secondary_elements + 1) *
	                           sizeof(*root));

	root->name = "root";
	root->size = (primary_elements + secondary_elements) + 1;
	SELF_LR(root);
	SELF_UD(root);

	for (i = 1; i < primary_elements + 1; ++i) {
		(root + i)->name = primary[i - 1];
		(root + i)->size = 0;
		SELF_UD(root + i);
		APPEND_LR(root + i, root + i - 1);
	}

	for (; i < primary_elements + secondary_elements + 1; ++i) {
		(root + i)->name = secondary[i - primary_elements - 1];
		(root + i)->size = 0;
		SELF_UD(root + i);
		SELF_LR(root + i);
	}

	universe->root = root;
	universe->solution = new_vector(primary_elements + secondary_elements);
	universe->allocated = new_vector(subsets);
	universe->solutions = 0;

	return universe;
}

void
dlx_add_subset(struct Universe *universe,
               char *name, unsigned int size, ...)
{
	va_list args;
	struct Data *subset = malloc(size * sizeof(*subset));

	va_start(args, size);

	subset->column = universe->root + 1 + va_arg(args, unsigned int);
	subset->name = name;
	APPEND_UD(subset, subset->column);
	SELF_LR(subset);
	++subset->column->size;

	for (unsigned int i = 1; i < size; ++i) {
		(subset + i)->column =
		        universe->root + 1 + va_arg(args, unsigned int);
		(subset + i)->name = name;
		APPEND_UD(subset + i, (subset + i)->column);
		APPEND_LR(subset + i, subset + i - 1);
		++(subset + i)->column->size;
	}

	va_end(args);

	push(&universe->allocated, subset);
}


void
cover(struct Data *column)
{
	column->left->right = column->right;
	column->right->left = column->left;

	FOREACH(row, column, down, FOREACH(it, row, right,
	                                   it->up->down = it->down;
	                                   it->down->up = it->up;
	                                   --it->column->size;
	                                  ););
}

void
uncover(struct Data *column)
{
	FOREACH(row, column, up, FOREACH(it, row, left,
	                                 it->up->down = it;
	                                 it->down->up = it;
	                                 ++it->column->size;
	                                ););

	column->left->right = column;
	column->right->left = column;
}

struct Data *
choose_column(struct Data *root)
{
	struct Data *column = root->right;

	FOREACH(it, root, right, if (it->size < column->size) column = it;);

	return column;
}

void
print_solution(struct Universe *universe)
{
	unsigned int i;

	printf("S%u* = {", ++universe->solutions);
	for (i = 0; i < universe->solution.len; ++i) {
		printf("%s", universe->solution.at[i]->name);
		if (i + 1 != universe->solution.len)
			printf(", ");
	}
	puts("}");
}

void
dlx_search_all(struct Universe *universe)
{
	struct Data *c;

	if (universe->root->right == universe->root) {
		print_solution(universe);
		return;
	}

	cover(c = choose_column(universe->root));

	FOREACH(r, c, down,
	        push(&universe->solution, r);

	        FOREACH(j, r, right, cover(j->column););

	        dlx_search_all(universe);

	        c = (r = pop(&universe->solution))->column;

	        FOREACH(j, r, left, uncover(j->column););
	       );

	uncover(c);
}

void
dlx_search_any(struct Universe *universe)
{
	struct Data *c;

	if (universe->root->right == universe->root) {
		print_solution(universe);
		return;
	}

	cover(c = choose_column(universe->root));

	FOREACH(r, c, down,
	        push(&universe->solution, r);

	        FOREACH(j, r, right, cover(j->column););

	        dlx_search_any(universe);

	        c = (r = pop(&universe->solution))->column;

	        FOREACH(j, r, left, uncover(j->column););

	if (universe->solutions > 0) {
	uncover(c);
		return;
	}
	       );

	uncover(c);
}
