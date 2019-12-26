#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>

#define CREATE_VECTOR(TYPE, NAME)                                         \
	typedef struct {                                                       \
		TYPE* at;                                                      \
		size_t len, capacity;                                          \
	} NAME##_vector;                                                       \
	\
	NAME##_vector \
	vector_##NAME##_new(unsigned int capacity)                             \
	{                                                                      \
		NAME##_vector vector;                                          \
		\
		vector.at = malloc(capacity * sizeof(*vector.at));             \
		vector.capacity = capacity;                                    \
		vector.len = 0;                                                \
		\
		return vector;                                                 \
	}                                                                      \
	\
	void                                                                   \
	vector_##NAME##_resize(NAME##_vector *vector)                          \
	{                                                                      \
		vector->capacity *= 2;                                         \
		vector->at = realloc(vector->at,                               \
		                     (vector->capacity) * sizeof(*vector->at));  \
	}                                                                      \
	\
	void                                                                   \
	vector_##NAME##_push(NAME##_vector *vector, TYPE data)                 \
	{                                                                      \
		if (vector->len + 1 == vector->capacity)                       \
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
