#ifndef CORTEX_H
#define CORTEX_H

#include "../util/common.h"

typedef struct sensor_rec {
	double timestamp;
	char *attr_name;
	float rec_val;
} sensor_rec, *sensor_rec_ptr;

typedef struct reconf_func_context  {
	char *func_name; 
	char *in_format; 
	char *out_format; 
} reconf_func_context, *reconf_func_context_ptr;

typedef struct reconf_func_recs  {
	int reconf_func_num; 
	struct reconf_func_context *reconf_func_recs; 
} reconf_func_recs, *reconf_func_recs_ptr;

typedef struct reconf_cyton_context  {
	struct split_context_list *reconf_split_recs; 
	struct func_context_list *reconf_func_recs; 
	struct stone_context_list *reconf_stone_recs; 
} reconf_cyton_context, *reconf_cyton_context_ptr;

typedef struct node_mongraph_reconf_rec {
	char *node_contact_list;
	int qid;
	int action_type; 
	int action; 
	struct reconf_cyton_context *reconf_in_ctx;
	struct reconf_cyton_context *reconf_out_ctx;
} node_mongraph_reconf_rec, *node_mongraph_reconf_rec_ptr;

typedef struct nodes_mongraph_reconf_recs {
	int node_mongraph_reconf_num; 
	struct node_mongraph_reconf_rec *node_mongraph_reconf_recs; 
} nodes_mongraph_reconf_recs, *nodes_mongraph_reconf_recs_ptr;

struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs = NULL;

extern FMStructDescRec *get_format_list(char *format); 
extern void init_split_context(int num_recs, struct split_context *split_ctx);
extern void init_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void init_func_context(int num_recs, struct func_context *func_ctx); 
extern void clean_split_context(int num_recs, struct split_context *split_ctx);
extern void clean_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void clean_func_context(int num_recs, struct func_context *func_ctx); 

#endif
