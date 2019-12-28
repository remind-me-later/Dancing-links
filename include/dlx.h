#ifndef __DLX_H__
#define __DLX_H__

/* Objects */

typedef struct Universe *dlx_universe;

/* Functions */

struct Universe *dlx_create_universe();

void dlx_delete_universe(struct Universe *universe);

void dlx_add_primary_constraints(struct Universe *u, char *constraints);

void dlx_add_secondary_constraints(struct Universe *u, char *constraints);

void dlx_add_subset(struct Universe *universe, unsigned int size,
                    char *name, ...);

void dlx_search_all(struct Universe *universe);

void dlx_search_any(struct Universe *universe);

char *dlx_pop_solution(struct Universe *universe);

void dlx_print_universe(struct Universe *universe);

void dlx_print_solutions(struct Universe *u);

#endif
