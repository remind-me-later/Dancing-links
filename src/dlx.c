#include "dlx.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Structs
typedef struct dlx_node_struct {
    struct dlx_node_struct *up, *down, *left, *right;

    union {
	struct {
	    struct dlx_node_struct *column;
	    void *subset_label;
	};
	unsigned int size;
    };
} dlx_node;

struct dlx_universe_struct {
    dlx_node root;

    dlx_node *column_headers;
    size_t column_headers_size;

    dlx_node **subsets;
    size_t subsets_size;

    dlx_node **solution_stack;
    size_t solution_stack_size;

    void (*solution_handler)(void **, size_t);

    unsigned int number_of_solutions_found;
    void **current_solution;
};

// Macros
#define FOREACH(ITER, NODE, DIRECTION)                                         \
    for (dlx_node *ITER = (NODE)->DIRECTION; ITER != (NODE);                   \
	 ITER = ITER->DIRECTION)

/* Node functions */

void append_self_horizontally(dlx_node *node) {
    node->right = node;
    node->left = node;
}

void append_self_vertically(dlx_node *node) {
    node->up = node;
    node->down = node;
}

void append_left(dlx_node *node, dlx_node *left) {
    node->right = left->right;
    node->left = left;
    left->right->left = node;
    node->left->right = node;
}

void append_above(dlx_node *node, dlx_node *up) {
    node->down = up->down;
    node->up = up;
    up->down->up = node;
    node->up->down = node;
}

void cover(dlx_node *column) {
    column->left->right = column->right;
    column->right->left = column->left;

    FOREACH(row, column, down) {
	FOREACH(it, row, right) {
	    it->up->down = it->down;
	    it->down->up = it->up;
	    --it->column->size;
	}
    }
}

void uncover(dlx_node *column) {
    FOREACH(row, column, up) {
	FOREACH(it, row, left) {
	    it->up->down = it;
	    it->down->up = it;
	    ++it->column->size;
	}
    }

    column->left->right = column;
    column->right->left = column;
}

dlx_node *choose_column(dlx_universe u) {
    dlx_node *it, *column = u->root.right;

    for (it = column->right; it != &u->root; it = it->right) {
	if (it->size < column->size) {
	    column = it;
	}
    }

    return column;
}

// Library functions

dlx_universe dlx_universe_new(
    void (*solution_handler)(void **, size_t),
    size_t number_of_primary_constraints,
    size_t number_of_secondary_constraints, size_t number_of_subsets) {
    dlx_universe u = malloc(sizeof(*u));

    if (u == NULL) {
	return NULL;
    }

    size_t number_of_constraints =
	number_of_primary_constraints + number_of_secondary_constraints;

    u->column_headers = malloc(sizeof(dlx_node) * number_of_constraints);

    if (u->column_headers == NULL) {
	return NULL;
    }

    u->column_headers_size = number_of_constraints;

    u->subsets = malloc(sizeof(dlx_node *) * number_of_subsets);

    if (u->subsets == NULL) {
	return NULL;
    }

    u->subsets_size = 0;

    u->solution_stack = malloc(sizeof(dlx_node *) * number_of_constraints);

    if (u->solution_stack == NULL) {
	return NULL;
    }

    u->solution_stack_size = 0;

    u->current_solution = malloc(sizeof(void *) * number_of_constraints);

    if (u->current_solution == NULL) {
	return NULL;
    }

    u->number_of_solutions_found = 0;

    u->solution_handler = solution_handler;

    append_self_vertically(&u->root);
    append_self_horizontally(&u->root);

    for (size_t i = 0; i < number_of_primary_constraints; ++i) {
	append_self_vertically(u->column_headers + i);
	append_left(u->column_headers + i, u->root.left);
	u->column_headers[i].size = 0;
    }

    for (size_t i = number_of_primary_constraints; i < number_of_constraints;
	 ++i) {
	append_self_vertically(u->column_headers + i);
	append_self_horizontally(u->column_headers + i);
	u->column_headers[i].size = 0;
    }

    return u;
}

void dlx_universe_delete(dlx_universe u) {
    for (size_t i = 0; i < u->subsets_size; ++i) {
	free(u->subsets[i]);
    }

    free(u->subsets);
    free(u->current_solution);
    free(u->solution_stack);
    free(u->column_headers);
    free(u);
}

void dlx_universe_add_subset(
    dlx_universe universe, size_t subset_size, void *subset_label, ...) {
    va_list args;
    dlx_node *subset = malloc(sizeof(dlx_node) * subset_size);

    va_start(args, subset_label);

    append_self_horizontally(subset);

    for (unsigned int i = 0; i < subset_size; ++i) {
	subset[i].subset_label = subset_label;
	subset[i].column =
	    universe->column_headers + va_arg(args, unsigned int);
	append_above(subset + i, subset[i].column);
	append_left(subset + i, subset[0].left);
	++subset[i].column->size;
    }

    va_end(args);

    universe->subsets[universe->subsets_size++] = subset;
}

void **dlx_get_solution(dlx_universe universe) {
    for (size_t i = 0; i < universe->solution_stack_size; ++i) {
	universe->current_solution[i] =
	    universe->solution_stack[i]->subset_label;
    }

    return universe->current_solution;
}

void dlx_universe_search(
    dlx_universe universe, unsigned int desired_number_of_solutions) {
    if (universe->root.right == &universe->root) {
	(*universe->solution_handler)(
	    dlx_get_solution(universe), universe->solution_stack_size);

	++universe->number_of_solutions_found;
	return;
    }

    dlx_node *column = choose_column(universe);

    cover(column);

    FOREACH(r, column, down) {
	universe->solution_stack[universe->solution_stack_size++] = r;

	FOREACH(j, r, right) { cover(j->column); }

	dlx_universe_search(universe, desired_number_of_solutions);

	r = universe->solution_stack[--universe->solution_stack_size];

	column = r->column;

	FOREACH(j, r, left) { uncover(j->column); }

	if (desired_number_of_solutions &&
	    universe->number_of_solutions_found ==
		desired_number_of_solutions) {
	    break;
	}
    }

    uncover(column);
}
