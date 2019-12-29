#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>

#define CREATE_VECTOR(TYPE, NAME)                                         \
	typedef struct {                                                       \
		TYPE* at;                                                      \
		size_t len, cap;                                          \
	} NAME##_vector;                                                       \
	\
	NAME##_vector \
	vector_##NAME##_new(unsigned int cap)                             \
	{                                                                      \
		NAME##_vector vector;                                          \
		\
		vector.at = malloc(cap * sizeof(*vector.at));             \
		vector.cap = cap;                                    \
		vector.len = 0;                                                \
		\
		return vector;                                                 \
	}                                                                      \
	\
	void								       \
	vector_##NAME##_delete(NAME##_vector *vector)			       \
	{								       \
		free(vector->at);					       \
	}								       \
	void                                                                   \
	vector_##NAME##_resize(NAME##_vector *vector)                          \
	{                                                                      \
		vector->cap *= 2;                                         \
		vector->at = realloc(vector->at,                               \
		                     (vector->cap) * sizeof(*vector->at));  \
	}                                                                      \
	\
	void                                                                   \
	vector_##NAME##_push(NAME##_vector *vector, TYPE data)                 \
	{                                                                      \
		if (vector->len + 1 == vector->cap)                       \
			vector_##NAME##_resize(vector);                        \
		\
		vector->at[vector->len++] = data;                              \
	}                                                                      \
	\
	TYPE                                                                   \
	vector_##NAME##_pop(NAME##_vector *vector)                             \
	{                                                                      \
		return vector->at[--vector->len];    \
	}                                                                 

#endif
