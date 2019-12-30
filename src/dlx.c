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
		union {
			char primary;
			unsigned int size;
		};
	};
};

CREATE_VECTOR(char *, string)

struct Set {
	struct Data *elem;
	size_t len;
};

struct Universe {
	string_vector sols;

	struct Data root;
	struct Set elems;
	struct Set *subsets;
	struct Data **cur_sol;

	unsigned int cur_len, ss_len;
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
__attribute__((hot))
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
__attribute__((hot))
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
__attribute__((hot))
choose_column(struct Universe *u)
{
	struct Data *it, *column = u->root.right;

	for (it = column->right; it != &u->root; it = it->right)
		if (it->size < column->size)
			column = it;

	return column;
}

/* Library functions */

struct Universe *
dlx_create_universe(unsigned int elements, unsigned int subsets,
                    unsigned int solutions)
{
	struct Universe *u = malloc(sizeof(*u));

	u->elems.elem = malloc(elements * sizeof(*u->elems.elem));
	u->cur_sol = malloc(elements * sizeof(*u->cur_sol));
	u->subsets = malloc(subsets * sizeof(*u->subsets));
	u->sols = vector_string_new(solutions);

	u->cur_len = 0;
	u->elems.len = 0;
	u->ss_len = 0;

	u->root.name = "root";
	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void
dlx_delete_universe(struct Universe *u)
{
	unsigned int i;

	for (i = 0; i < u->ss_len; ++i)
		free(u->subsets[i].elem);

	for (i = 0; i < u->sols.len; ++i)
		free(u->sols.at[i]);

	free(u->cur_sol);
	free(u->elems.elem);
	free(u->subsets);
	vector_string_delete(&u->sols);

	free(u);
}

void
dlx_add_constraints(struct Universe *u, char *constraints, char primary)
{
	char *elem_name = strtok(constraints, ",");

	for (; elem_name != NULL; elem_name = strtok(NULL, ",")) {
		u->elems.elem[u->elems.len].name = trim(elem_name);
		u->elems.elem[u->elems.len].primary = primary;
		++u->elems.len;
	}
}

void
dlx_add_subset(struct Universe *u, unsigned int size, char *name, ...)
{
	va_list args;
	struct Data *subset = malloc(size * sizeof(*subset));
	size_t *i = &u->subsets[u->ss_len].len;

	va_start(args, name);

	for (*i = 0; *i < size; ++*i) {
		subset[*i].name = name;
		subset[*i].col_pos = va_arg(args, unsigned int);
	}

	va_end(args);

	u->subsets[u->ss_len++].elem = subset;
}

void
dlx_create_links(struct Universe *u)
{
	unsigned int i, j;

	for (i = 0; i < u->elems.len; ++i) {
		SELF_UD(u->elems.elem + i);

		if (u->elems.elem[i].primary)
			APPEND_LR(u->elems.elem + i, u->root.left);
		else
			SELF_LR(u->elems.elem + i);

		u->elems.elem[i].size = 0;
	}

	for (i = 0; i < u->ss_len; ++i) {
		SELF_LR(u->subsets[i].elem);
		for (j = 0; j < u->subsets[i].len; ++j) {
			u->subsets[i].elem[j].column =
			        u->elems.elem + u->subsets[i].elem[j].col_pos;
			APPEND_UD(u->subsets[i].elem + j,
			          u->subsets[i].elem[j].column);
			APPEND_LR(u->subsets[i].elem + j,
			          u->subsets[i].elem->left);
			++u->subsets[i].elem[j].column->size;
		}
	}
}

void
dlx_push_solution(struct Universe *u)
{
	unsigned int i, len;
	char *str;

	for (i = len = 0; i < u->cur_len; ++i)
		len += strlen(u->cur_sol[i]->name);

	str = malloc((len + (u->cur_len - 1) * 2 + 1) * sizeof(*str));

	for (i = len = 0; i < u->cur_len - 1; ++i)
		len += sprintf(str + len, "%s, ", u->cur_sol[i]->name);

	len += sprintf(str + len, "%s", u->cur_sol[i]->name);

	vector_string_push(&u->sols, str);
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
		u->cur_sol[u->cur_len++] = r;

		FOREACH(j, r, right) cover(j->column);

		dlx_search(u, nsol);

		r = u->cur_sol[--u->cur_len];
		c = r->column;

		FOREACH(j, r, left) uncover(j->column);

		if (__builtin_expect(nsol && u->sols.len == nsol, 0))
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
	for (i = 0; i < u->elems.len - 1; ++i)
		printf("%s, ", u->elems.elem[i].name);
	printf("%s}\n", u->elems.elem[u->elems.len - 1].name);

	for (i = 0; i < u->ss_len; ++i) {
		printf("%s = {", u->subsets[i].elem->name);
		for (j = 0; j < u->subsets[i].len - 1; ++j)
			printf("%s, ", u->subsets[i].elem[j].column->name);
		printf("%s}\n",
		       u->subsets[i].elem[u->subsets[i].len - 1].column->name);
	}
}

void
dlx_print_solutions(struct Universe *u)
{
	unsigned int i;

	for (i = 0; i < u->sols.len; ++i)
		printf("S%u* = {%s}\n", i + 1, u->sols.at[i]);
}

