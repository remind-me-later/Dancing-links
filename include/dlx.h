#ifndef __DLX_H__
#define __DLX_H__

typedef struct Universe *dlx_universe;

void dlx_add_subset(struct Universe *universe, char *name,
                    unsigned int size, ...);

struct Universe *dlx_create_universe();

void dlx_destroy_universe(struct Universe *universe);

void dlx_add_primary_constraints(struct Universe *u, char *constraints);

void dlx_add_secondary_constraints(struct Universe *u, char *constraints);

void dlx_search_all(struct Universe *universe);

void dlx_search_any(struct Universe *universe);

void dlx_print_universe(struct Universe *universe);

void dlx_print_solutions(struct Universe *u);

#endif
