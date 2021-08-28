#include "dlx.h"
#include <stdarg.h>

#define FOREACH(it, node, direction)                                           \
    for (struct dlx_node *it = (node)->direction; it != (node);                \
	 it = it->direction)

struct dlx_node {
    struct dlx_node *up, *down, *left, *right;

    union {
	struct {
	    struct dlx_node *column;
	    void *subset_label;
	};
	unsigned int size;
    };
};

struct dlx_solution_iterator {
    struct dlx_node **solutions;
    size_t index;
    size_t end;
};

struct dlx_universe {
    struct dlx_node root;

    struct dlx_node *column_headers;
    size_t column_headers_size;

    struct dlx_node **subsets;
    size_t subsets_size;

    struct dlx_node **solution_stack;
    size_t solution_stack_size;

    void (*solution_handler)(struct dlx_solution_iterator *iter);
    struct dlx_solution_iterator solution_iterator;
    unsigned int number_of_solutions_found;
};

// dlx_solution_iterator methods

void dlx_solution_iterator_rewind(struct dlx_solution_iterator *iter) {
    iter->index = 0;
}

void dlx_solution_iterator_init(
    struct dlx_solution_iterator *iter, struct dlx_universe *universe) {
    iter->index = 0;
    iter->end = universe->solution_stack_size;
}

void *dlx_solution_iterator_next(struct dlx_solution_iterator *iter) {
    if (iter->index >= iter->end) {
	return NULL;
    }

    return iter->solutions[iter->index++]->subset_label;
}

size_t dlx_solution_iterator_remaining(struct dlx_solution_iterator *iter) {
    return iter->end - iter->index;
}

// dlx_node methods

void append_self_horizontally(struct dlx_node *node) {
    node->right = node;
    node->left = node;
}

void append_self_vertically(struct dlx_node *node) {
    node->up = node;
    node->down = node;
}

void append_left(struct dlx_node *node, struct dlx_node *left) {
    node->right = left->right;
    node->left = left;
    left->right->left = node;
    node->left->right = node;
}

void append_above(struct dlx_node *node, struct dlx_node *up) {
    node->down = up->down;
    node->up = up;
    up->down->up = node;
    node->up->down = node;
}

void cover(struct dlx_node *column) {
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

void uncover(struct dlx_node *column) {
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

// dlx_universe methods

struct dlx_node *choose_column(struct dlx_universe *u) {
    struct dlx_node *it, *column = u->root.right;

    for (it = column->right; it != &u->root; it = it->right) {
	if (it->size < column->size) {
	    column = it;
	}
    }

    return column;
}

struct dlx_universe *dlx_universe_new(
    void (*solution_handler)(struct dlx_solution_iterator *iter),
    size_t number_of_primary_constraints,
    size_t number_of_secondary_constraints, size_t number_of_subsets) {
    struct dlx_universe *universe = malloc(sizeof(struct dlx_universe));

    if (universe == NULL) {
	return NULL;
    }

    size_t number_of_constraints =
	number_of_primary_constraints + number_of_secondary_constraints;

    universe->column_headers =
	malloc(sizeof(struct dlx_node) * number_of_constraints);

    if (universe->column_headers == NULL) {
	return NULL;
    }

    universe->column_headers_size = number_of_constraints;

    universe->subsets = malloc(sizeof(struct dlx_node *) * number_of_subsets);

    if (universe->subsets == NULL) {
	return NULL;
    }

    universe->subsets_size = 0;

    universe->solution_stack =
	malloc(sizeof(struct dlx_node *) * number_of_constraints);

    if (universe->solution_stack == NULL) {
	return NULL;
    }

    universe->solution_iterator.solutions = universe->solution_stack;

    universe->solution_stack_size = 0;
    universe->number_of_solutions_found = 0;
    universe->solution_handler = solution_handler;

    append_self_vertically(&universe->root);
    append_self_horizontally(&universe->root);

    for (size_t i = 0; i < number_of_primary_constraints; ++i) {
	append_self_vertically(universe->column_headers + i);
	append_left(universe->column_headers + i, universe->root.left);
	universe->column_headers[i].size = 0;
    }

    for (size_t i = number_of_primary_constraints; i < number_of_constraints;
	 ++i) {
	append_self_vertically(universe->column_headers + i);
	append_self_horizontally(universe->column_headers + i);
	universe->column_headers[i].size = 0;
    }

    return universe;
}

void dlx_universe_free(struct dlx_universe *universe) {
    for (size_t i = 0; i < universe->subsets_size; ++i) {
	free(universe->subsets[i]);
    }

    free(universe->subsets);
    free(universe->solution_stack);
    free(universe->column_headers);
    free(universe);
}

void dlx_universe_add_subset(
    struct dlx_universe *universe, size_t subset_size, void *subset_label,
    ...) {
    va_list args;
    struct dlx_node *subset = malloc(sizeof(struct dlx_node) * subset_size);

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

void dlx_universe_search(
    struct dlx_universe *universe, unsigned int desired_number_of_solutions) {
    if (universe->root.right == &universe->root) {
	dlx_solution_iterator_init(&universe->solution_iterator, universe);
	(*universe->solution_handler)(&universe->solution_iterator);
	++universe->number_of_solutions_found;
	return;
    }

    struct dlx_node *column = choose_column(universe);

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
