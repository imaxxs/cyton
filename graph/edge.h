#ifndef EDGE_H
#define EDGE_H

#include "../util/common.h"

typedef struct edge_ctx {
	int type; //intra and inter node link
	int start_stone;
	int end_stone;
	int split_action;
} edge_ctx, *edge_ctx_ptr;

#endif
