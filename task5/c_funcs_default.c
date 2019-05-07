#include <stddef.h>
#include "rpl.h"

static const struct c_func *tmp[] = {&rpl_add, &rpl_mul, &rpl_sub, &rpl_div, &rpl_pow,NULL};

const struct c_func **c_funcs_default = tmp;
