#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
extern "C" {
#include "fm.h"
#include "cod.h"
#include "evpath.h"
#include "revpath.h"
#include "gen_thread.h"
}

#define NUM_COORDS 1
#define NUM_AGGREGATORS 1
#define NUM_NODES 4 // Default 6 for load balancer, 6 for fault  and 4 for entropy - all rubis distributed 

#define NUM_QUERY 1 
#define NUM_FUNCS 6

#define NUM_MONS 10 // loadbalancer 10 , For load balancing and fault monitoring - 7 , entropy 6
#define MON_COPY 1 

/*typedef struct nodes_state_map {
	char *node_contact_list;
	char *node_coord_contact_list;
	char *node_aggr_contact_list;
	char *node_actu_contact_list;
	char *node_parent_contact_list;
	char* *node_child_contact_list;
} nodes_state_map, *nodes_state_map_ptr;

typedef struct context {
	struct cyton *cyctx;
	struct axon *axctx;
} context, *context_ptr;

struct context *ctx;*/

typedef struct func_stone {
	char *func_name;
	int stone_pcopy;
	int split_vcopy;
	int stone_id;
	int stone_action;
	int split_stone;
	int split_action;
} func_stone, func_stone_ptr;

typedef struct ret_node_coord_map {
	char* node_contact_list;
	char* node_coord_contact_list;
	char* node_aggr_contact_list;
	char* node_actu_contact_list;
	char* node_parent_contact_list;
	char* node_child_contact_list;
} ret_node_coord_map, *ret_node_coord_map_ptr;

typedef struct split_context {
	int split_stone;
	int split_action;
} split_context, *split_context_ptr;

typedef struct split_context_list {
	int split_ctx_num;
	struct split_context *split_ctx;
} split_context_list, *split_context_list_ptr;

typedef struct stone_context {
	int stone_pcopy;
	int split_vcopy;
	int stone_id;
	int stone_action;
	int split_stone;
	int split_action;
} stone_context, *stone_context_ptr;

typedef struct stone_context_list {
	int stone_ctx_num;
	struct stone_context *stone_ctx;
} stone_context_list, *stone_context_list_ptr;

typedef struct func_context {
	char *func_name;
	char *in_format;
	char *out_format;
	int func_order; // 1, 2, 3, 4 
	int func_level; // 0 for monitors, 1 for local streams and 2 for sense/coord vertices
} func_context, *func_context_ptr;

typedef struct func_context_list {
	int func_ctx_num;
	struct func_context *func_ctx;
} func_context_list, *func_context_list_ptr;

typedef struct mon_context {
	char *mon_key;
	char *mon_attr;
	char *mon_type;
	char *mon_resource_type; // 1, 2, 3, 4 
} mon_context, *mon_context_ptr;

typedef struct sample_context {
	int sample_rate_sec;
	int sample_rate_usec;
	int sample_window_width;
	int sample_window_unit; // 1 for sec, 0 for usec
} sample_context, *sample_context_ptr;

typedef struct metric_list {
	int num_recs;
	char **metric_recs;
} metric_list, *metric_list_ptr;

typedef struct func_rec {
	char *func_name;
	char *in_format;
	char *out_format;
	int flevel_start_point;
	int flevel_end_point;
	int func_splits;
	int func_type; // 0 for source, 1 for sink, 2 for filter, 3 for transform, 4 for router
	int func_order; // 1, 2, 3, 4 
	int func_level; // 0 for monitors, 1 for local streams and 2 for sense/coord vertices
} func_rec, *func_rec_ptr;

typedef struct control_comp {
	char *node_contact_list;
	char *node_coord_contact_list;
	char *node_aggr_contact_list;
	char *node_actu_contact_list;
} control_comp, *control_comp_ptr;

typedef struct deploy_nodes_context {
	int num_deploy_nodes;
	char **deploy_nodes_contact_list;
} deploy_nodes_context, *deploy_nodes_context_ptr;

typedef struct cyton {
	struct nodes_state_map *nomap;
} cyton, *cyton_ptr;

typedef struct axon {
	struct queries_nodes_map  *qnmap;
	struct monitors_nodes_map *mnmap; 
	struct coord_overlay_map *comap;
	struct query_node_states *qnstates; 
} axon, *axon_ptr;

#endif
