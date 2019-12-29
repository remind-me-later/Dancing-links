#ifndef __DLX_H__
#define __DLX_H__

/* Objects */

typedef struct Universe *dlx_universe;

/* Functions */

struct Universe *dlx_create_universe();

void dlx_delete_universe(struct Universe *universe);

void dlx_add_constraints(struct Universe *u, char *constraints, char primary);

void dlx_add_subset(struct Universe *universe, unsigned int size,
                    char *name, ...);

void dlx_create_links(struct Universe *u);

void dlx_search(struct Universe *universe, unsigned int nsol);

char *dlx_pop_solution(struct Universe *universe);

void dlx_print_universe(struct Universe *universe);

void dlx_print_solutions(struct Universe *u);

#endif
