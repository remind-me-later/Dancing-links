#include "dlx.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

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
CREATE_VECTOR(char, char)
CREATE_VECTOR(char *, string)

struct Universe {
	struct Data root;
	data_vector elem;
	dataptr_vector alloc;
	dataptr_vector sol_stack;
	string_vector sols;
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

/* string utility functions */

char *
ltrim(char *s)
{
	while (isspace(*s))
		s++;
	return s;
}

char *
rtrim(char *s)
{
	char *back = s + strlen(s);
	while (isspace(*--back));
	*(back + 1) = '\0';
	return s;
}

char *
trim(char *s)
{
	return rtrim(ltrim(s));
}

/* Internal functions */

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

/* Library functions */

struct Universe *
dlx_create_universe()
{
	struct Universe *u = malloc(sizeof(*u));

	u->elem = vector_data_new(128);
	u->sol_stack = vector_dataptr_new(128);
	u->sols = vector_string_new(64);
	u->alloc = vector_dataptr_new(128);

	u->root.name = "root";
	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void
dlx_delete_universe(struct Universe *u)
{
	vector_data_delete(&u->elem);

	for (unsigned int i = 0; i < u->alloc.len; ++i)
		free(u->alloc.at[i]);

	for (unsigned int i = 0; i < u->sols.len; ++i)
		free(u->sols.at[i]);

	vector_dataptr_delete(&u->alloc);
	vector_string_delete(&u->sols);
	free(u);
}

void
dlx_add_primary_constraints(struct Universe *u, char *constraints)
{
	char *elem_name = strtok(constraints, ",");

	while (elem_name != NULL) {
		if (u->elem.len + 1 == u->elem.capacity)
			vector_data_resize(&u->elem);

		elem_name = trim(elem_name);
		u->elem.at[u->elem.len].name = elem_name;
		u->elem.at[u->elem.len].size = 0;
		SELF_UD(u->elem.at + u->elem.len);
		APPEND_LR(u->elem.at + u->elem.len, u->root.left);
		++u->elem.len;
		elem_name = strtok(NULL, ",");
	}
}

void
dlx_add_secondary_constraints(struct Universe *u, char *constraints)
{
	char *elem_name = strtok(constraints, ",");

	while (elem_name != NULL) {
		if (u->elem.len + 1 == u->elem.capacity)
			vector_data_resize(&u->elem);

		elem_name = trim(elem_name);
		u->elem.at[u->elem.len].name = elem_name;
		u->elem.at[u->elem.len].size = 0;
		SELF_UD(u->elem.at + u->elem.len);
		SELF_LR(u->elem.at + u->elem.len);
		++u->elem.len;
		elem_name = strtok(NULL, ",");
	}
}

void
dlx_add_subset(struct Universe *u, unsigned int size, char *name, ...)
{
	va_list args;
	struct Data *subset = malloc(size * sizeof(*subset));

	va_start(args, name);

	SELF_LR(subset);
	for (unsigned int i = 0; i < size; ++i) {
		(subset + i)->name = name;
		(subset + i)->column = u->elem.at + va_arg(args, unsigned int);
		APPEND_UD(subset + i, (subset + i)->column);
		APPEND_LR(subset + i, subset->left);
		++(subset + i)->column->size;
	}

	va_end(args);

	vector_dataptr_push(&(u->alloc), subset);
}

void
dlx_push_solution(struct Universe *u)
{
	char_vector str = vector_char_new(16);

	for (unsigned int i = 0; i < u->sol_stack.len - 1; ++i) {
		if (str.len + strlen(u->sol_stack.at[i]->name) >= str.capacity)
			vector_char_resize(&str);

		str.len += sprintf(str.at + str.len, "%s, ",
		                   u->sol_stack.at[i]->name);
	}

	str.len += sprintf(str.at + str.len, "%s",
	                   u->sol_stack.at[u->sol_stack.len - 1]->name);

	vector_string_push(&u->sols, str.at);
}

char *
dlx_pop_solution(struct Universe *u)
{
	return u->sols.at[--u->sols.len];
}

void
dlx_search_all(struct Universe *u)
{
	struct Data *c;

	if (u->root.right == &u->root) {
		dlx_push_solution(u);
		return;
	}

	cover(c = choose_column(u));

	FOREACH(r, c, down) {
		vector_dataptr_push(&u->sol_stack, r);

		FOREACH(j, r, right) cover(j->column);

		dlx_search_all(u);

		r = vector_dataptr_pop(&u->sol_stack);
		c = r->column;

		FOREACH(j, r, left) uncover(j->column);
	}

	uncover(c);
}

void
dlx_search_any(struct Universe *u)
{
	struct Data *c;

	if (u->root.right == &u->root) {
		dlx_push_solution(u);
		return;
	}

	cover(c = choose_column(u));

	FOREACH(r, c, down) {
		vector_dataptr_push(&u->sol_stack, r);

		FOREACH(j, r, right) cover(j->column);

		dlx_search_any(u);

		r = vector_dataptr_pop(&u->sol_stack);
		c = r->column;

		FOREACH(j, r, left) uncover(j->column);

		if (u->sols.len > 0) {
			uncover(c);
			return;
		}
	}

	uncover(c);
}

/* Printer functions */

void
dlx_print_universe(struct Universe *u)
{
	unsigned int i;

	printf("U = {");
	for (i = 0; i < u->elem.len - 1; ++i)
		printf("%s, ", u->elem.at[i].name);
	printf("%s}\n", u->elem.at[u->elem.len - 1].name);

	for (i = 0; i < u->alloc.len; ++i) {
		printf("%s = {", u->alloc.at[i]->name);
		FOREACH_SUBSET(it, u->alloc.at[i],
		               printf("%s", it->column->name);
		               if (it->right != u->alloc.at[i])
		               printf(", ");
		              );
		puts("}");
	}
}

void
dlx_print_solutions(struct Universe *u)
{
	for (unsigned int i = 0; i < u->sols.len; ++i)
		printf("S%u* = {%s}\n", i + 1, u->sols.at[i]);
}

