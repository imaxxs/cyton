#ifndef VERTEX_H
#define VERTEX_H

#include "../util/common.h"

typedef struct vertex_ctx {
	int vertex_type;
	int action_type;
	char *input_format;
	char *output_format;
	char *sense_func;
	char *act_func;
	char *callback_handler;
} vertex_ctx, *vertex_ctx_ptr;

extern FMStructDescRec *get_format_list(char *format);
extern void init_split_context(int num_recs, struct split_context *split_ctx);

#endif
