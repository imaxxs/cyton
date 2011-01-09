#ifndef PLANNER_H
#define PLANNER_H

#include "../util/common.h"

typedef struct func_list {
	int num_recs;
	struct func_rec *func_recs;
} func_list, *func_list_ptr;

typedef struct node_deploy {
	char *node_contact_list;
	char *node_coord_contact_list;
	int node_type; // Node type 0 = normal node, 1 = aggregator and 2 = coordinator
	int query_id;
	int graph_id;
	int num_recs;
	struct func_rec *func_recs;
} node_deploy, *node_deploy_ptr;

typedef struct query_ctx {
	int qid;
	int qtype;
	int graph_id; // Field not been used to be removed shortly
	char *src_contact_list;
	char *coord_contact_list;
	struct metric_list *metric_list;
	struct func_list *func_list;
	struct axon *axctx;	
	CManager cm;
} query_ctx, *query_ctx_ptr;

typedef struct local_exec_act {
	int exec_id;
	char *exec_func;
} local_exec_act, *local_exec_act_ptr;

typedef struct coord_overlay_map {
	char* node_contact_list;
	char* node_coord_contact_list;
	char* node_parent_contact_list;
	char* node_aggr_contact_list;
	char* node_actu_contact_list;
	char** node_child_contact_list;
} coord_overlay_map, *coord_overlay_map_ptr;

struct axon *paxctx;

CMFormat planner_node_deploy_format;
CMFormat per_node_deploy_format;
CMFormat local_term_format;

extern void connect_monitors_nodes_funcs_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, 
struct metric_list *metric_list);
extern void connect_nodes_aggregator_funcs_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid); 

extern int check_coord_is_query_graph_deployed(struct axon *axctx, int qid, int graph_id, int total_deploy_aggregators, int total_deploy_nodes);
extern int check_coord_is_aggregator_graph_deployed_for_query(struct axon *axctx, int num_deploy, char *deploy_contact_list, int qid, int graph_id);
extern int check_coord_is_nodes_graph_deployed_for_query(struct axon *axctx, int num_deploy, char *deploy_contact_list, int qid, int graph_id);

extern void register_aggregator_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, int num_recs, 
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list);

extern void clean_deploy_nodes_context(int num_recs, struct deploy_nodes_context *deploy_nodes_ctx); 
extern void get_deployable_nodes_from_coord(struct axon *axctx, char *coord_contact_list, struct deploy_nodes_context *deploy_nodes_ctx);
extern void get_deployable_aggregators_from_coord(struct axon *axctx, char *coord_contact_list, struct deploy_nodes_context *deploy_aggregators_ctx);
extern struct control_comp *get_coord_control_comps(struct axon *axctx, char *coord_contact_list);
extern int check_is_coord_control_comps_assigned(struct axon *axctx, char *coord_contact_list);
extern int check_is_node_coord_assigned(struct cyton *cyctx, char *coord_contact_list);
extern int check_is_node_coord(struct axon *axctx, char *coord_contact_list, char *node_contact_list);
extern int check_is_aggregator_coord(struct axon *axctx, int num_deploy_aggregators, char **deploy_aggregators_contact_list, char *coord_contact_list);

extern int *realize_vertex(CManager cm, int qid, struct func_rec frec, int pcopies, EVSimpleHandlerFunc handler, char *handler_data, struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list, struct split_context_list *split_ctx_list); 
extern void realize_edge(CManager cm, int qid, int pcopies, struct split_context_list *split_ctx_list, int *end_stone);

extern void register_coord_endpoints_to_axon(CManager cm, struct cyton *cyctx, int count, int graph_id, struct func_rec func_recs);
extern void register_graph_endpoints_to_axon(CManager cm, struct cyton *cyctx, int count, int graph_id, struct func_rec func_recs);
extern void register_query_deployed_to_axon(CManager cm, struct axon *axctx, int qid, int graph_id);

extern void register_nodes_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_nodes, char *deploy_contact_list, int qid);
extern void register_aggregator_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_aggregators, char *deploy_contact_list, 
int qid); 

extern void register_nodes_for_query(struct axon *axctx, int qid, char *coord_contact_list, int num_deploy_nodes, char **deploy_nodes_contact_list, 
int is_query_deployed, int is_query_running);

extern void register_aggregators_for_query(struct axon *axctx, int qid, char *coord_cotanct_list, int num_deploy_aggregators, 
char **deploy_aggregators_contact_list, int is_query_deployed, int is_query_running);

extern void register_queries_to_axon(CManager cm, struct axon *axctx, char *src_contact_list, char *coord_contact_list, int qid, int qtype, 
int is_query_deployed, int is_query_on, int query_graph_id, int num_funcs, struct func_rec *func_recs);

extern void init_stone_ids(int num_recs, int *stone_ids); 
extern void init_split_context(int num_recs, struct split_context *split_ctx);
extern void init_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void init_func_context(int num_recs, struct func_context *func_ctx); 
extern void clean_split_context(int num_recs, struct split_context *split_ctx);
extern void clean_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void clean_func_context(int num_recs, struct func_context *func_ctx); 
extern void clean_stone_ids(int num_recs, int *stone_ids); 

#endif
