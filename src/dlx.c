#include "dlx.h"

#include "vector.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

// Structs
typedef struct dlx_data_struct {
	struct dlx_data_struct *up, *down, *left, *right;
	void *                  ref;

	union {
		union {
			struct dlx_data_struct *column;
			uint                    col_pos;
		};
		union {
			bool cons_type;
			uint size;
		};
	};
} dlx_data_t;

VECT_GENERATE_NAME(void *, void)
VECT_GENERATE_NAME(dlx_data_t, data)
VECT_GENERATE_NAME(vect_data *, vect_data)
VECT_GENERATE_NAME(dlx_data_t *, data_ptr)

struct Universe {
	dlx_data_t root;

	vect_data *     elems;
	vect_vect_data *subsets;

	vect_data_ptr *sol;

	uint solutions;
	void (*sol_handler)(void **, uint);
};

// Macros

#define FOREACH(ITER, NODE, DIRECTION)                             \
	for (dlx_data_t *ITER = (NODE)->DIRECTION; ITER != (NODE); \
	     ITER             = ITER->DIRECTION)

#define SELF_LR(NODE)                   \
	do {                            \
		(NODE)->right = (NODE); \
		(NODE)->left  = (NODE); \
	} while (0)

#define SELF_UD(NODE)                  \
	do {                           \
		(NODE)->up   = (NODE); \
		(NODE)->down = (NODE); \
	} while (0)

#define APPEND_LR(NODE, LEFT)                        \
	do {                                         \
		(NODE)->right       = (LEFT)->right; \
		(NODE)->left        = (LEFT);        \
		(LEFT)->right->left = (NODE);        \
		(NODE)->left->right = (NODE);        \
	} while (0)

#define APPEND_UD(NODE, UP)                    \
	do {                                   \
		(NODE)->down     = (UP)->down; \
		(NODE)->up       = (UP);       \
		(UP)->down->up   = (NODE);     \
		(NODE)->up->down = (NODE);     \
	} while (0)

/* Internal functions */

void cover(dlx_data_t *column) {
	column->left->right = column->right;
	column->right->left = column->left;

	FOREACH (row, column, down) {
		FOREACH (it, row, right) {
			it->up->down = it->down;
			it->down->up = it->up;
			--it->column->size;
		}
	}
}

void uncover(dlx_data_t *column) {
	FOREACH (row, column, up) {
		FOREACH (it, row, left) {
			it->up->down = it;
			it->down->up = it;
			++it->column->size;
		}
	}

	column->left->right = column;
	column->right->left = column;
}

dlx_data_t *choose_column(struct Universe *u) {
	dlx_data_t *it, *column = u->root.right;

	for (it = column->right; it != &u->root; it = it->right) {
		if (it->size < column->size) {
			column = it;
		}
	}

	return column;
}

// Library functions

struct Universe *dlx_create_universe(void (*sol_handler)(void **, uint)) {
	struct Universe *u = malloc(sizeof(*u));

	if (u == NULL) {
		return NULL;
	}

	u->elems   = vect_init_data(0);
	u->subsets = vect_init_vect_data(0);

	u->sol       = vect_init_data_ptr(0);
	u->solutions = 0;

	u->sol_handler = sol_handler;

	SELF_UD(&u->root);
	SELF_LR(&u->root);

	return u;
}

void dlx_delete_universe(struct Universe *u) {
	for (uint i = 0; i < u->subsets->size; ++i) {
		vect_free(vect_at(u->subsets, i));
	}

	vect_free(u->elems);
	vect_free(u->subsets);

	vect_free(u->sol);

	free(u);
}

void dlx_add_constraint(struct Universe *u, bool primary, void *ref) {
	dlx_data_t cons;

	cons.ref       = ref;
	cons.cons_type = primary;

	vect_push(u->elems, cons);
}

void dlx_add_subset(struct Universe *u, uint size, void *ref, ...) {
	va_list    args;
	vect_data *subset = vect_init_data(size);
	dlx_data_t data;

	va_start(args, ref);

	for (uint i = 0; i < size; ++i) {
		data.ref     = ref;
		data.col_pos = va_arg(args, uint);
		vect_push(subset, data);
	}

	va_end(args);

	vect_push(u->subsets, subset);
}

void dlx_create_links(struct Universe *u) {
	for (uint i = 0; i < u->elems->size; ++i) {
		SELF_UD(u->elems->data + i);

		if (vect_at(u->elems, i).cons_type == DLX_PRIMARY)
			APPEND_LR(u->elems->data + i, u->root.left);
		else
			SELF_LR(u->elems->data + i);

		vect_at(u->elems, i).size = 0;
	}

	for (uint i = 0; i < u->subsets->size; ++i) {
		SELF_LR(vect_at(u->subsets, i)->data);

		for (uint j = 0; j < vect_at(u->subsets, i)->size; ++j) {
			vect_at(vect_at(u->subsets, i), j).column =
			    u->elems->data +
			    vect_at(vect_at(u->subsets, i), j).col_pos;
			APPEND_UD(vect_at(u->subsets, i)->data + j,
			          vect_at(vect_at(u->subsets, i), j).column);
			APPEND_LR(vect_at(u->subsets, i)->data + j,
			          vect_at(vect_at(u->subsets, i), 0).left);
			++vect_at(vect_at(u->subsets, i), j).column->size;
		}
	}
}

void **dlx_get_solution(struct Universe *u) {
	void **new_sol = malloc(u->sol->size * sizeof(void *));

	for (uint i = 0; i < u->sol->size; ++i) {
		new_sol[i] = vect_at(u->sol, i)->ref;
	}

	return new_sol;
}

void dlx_recurse(struct Universe *u, uint nsol) {
	dlx_data_t *c;

	if (u->root.right == &u->root) {
		int    size     = u->sol->size;
		void **solution = dlx_get_solution(u);

		(*u->sol_handler)(solution, size);

		++u->solutions;
		return;
	}

	c = choose_column(u);

	cover(c);

	FOREACH (r, c, down) {
		vect_push(u->sol, r);

		FOREACH (j, r, right) { cover(j->column); }

		dlx_recurse(u, nsol);

		r = vect_pop(u->sol);
		c = r->column;

		FOREACH (j, r, left) { uncover(j->column); }

		if (nsol && u->solutions == nsol) {
			break;
		}
	}

	uncover(c);
}

void dlx_search(struct Universe *u, uint nsol) {
	dlx_create_links(u);
	dlx_recurse(u, nsol);
}
