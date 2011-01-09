#ifndef NODE_AGENT_H
#define NODE_AGENT_H

#include "../util/common.h"

typedef struct reconf_split_context {
	int split_stone;
	int split_action;
} reconf_split_context, *reconf_split_context_ptr;

typedef struct reconf_split_recs {
	int reconf_split_num;
	struct reconf_split_context *reconf_splits;
} reconf_split_recs, *reconf_split_recs_ptr;

typedef struct reconf_stone_context {
	int stone_id;
	int stone_action;
} reconf_stone_context, *reconf_stone_context_ptr;

typedef struct reconf_stone_recs {
	int reconf_stone_num;
	struct reconf_stone_context *reconf_stones;
} reconf_stone_recs, *reconf_stone_recs_ptr;

typedef struct reconf_cyton_context  {
	struct reconf_split_recs *reconf_split_recs; 
	struct reconf_stone_recs *reconf_stone_recs; 
} reconf_cyton_context, *reconf_cyton_context_ptr;

typedef struct reconf_func_context  {
	char *func_name; 
	char *in_format; 
	char *out_format; 
	int func_pcopies;
	int func_vsplits;
} reconf_func_context, *reconf_func_context_ptr;

typedef struct reconf_func_recs  {
	int reconf_func_num; 
	struct reconf_func_context *reconf_funcs; 
} reconf_func_recs, *reconf_func_recs_ptr;

typedef struct node_mongraph_reconf_rec {
	char *node_contact_list;
	int qid;
	int action_type; 
	int action; 
	struct reconf_func_recs *reconf_recs;
	struct reconf_cyton_context *reconf_in_ctx;
	struct reconf_cyton_context *reconf_out_ctx;
} node_mongraph_reconf_rec, *node_mongraph_reconf_rec_ptr;

typedef struct nodes_mongraph_reconf_recs {
	int node_mongraph_reconf_num; 
	struct node_mongraph_reconf_rec *node_mongraph_reconf_recs; 
} nodes_mongraph_reconf_recs, *nodes_mongraph_reconf_recs_ptr;

typedef struct context {
	struct cyton *cyctx;
	struct axon *axctx;
} context, *context_ptr;

typedef struct local_exec_act {
	int exec_id;
	char *exec_func;
} local_exec_act, *local_exec_act_ptr;

typedef struct func_list {
	int num_recs;
	struct func_rec *func_recs;
} func_list, *func_list_ptr;

typedef struct node_boot {
	int is_control;
	char *bootserver_contact_list;
	char *node_contact_list;
} node_boot, *node_boot_ptr;

typedef struct nodes_state_map {
	char *node_contact_list;
	char *node_coord_contact_list;
	char *node_aggr_contact_list;
	char *node_actu_contact_list;
	char *node_parent_contact_list;
	char* *node_child_contact_list;
} nodes_state_map, *nodes_state_map_ptr;

typedef struct node_monitors_deploy {
	char *node_contact_list;
	int node_type;
	struct stone_context *stone_endpoints;
	struct func_context *func_endpoints;
	struct mon_context *mon_endpoints;
} node_monitors_deploy, *node_monitors_deploy_ptr;

typedef struct node_graph_deploy {
	int graph_id;
	int is_graph_deployed;
	char *node_contact_list;
	int node_type;
	struct stone_context_list *stone_endpoints;
	struct func_context_list *func_endpoints;
} node_graph_deploy, *node_graph_deploy_ptr;

typedef struct node_deploy {
	char *node_contact_list;
	char *node_coord_contact_list;
	int node_type; // Node type 0 = normal node, 1 = aggregator and 2 = coordinator
	int query_id;
	int graph_id;
	int num_recs;
	struct func_rec *func_recs;
} node_deploy, *node_deploy_ptr;

struct cyton *cyctx;
struct axon *axctx;
struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs;

CManager cm;
CMFormat boot_in_format;
CMFormat boot_out_format;
CMFormat actuate_format;
CMFormat node_info_format;
CMFormat live_info_format;
CMFormat control_info_format;
CMFormat node_monitors_deploy_format;
CMFormat node_graph_deploy_format;
CMFormat node_deploy_format;
CMFormat local_agent_term_format;
CMFormat request_trace_format;
CMFormat extern_format;

extern struct cyton *create_cyton_context();
extern struct axon *create_axon_context();

extern int check_is_node_aggregator(struct cyton *cyctx, char *node_contact_list); 
extern int check_if_nodes_deployed_count(struct axon *axctx, int num_to_deploy);
extern int check_is_node_coord(struct cyton *cyctx, char *node_contact_list); 
extern int check_is_node_coord_assigned(struct cyton *cyctx, char *node_contact_list); 
extern int check_is_node_control_comps_assigned(struct cyton *cyctx, char *node_contact_list); 
extern void add_coord_node_child_state(struct axon *axctx, struct node_boot *node_live_info);
extern void update_local_node_state_from_boot(struct cyton *cyctx, struct ret_node_coord_map *bootserver_msg);
extern void update_local_node_state_from_coord(struct cyton *cyctx, struct control_comp * ctrl_cmp);
extern void init_coord_node_state(struct cyton *cyctx, struct axon *axctx);

extern void create_monitors_stream(CManager cm, struct context *ctx, char *mon_type, char *resource_type, char *mon_metric, int metric_num,
char *node_contact_list, char *node_coord_contact_list, struct stone_context *stone_ctx, struct func_context *func_ctx, struct mon_context *mon_ctx, 
attr_list mon_metric_attr_list); 
extern void register_monitors_to_axon(struct axon *axctx, char *node_contact_list, char *key, char *mon_attr, EVstone split_stone, 
EVaction split_action, char *out_format);

