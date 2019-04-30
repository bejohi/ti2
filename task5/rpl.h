#ifndef RPL_H
#define RPL_H

#include <stdint.h>

enum item_type {
	TYPE_NUMBER,
	TYPE_STRING,
	TYPE_C_FUNC,
	TYPE_NUMOF // <-- last entry is used to get the number of types supported
};

// Forward declarations of data types to allow their use prior their definition
struct item;
struct rpl_context;

struct c_func {
	const char *name;
	int (*impl)(struct rpl_context *);
};

struct rpl_context {
	struct item *stack;
	struct item *free;
	const struct c_func **c_funcs;
	size_t stack_size;
};

struct item {
	struct item *next;
	// The value can only by of one type, thus they can share memory
	union {
		int64_t number;
		const char *str;
		const struct c_func *c_func;
	} value;
	enum item_type type;
};

extern const struct c_func rpl_add;
extern const struct c_func rpl_mul;

extern const struct c_func **c_funcs_default;

int rpl_parse(struct rpl_context *context, const char *str);
int rpl_eval(struct rpl_context *context);
int rpl_increase_stack(struct rpl_context *context);
void rpl_free_top(struct rpl_context *context);
struct item * rpl_get_operand(struct rpl_context *context);
struct item * rpl_push_result(struct rpl_context *context);
void rpl_print(struct item *);

static inline void rpl_clear_stack(struct rpl_context *context)
{
	while (context->stack) {
		rpl_free_top(context);
	}
}

static inline int rpl_needs_eval(struct item *i)
{
	return (i->type > TYPE_STRING);
}

#endif
