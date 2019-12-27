#include "dlx.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

/* Structs */

struct Data {
	struct Data *up, *down, *left, *right;
	char *name;

	union {
		struct Data *column;
		unsigned int size;
	};
};

CREATE_VECTOR(struct Data *, dataptr)
CREATE_VECTOR(struct Data, data)

struct Universe {
	struct Data root;
	data_vector element;
	dataptr_vector allocated;
	dataptr_vector solution;
	unsigned int solutions;
};

/* Macros */

#define FOREACH(ITER, NODE, DIRECTION) \
	for(struct Data *ITER = (NODE)->DIRECTION; \
	    ITER != (NODE); ITER = ITER->DIRECTION)

#define FOREACH_SUBSET(ITER, NODE, ...) do { \
		struct Data *(ITER) = (NODE); \
		do { \
			__VA_ARGS__ \
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
		(NODE)->left->right = (NODE); \
	} while (0)

#define APPEND_UD(NODE, UP) do { \
		(NODE)->down = (UP)->down; \
		(NODE)->up = (UP); \
		(UP)->down->up = (NODE); \
		(NODE)->up->down = (NODE); \
	} while (0)

/* Functions */

void
dlx_print_universe(struct Universe *u)
{
	unsigned int i;

	printf("U = {");
	for (i = 0; i < u->element.len; ++i) {
		printf("%s", u->element.at[i].name);
		if (i + 1 != u->element.len)
			printf(", ");
	}
	printf("}\n\n");

	for (i = 0; i < u->allocated.len; ++i) {
		printf("%s = {", u->allocated.at[i]->name);
		FOREACH_SUBSET(it, u->allocated.at[i],
		               printf("%s", it->column->name);
		               if (it->right != u->allocated.at[i])
		               printf(", ");
		              );
		puts("}");
	}
}

void
dlx_destroy_universe(struct Universe *u)
{
	vector_data_delete(&u->element);

	for (unsigned int i = 0; i < u->allocated.len; ++i)
		free(u->allocated.at[i]);

	vector_dataptr_delete(&u->allocated);
	vector_dataptr_delete(&u->solution);
	free(u);
}

struct Universe *
dlx_create_universe()
{
	struct Universe *u = malloc(sizeof(*u));

	u->element = vector_data_new(128);
	u->solution = vector_dataptr_new(128);
	u->allocated = vector_dataptr_new(128);
	u->solutions = 0;

	u->root.name = "root";
	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void
dlx_add_primary_constraints(struct Universe *u, unsigned int number,
                            char **constraints)
{
	while (u->element.len + number >= u->element.capacity)
		vector_data_resize(&u->element);

	for (unsigned int i = u->element.len; i < u->element.len + number; ++i) {
		u->element.at[i].name = constraints[i];
		u->element.at[i].size = 0;
		SELF_UD(u->element.at + i);
		APPEND_LR(u->element.at + i, u->root.left);
	}

	u->element.len += number;
}

void
dlx_add_secondary_constraints(struct Universe *u, unsigned int number,
                              char **constraints)
{
	while (u->element.len + number >= u->element.capacity)
		vector_data_resize(&u->element);

	for (unsigned int i = u->element.len; i < u->element.len + number; ++i) {
		u->element.at[i].name = constraints[i];
		u->element.at[i].size = 0;
		SELF_UD(u->element.at + i);
		SELF_LR(u->element.at + i);
	}

	u->element.len += number;
}

void
dlx_add_subset(struct Universe *u,
               char *name, unsigned int size, ...)
{
	va_list args;
	struct Data *subset = malloc(size * sizeof(*subset));

	va_start(args, size);

	SELF_LR(subset);
	for (unsigned int i = 0; i < size; ++i) {
		(subset + i)->name = name;
		(subset + i)->column =
		        u->element.at + va_arg(args, unsigned int);
		APPEND_UD(subset + i, (subset + i)->column);
		APPEND_LR(subset + i, subset->left);
		++(subset + i)->column->size;
	}

	va_end(args);

	vector_dataptr_push(&(u->allocated), subset);
}

void
cover(struct Data *column)
{
	column->left->right = column->right;
	column->right->left = column->left;

	FOREACH(row, column, down)
	FOREACH(it, row, right) {
		it->up->down = it->down;
		it->down->up = it->up;
		--it->column->size;
	}
}

void
uncover(struct Data *column)
{
	FOREACH(row, column, up)
	FOREACH(it, row, left) {
		it->up->down = it;
		it->down->up = it;
		++it->column->size;
	}

	column->left->right = column;
	column->right->left = column;
}

struct Data *
choose_column(struct Universe *u)
{
	struct Data *column = u->root.right;

	FOREACH(it, &u->root, right) {
		if (it->size < column->size)
			column = it;
	}

	return column;
}

void
print_solution(struct Universe *u)
{
	printf("S%u* = {", ++u->solutions);
	for (unsigned int i = 0; i < u->solution.len; ++i) {
		printf("%s", u->solution.at[i]->name);
		if (i + 1 != u->solution.len)
			printf(", ");
	}
	puts("}");
}

void
dlx_search_all(struct Universe *u)
{
	struct Data *c;

	if (u->root.right == &u->root) {
		print_solution(u);
		return;
	}

	cover(c = choose_column(u));

	FOREACH(r, c, down) {
		vector_dataptr_push(&u->solution, r);

		FOREACH(j, r, right) cover(j->column);

		dlx_search_all(u);

		c = (r = vector_dataptr_pop(&u->solution))->column;

		FOREACH(j, r, left) uncover(j->column);
	}

	uncover(c);
}

void
dlx_search_any(struct Universe *u)
{
	struct Data *c;

	if (u->root.right == &u->root) {
		print_solution(u);
		return;
	}

	cover(c = choose_column(u));

	FOREACH(r, c, down) {
		vector_dataptr_push(&u->solution, r);

		FOREACH(j, r, right) cover(j->column);

		dlx_search_any(u);

		c = (r = vector_dataptr_pop(&u->solution))->column;

		FOREACH(j, r, left) uncover(j->column);

		if (u->solutions > 0) {
			uncover(c);
			return;
		}
	}

	uncover(c);
}
