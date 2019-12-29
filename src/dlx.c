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
		union {
			struct Data *column;
			unsigned int col_pos;
		};
		struct {
			char primary;
			unsigned int size;
		};
	};
};

CREATE_VECTOR(struct Data *, dataptr)
CREATE_VECTOR(struct Data, data)
CREATE_VECTOR(data_vector, datavtr)
CREATE_VECTOR(char, char)
CREATE_VECTOR(char *, string)

struct Universe {
	struct Data root;
	data_vector elem;
	datavtr_vector subsets;
	dataptr_vector sol_stack;
	string_vector sols;
};

/* Macros */

#define FOREACH(ITER, NODE, DIRECTION) \
	for(struct Data *ITER = (NODE)->DIRECTION; \
	    ITER != (NODE); ITER = ITER->DIRECTION)

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

	u->elem = vector_data_new(32);
	u->sol_stack = vector_dataptr_new(32);
	u->sols = vector_string_new(32);
	u->subsets = vector_datavtr_new(32);

	u->root.name = "root";
	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void
dlx_delete_universe(struct Universe *u)
{
	for (unsigned int i = 0; i < u->subsets.len; ++i)
		vector_data_delete(u->subsets.at + i);

	for (unsigned int i = 0; i < u->sols.len; ++i)
		free(u->sols.at[i]);

	vector_data_delete(&u->elem);
	vector_datavtr_delete(&u->subsets);
	vector_dataptr_delete(&u->sol_stack);
	vector_string_delete(&u->sols);

	free(u);
}

void
dlx_add_constraints(struct Universe *u, char *constraints, char primary)
{
	char *elem_name = strtok(constraints, ",");

	for (; elem_name != NULL; elem_name = strtok(NULL, ",")) {
		if (u->elem.len == u->elem.cap)
			vector_data_resize(&u->elem);

		u->elem.at[u->elem.len].name = trim(elem_name);
		u->elem.at[u->elem.len].size = 0;
		u->elem.at[u->elem.len].primary = primary;
		++u->elem.len;
	}
}

void
dlx_add_subset(struct Universe *u, unsigned int size, char *name, ...)
{
	va_list args;
	data_vector subset = vector_data_new(size);

	va_start(args, name);

	for (unsigned int i = 0; i < size; ++i) {
		subset.at[i].name = name;
		subset.at[i].col_pos = va_arg(args, unsigned int);
	}

	va_end(args);

	subset.len = size;

	vector_datavtr_push(&(u->subsets), subset);
}

void
dlx_create_links(struct Universe *u)
{
	unsigned int i, j;

	for (i = 0; i < u->elem.len; ++i) {
		SELF_UD(u->elem.at + i);

		if (u->elem.at[i].primary)
			APPEND_LR(u->elem.at + i, u->root.left);
		else
			SELF_LR(u->elem.at + i);
	}

	for (i = 0; i < u->subsets.len; ++i) {
		SELF_LR(u->subsets.at[i].at);
		for (j = 0; j < u->subsets.at[i].len; ++j) {
			u->subsets.at[i].at[j].column =
			        u->elem.at + u->subsets.at[i].at[j].col_pos;
			++u->subsets.at[i].at[j].column->size;
			APPEND_UD(u->subsets.at[i].at + j,
			          u->subsets.at[i].at[j].column);
			APPEND_LR(u->subsets.at[i].at + j,
			          u->subsets.at[i].at->left);
		}
	}
}

void
dlx_push_solution(struct Universe *u)
{
	unsigned int i;

	char_vector str = vector_char_new(16);

	for (i = 0; i < u->sol_stack.len - 1; ++i) {
		while (str.cap < str.len + strlen(u->sol_stack.at[i]->name) + 3)
			vector_char_resize(&str);

		str.len += sprintf(str.at + str.len,
		                   "%s, ", u->sol_stack.at[i]->name);
	}

	while (str.cap < str.len + strlen(u->sol_stack.at[i]->name) + 1)
		vector_char_resize(&str);

	str.len += sprintf(str.at + str.len, "%s", u->sol_stack.at[i]->name);

	vector_string_push(&u->sols, str.at);
}

char *
dlx_pop_solution(struct Universe *u)
{
	return u->sols.at[--u->sols.len];
}

void
dlx_search(struct Universe *u, unsigned int nsol)
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

		dlx_search(u, nsol);

		r = vector_dataptr_pop(&u->sol_stack);
		c = r->column;

		FOREACH(j, r, left) uncover(j->column);

		if (nsol && u->sols.len == nsol)
			break;
	}

	uncover(c);
}

/* Printer functions */

void
dlx_print_universe(struct Universe *u)
{
	unsigned int i, j;

	printf("U = {");
	for (i = 0; i < u->elem.len - 1; ++i)
		printf("%s, ", u->elem.at[i].name);
	printf("%s}\n", u->elem.at[u->elem.len - 1].name);

	for (i = 0; i < u->subsets.len; ++i) {
		printf("%s = {", u->subsets.at[i].at->name);
		for (j = 0; j < u->subsets.at[i].len - 1; ++j)
			printf("%s, ", u->subsets.at[i].at[j].column->name);
		printf("%s}\n",
		       u->subsets.at[i].at[u->subsets.at[i].len - 1].column->name);
	}
}

void
dlx_print_solutions(struct Universe *u)
{
	for (unsigned int i = 0; i < u->sols.len; ++i)
		printf("S%u* = {%s}\n", i + 1, u->sols.at[i]);
}