extern void update_coord_graph_deployed_state(struct axon *axctx, struct node_graph_deploy *node_graph_deploy_data);
extern void create_aggregator_streams_network(CManager cm, struct axon *axctx, struct query_ctx *qctx);

extern int *realize_vertex(CManager cm, int qid, struct func_rec frec, int pcopies, EVSimpleHandlerFunc handler, char *handler_data, 
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list, struct split_context_list *split_ctx_list); 
extern void realize_edge(CManager cm, int qid, int pcopies, struct split_context_list *split_ctx_list, int *end_stone);

extern void register_nodes_endpoints_for_query(CManager cm, struct axon *axctx, char *node_contact_list, char *coord_contact_list, int qid, 
int num_recs, struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list);
extern void register_aggregator_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, int num_recs, 
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list); 

extern void register_nodes_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_nodes, char *deploy_contact_list, int qid);
extern void register_aggregator_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_aggregators, char *deploy_contact_list, 
int qid); 

extern void deploy_monitoring_graph(CManager cm, struct axon *axctx, char *src_contact_list, char *coord_contact_list, int qid, int qtype, 
int graph_id, struct metric_list *metric_list_ptr, int num_funcs, struct func_rec *func_recs);

void set_qnode_state(struct axon *axctx, char *node_contact_list, int qid, char *func_name, int pcopies, int vcopies, int stone_id, int stone_action, 
int split_stone, int split_action); 
extern void get_qnode_state(struct axon *axctx, char *node_contact_list, int qid, char *func_name, int stone_ctx_num, struct stone_context *stone_ctx);
extern FMStructDescRec *get_format_list(char *format); 
extern void add_metric(CManager cm, struct nodes_mongraph_reconf_rec *nmon_greconf_rec_ptr);
extern void get_deployable_nodes_from_coord(struct axon *axctx, char *coord_contact_list, struct deploy_nodes_context *deploy_nodes_ctx);
extern int check_is_constraint_violated(struct axon *axctx);

extern char *get_cpu_nice_metrics_ECL;
extern char *get_cpu_user_metrics_ECL;
extern char *get_cpu_system_metrics_ECL;
extern char *get_cpu_idle_metrics_ECL;
extern char *get_mem_buffers_metrics_ECL;
extern char *get_mem_free_metrics_ECL;
extern char *get_mem_cached_metrics_ECL;
extern char *get_swap_free_metrics_ECL;
extern char *get_load_one_metrics_ECL;
extern char *get_load_five_metrics_ECL;
extern char *get_load_fifteen_metrics_ECL;
extern char *get_cpu_min_freq_metrics_ECL;
extern char *get_cpu_max_freq_metrics_ECL;
extern char *get_cpu_cur_freq_metrics_ECL;

extern void init_split_context(int num_recs, struct split_context *split_ctx);
extern void init_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void init_func_context(int num_recs, struct func_context *func_ctx); 
extern void init_mon_context(int num_recs, struct mon_context *mon_ctx); 
extern void init_sample_context(int num_recs, struct sample_context *sample_ctx); 
extern void init_stone_ids(int num_recs, int *stone_ids); 
extern void clean_split_context(int num_recs, struct split_context *split_ctx);
extern void clean_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void clean_func_context(int num_recs, struct func_context *func_ctx); 
extern void clean_mon_context(int num_recs, struct mon_context *mon_ctx); 
extern void clean_sample_context(int num_recs, struct sample_context *sample_ctx); 
extern void clean_stone_ids(int num_recs, int *stone_ids); 
extern "C" {
extern int libvirt_init();
extern int libvirt_open();
extern int libvirt_close();
extern int libvirt_num_domains();
extern int libvirt_num_active_domains();
extern int libvirt_num_inactive_domains();
extern struct domain_info* libvirt_get_domain_info(char *domain_name);
extern struct domain_info* libvirt_get_domain_disk_info(char *domain_name);
extern struct domain_info* libvirt_get_domain_interface_info(char *domain_name); 
extern int libvirt_domain_exists();
extern int libvirt_get_domain_id(char *domain_name);
extern unsigned char libvirt_get_domain_state(char *domain_name);
extern unsigned long libvirt_get_domain_mem_max(char *domain_name); 
extern unsigned long libvirt_get_domain_mem_used(char *domain_name);
extern unsigned short libvirt_get_domain_num_vcpus(char *domain_name);
extern unsigned long libvirt_get_domain_cpu_total_time(char *domain_name);
extern unsigned long libvirt_get_domain_vblock_rreq(char *domain_name);
extern unsigned long libvirt_get_domain_vblock_wreq(char *domain_name);
extern unsigned long libvirt_get_domain_vblock_rbytes(char *domain_name);
extern unsigned long libvirt_get_domain_vblock_wbytes(char *domain_name); 
extern unsigned long libvirt_get_domain_if_rxbytes(char *domain_name);
extern unsigned long libvirt_get_domain_if_txbytes(char *domain_name);
extern unsigned long libvirt_get_domain_if_rxpackets(char *domain_name);
extern unsigned long libvirt_get_domain_if_txpackets(char *domain_name);
extern unsigned long libvirt_get_domain_if_rxerrors(char *domain_name);
extern unsigned long libvirt_get_domain_if_txerrors(char *domain_name);
extern unsigned long libvirt_get_domain_if_rxdrops(char *domain_name);
extern unsigned long libvirt_get_domain_if_txdrops(char *domain_name);
extern int libvirt_set_vcpus(char *domain_name, int num_vcpus);
extern int libvirt_set_memory(char *domain_name, unsigned long mem_kbs);
extern int libvirt_create_domain(char *domain_xml);
extern int libvirt_shutdown_domain(char *domain_name);
extern int libcurl_init(char *url);
}

#endif
