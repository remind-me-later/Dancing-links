typedef struct Universe *dlx_universe;

void dlx_add_subset(struct Universe *universe, char *name,
                    unsigned int size, ...);

struct Universe *dlx_create_universe(unsigned int subsets,
                                     unsigned int primary_elements,
                                     unsigned int secondary_elements,
				     char **primary, char **secondary);

void dlx_destroy_universe(struct Universe *universe);

void dlx_search_all(struct Universe *universe);

void dlx_search_any(struct Universe *universe);

void dlx_print_universe(struct Universe *universe);
