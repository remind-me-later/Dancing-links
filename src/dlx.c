#include "dlx.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* Structs */

struct Data {
	struct Data *up, *down, *left, *right;
	void *ref;

	union {
		union {
			struct Data *column;
			unsigned int col_pos;
		};
		union {
			char const_type;
			unsigned int size;
		};
	};
};

VECT_GENERATE_NAME(void *, void)
VECT_GENERATE_NAME(vect_void *, vect_void)
VECT_GENERATE_NAME(struct Data, data)
VECT_GENERATE_NAME(vect_data *, vect_data)
VECT_GENERATE_NAME(struct Data *, data_ptr)

struct Universe {
	struct Data root;

	vect_data *elems;
	vect_vect_data *subsets;

	vect_data_ptr *sol;
	vect_vect_void *sols;
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
	struct Data *it, *column = u->root.right;

	for (it = column->right; it != &u->root; it = it->right)
		if (it->size < column->size)
			column = it;

	return column;
}

/* Library functions */

struct Universe *
dlx_create_universe()
{
	struct Universe *u = malloc(sizeof(*u));

	u->elems = vect_init_data(0);
	u->subsets = vect_init_vect_data(0);

	u->sol = vect_init_data_ptr(0);
	u->sols = vect_init_vect_void(0);

	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void
dlx_delete_universe(struct Universe *u)
{
	unsigned int i;

	for (i = 0; i < u->subsets->size; ++i)
		vect_free(vect_at(u->subsets, i));

	for (i = 0; i < u->sols->size; ++i)
		vect_free(vect_at(u->sols, i));

	vect_free(u->elems);
	vect_free(u->subsets);

	vect_free(u->sol);
	vect_free(u->sols);

	free(u);
}

void
dlx_add_constraint(struct Universe *u, char const_type, void *ref)
{
	struct Data cons;

	cons.ref = ref;
	cons.const_type = const_type;

	vect_push(u->elems, cons);
}

void
dlx_add_subset(struct Universe *u, unsigned int size, void *ref, ...)
{
	va_list args;
	unsigned int i;
	vect_data *subset = vect_init_data(size);
	struct Data data;


	va_start(args, ref);

	for (i = 0; i < size; ++i) {
		data.ref = ref;
		data.col_pos = va_arg(args, unsigned int);
		vect_push(subset, data);
	}

	va_end(args);

	vect_push(u->subsets, subset);
}

void
dlx_create_links(struct Universe *u)
{
	unsigned int i, j;

	for (i = 0; i < u->elems->size; ++i) {
		SELF_UD(u->elems->data + i);

		if (vect_at(u->elems, i).const_type == DLX_PRIMARY)
			APPEND_LR(u->elems->data + i, u->root.left);
		else
			SELF_LR(u->elems->data + i);

		vect_at(u->elems, i).size = 0;
	}

	for (i = 0; i < u->subsets->size; ++i) {
		SELF_LR(vect_at(u->subsets, i)->data);

		for (j = 0; j < vect_at(u->subsets, i)->size; ++j) {
			vect_at(vect_at(u->subsets, i), j).column =
			        u->elems->data + vect_at(
			                vect_at(u->subsets, i), j).col_pos;
			APPEND_UD(vect_at(u->subsets, i)->data + j,
			          vect_at(vect_at(u->subsets, i), j).column);
			APPEND_LR(vect_at(u->subsets, i)->data + j,
			          vect_at(vect_at(u->subsets, i), 0).left);
			++vect_at(vect_at(u->subsets, i), j).column->size;
		}
	}
}

void
dlx_push_solution(struct Universe *u)
{
	unsigned int i;

	vect_void *new_sol = vect_init_void(u->sol->size);
	vect_push(u->sols, new_sol);

	for (i = 0; i < u->sol->size; ++i)
		vect_push(new_sol, vect_at(u->sol, i)->ref);
}

void *
dlx_pop_solution(struct Universe *u)
{
	return vect_at(u->sols, --u->sols->size);
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
		vect_push(u->sol, r);

		FOREACH(j, r, right) cover(j->column);

		dlx_search(u, nsol);

		r = vect_pop(u->sol);
		c = r->column;

		FOREACH(j, r, left) uncover(j->column);

		if (nsol && u->sols->size == nsol)
			break;
	}

	uncover(c);
}

/* Printer functions */

void
dlx_print_universe(struct Universe *u, char *cformat, char *ssformat)
{
	unsigned int i, j;

	printf("U = {");

	for (i = 0; i < u->elems->size - 1; ++i) {
		printf(cformat, vect_at(u->elems, i).ref);
		printf(", ");
	}

	printf(cformat, vect_at(u->elems, i).ref);
	printf("}\n");

	for (i = 0; i < u->subsets->size; ++i) {
		printf(ssformat, vect_at(vect_at(u->subsets, i), 0).ref);
		printf(" = {");

		for (j = 0; j < vect_at(u->subsets, i)->size - 1; ++j) {
			printf(cformat,
			       vect_at(vect_at(u->subsets, i), j).column->ref);
			printf(", ");
		}

		printf(cformat,
		       vect_at(vect_at(u->subsets, i), j).column->ref);
		printf("}\n");
	}
}

void
dlx_print_solutions(struct Universe *u, char *format)
{
	unsigned int i, j;

	for (i = 0; i < u->sols->size; ++i) {
		printf("S%u* = {", i + 1);

		for (j = 0; j + 1 < vect_at(u->sols, i)->size; ++j) {
			printf(format, vect_at(vect_at(u->sols, i), j));
			printf(", ");
		}

		printf(format, vect_at(vect_at(u->sols, i), j));
		printf("}\n");
	}
}

