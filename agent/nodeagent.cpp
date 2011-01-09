#include "nodeagent.h"

struct context *ctx;

static FMField in_field_list[] = 
{
	{"is_control", "integer", sizeof(int), FMOffset(node_boot_ptr, is_control)},
	{"bootserver_contact_list", "string", sizeof(char*), FMOffset(node_boot_ptr, bootserver_contact_list)},
	{"node_contact_list", "string", sizeof(char*), FMOffset(node_boot_ptr, node_contact_list)},
	{NULL, NULL, 0, 0}
};

static FMField out_field_list[] = 
{
	{"node_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_contact_list)},
	{"node_coord_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_coord_contact_list)},
	{"node_aggr_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_aggr_contact_list)},
	{"node_actu_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_actu_contact_list)},
	{"node_parent_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_parent_contact_list)},
	{"node_child_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_child_contact_list)},
	{NULL, NULL, 0, 0}
};

static FMField control_field_list[] = 
{
	{"node_contact_list", "string", sizeof(char*), FMOffset(control_comp_ptr, node_contact_list)},
	{"node_coord_contact_list", "string", sizeof(char*), FMOffset(control_comp_ptr, node_coord_contact_list)},
	{"node_aggr_contact_list", "string", sizeof(char*), FMOffset(control_comp_ptr, node_aggr_contact_list)},
	{"node_actu_contact_list", "string", sizeof(char*), FMOffset(control_comp_ptr, node_actu_contact_list)},
	{NULL, NULL, 0, 0}
};

static FMField stone_endpoint_field_list[] = 
{
	{"stone_id", "integer", sizeof(int), FMOffset(stone_context_ptr, stone_id)},
	{"stone_action", "integer", sizeof(int), FMOffset(stone_context_ptr, stone_action)},
	{"split_stone", "integer", sizeof(int), FMOffset(stone_context_ptr, split_stone)},
	{"split_action", "integer", sizeof(int), FMOffset(stone_context_ptr, split_action)},
	{NULL, NULL, 0, 0}
};

static FMField stone_context_list_field_list[] = 
{
	{"stone_ctx_num", "integer", sizeof(int), FMOffset(stone_context_list_ptr, stone_ctx_num)},
	{"stone_ctx", "stone_ctx[stone_ctx_num]", sizeof(struct stone_context), FMOffset(stone_context_list_ptr, stone_ctx)},
	{NULL, NULL, 0, 0}
};

static FMField func_endpoint_field_list[] = 
{
	{"func_name", "string", sizeof(char*), FMOffset(func_context_ptr, func_name)},
	{"in_format", "string", sizeof(char*), FMOffset(func_context_ptr, in_format)},
	{"out_format", "string", sizeof(char*), FMOffset(func_context_ptr, out_format)},
	{"func_order", "integer", sizeof(int), FMOffset(func_context_ptr, func_order)},
	{"func_level", "integer", sizeof(int), FMOffset(func_context_ptr, func_level)},
	{NULL, NULL, 0, 0}
};

static FMField func_context_list_field_list[] = 
{
	{"func_ctx_num", "integer", sizeof(int), FMOffset(func_context_list_ptr, func_ctx_num)},
	{"func_ctx", "func_ctx[func_ctx_num]", sizeof(struct func_context), FMOffset(func_context_list_ptr, func_ctx)},
	{NULL, NULL, 0, 0}
};

static FMField mon_endpoint_field_list[] = 
{
	{"mon_key", "string", sizeof(char*), FMOffset(mon_context_ptr, mon_key)},
	{"mon_attr", "string", sizeof(char*), FMOffset(mon_context_ptr, mon_attr)},
	{"mon_type", "string", sizeof(char*), FMOffset(mon_context_ptr, mon_type)},
	{"mon_resource_type", "string", sizeof(char*), FMOffset(mon_context_ptr, mon_resource_type)},
	{NULL, NULL, 0, 0}
};

static FMField node_graph_deploy_field_list[] = 
{
	{"graph_id", "integer", sizeof(int), FMOffset(node_graph_deploy_ptr, graph_id)},
	{"is_graph_deployed", "integer", sizeof(int), FMOffset(node_graph_deploy_ptr, is_graph_deployed)},
	{"node_contact_list", "string", sizeof(char*), FMOffset(node_graph_deploy_ptr, node_contact_list)},
	{"node_type", "integer", sizeof(int), FMOffset(node_graph_deploy_ptr, node_type)},
	{"stone_endpoints", "*(stone_context_list[2])", sizeof(struct stone_context_list), FMOffset(node_graph_deploy_ptr, stone_endpoints)},
	{"func_endpoints", "*(func_context_list[2])", sizeof(struct func_context_list), FMOffset(node_graph_deploy_ptr, func_endpoints)},
	{NULL, NULL, 0, 0}
};

static FMField node_monitors_deploy_field_list[] = 
{
	{"node_contact_list", "string", sizeof(char*), FMOffset(node_monitors_deploy_ptr, node_contact_list)},
	{"node_type", "integer", sizeof(int), FMOffset(node_monitors_deploy_ptr, node_type)},
	{"stone_endpoints", "*(stone_endpoints[1])", sizeof(struct stone_context), FMOffset(node_monitors_deploy_ptr, stone_endpoints)},
	{"func_endpoints", "*(func_endpoints[1])", sizeof(struct func_context), FMOffset(node_monitors_deploy_ptr, func_endpoints)},
	{"mon_endpoints", "*(mon_endpoints[1])", sizeof(struct mon_context), FMOffset(node_monitors_deploy_ptr, mon_endpoints)},
	{NULL, NULL, 0, 0}
};

static FMField func_field_list[] = 
{
	{"func_name", "string", sizeof(char*), FMOffset(func_rec_ptr, func_name)},
	{"in_format", "string", sizeof(char*), FMOffset(func_rec_ptr, in_format)},
	{"out_format", "string", sizeof(char*), FMOffset(func_rec_ptr, out_format)},
	{"flevel_start_point", "integer", sizeof(int), FMOffset(func_rec_ptr, flevel_start_point)},
	{"flevel_end_point", "integer", sizeof(int), FMOffset(func_rec_ptr, flevel_end_point)},
	{"func_splits", "integer", sizeof(int), FMOffset(func_rec_ptr, func_splits)},
	{"func_type", "integer", sizeof(int), FMOffset(func_rec_ptr, func_type)},
	{"func_order", "integer", sizeof(int), FMOffset(func_rec_ptr, func_order)},
	{"func_level", "integer", sizeof(int), FMOffset(func_rec_ptr, func_level)},
	{NULL, NULL, 0, 0}
};

static FMField local_term_field_list[] = 
{
	{"exec_id", "integer", sizeof(int), FMOffset(local_exec_act_ptr, exec_id)},
	{"exec_func", "string", sizeof(char*), FMOffset(local_exec_act_ptr, exec_func)},
	{NULL, NULL, 0, 0}
};

static FMField node_deploy_field_list[] = 
{
	{"node_contact_list", "string", sizeof(char*), FMOffset(node_deploy_ptr, node_contact_list)},
	{"node_coord_contact_list", "string", sizeof(char*), FMOffset(node_deploy_ptr, node_coord_contact_list)},
	{"node_type", "integer", sizeof(int), FMOffset(node_deploy_ptr, node_type)},
	{"query_id", "integer", sizeof(int), FMOffset(node_deploy_ptr, query_id)},
	{"graph_id", "integer", sizeof(int), FMOffset(node_deploy_ptr, graph_id)},
	{"num_recs", "integer", sizeof(int), FMOffset(node_deploy_ptr, num_recs)},
	{"func_recs", "func_recs[num_recs]", sizeof(struct func_rec), FMOffset(node_deploy_ptr, func_recs)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec in_format_list[] = 
{
    {"in_data", in_field_list, sizeof(node_boot), NULL},
    {NULL, NULL}
};

static FMStructDescRec live_info_format_list[] = 
{
    {"live_info_data", in_field_list, sizeof(node_boot), NULL},
    {NULL, NULL}
};

static FMStructDescRec node_info_format_list[] = 
{
    {"node_info_data", in_field_list, sizeof(node_boot), NULL},
    {NULL, NULL}
};

static FMStructDescRec out_format_list[] =
{
    {"out_data", out_field_list, sizeof(ret_node_coord_map), NULL},
    {NULL, NULL}
};

static FMStructDescRec control_format_list[] = 
{
    {"control_data", control_field_list, sizeof(control_comp), NULL},
    {NULL, NULL}
};

static FMStructDescRec local_term_format_list[] = 
{
    {"local_exec", local_term_field_list, sizeof(local_exec_act), NULL},
    {NULL, NULL}
};

static FMStructDescRec node_monitors_deploy_format_list[] = 
{
    {"node_monitors_deploy_data", node_monitors_deploy_field_list, sizeof(node_monitors_deploy), NULL},
    {"stone_endpoints", stone_endpoint_field_list, sizeof(stone_context), NULL},
    {"func_endpoints", func_endpoint_field_list, sizeof(func_context), NULL},
    {"mon_endpoints", mon_endpoint_field_list, sizeof(mon_context), NULL},
    {NULL, NULL}
};

static FMStructDescRec node_graph_deploy_format_list[] = 
{
    {"node_graph_deploy", node_graph_deploy_field_list, sizeof(node_graph_deploy), NULL},
    {"stone_context_list", stone_context_list_field_list, sizeof(stone_context_list), NULL},
    {"func_context_list", func_context_list_field_list, sizeof(func_context_list), NULL},
    {"stone_ctx", stone_endpoint_field_list, sizeof(stone_context), NULL},
    {"func_ctx", func_endpoint_field_list, sizeof(func_context), NULL},
    {NULL, NULL}
};

static FMStructDescRec node_deploy_format_list[] =
{
    {"node_deploy_data", node_deploy_field_list, sizeof(node_deploy), NULL},
    {"func_recs", func_field_list, sizeof(func_rec), NULL},
    {NULL, NULL}
};

typedef struct request_trace {
	char *server_timestamp;
    char *request_local_url;
	char *request_params;
    char *request_time;
	char *client_ip;
    char *server_ip;
    char *server_contactlist;
    int request_trace_output_stone;
    int request_trace_split_stone;
    int request_trace_split_action;
} request_trace, *request_trace_ptr;

static FMField request_trace_field_list[] = 
{
	{"server_timestamp", "string", sizeof(char*), FMOffset(request_trace_ptr, server_timestamp)},
	{"request_local_url", "string", sizeof(char*), FMOffset(request_trace_ptr, request_local_url)},
	{"request_params", "string", sizeof(char*), FMOffset(request_trace_ptr, request_params)},
	{"request_time", "string", sizeof(char*), FMOffset(request_trace_ptr, request_time)},
	{"client_ip", "string", sizeof(char*), FMOffset(request_trace_ptr, client_ip)},
	{"server_ip", "string", sizeof(char*), FMOffset(request_trace_ptr, server_ip)},
	{"server_contactlist", "string", sizeof(char*), FMOffset(request_trace_ptr, server_contactlist)},
	{"request_trace_output_stone", "integer", sizeof(int), FMOffset(request_trace_ptr, request_trace_output_stone)},
	{"request_trace_split_stone", "integer", sizeof(int), FMOffset(request_trace_ptr, request_trace_split_stone)},
	{"request_trace_split_action", "integer", sizeof(int), FMOffset(request_trace_ptr, request_trace_split_action)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec request_trace_format_list[] = 
{
	{"request_trace", request_trace_field_list, sizeof(request_trace), NULL},
	{NULL, NULL}
};

void populate_entropy_query_funcs(struct func_rec *func_recs) {
	func_recs[7].func_name = "dll:./../functions/libfunctions.la:rec_window";
	func_recs[7].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[7].func_order = 1;
	func_recs[7].func_level = 1;
	func_recs[7].flevel_start_point = 1;
	func_recs[7].in_format = "sensor";
	func_recs[7].out_format = "recs";

	func_recs[6].func_name = "dll:./../functions/libfunctions.la:append_range";
	func_recs[6].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[6].func_order = 1;
	func_recs[6].func_level = 1;
	func_recs[6].in_format = "recs";
	func_recs[6].out_format = "recs_range";

	func_recs[5].func_name = "dll:./../functions/libfunctions.la:append_level";
	func_recs[5].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[5].func_order = 2;
	func_recs[5].func_level = 1;
	func_recs[5].in_format = "recs_range";
	func_recs[5].out_format = "recs_range_level";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:recs_range_levels";
	func_recs[4].func_type = 3; // Transform - Takes array of last 120 sec (cpu util) records and bin records in userdefined range levels
	func_recs[4].func_order = 3;
	func_recs[4].func_level = 1;
	func_recs[4].in_format = "recs_range_level";
	func_recs[4].out_format = "recs_range_level_list";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:recs_range_stats_list";
	func_recs[3].func_type = 3; // Transform - Takes array of last 120 sec (cpu util) records and binned userdefined range levels and calculate count 
	func_recs[3].func_order = 4;
	func_recs[3].func_level = 1;
	func_recs[3].flevel_end_point = 1;
	func_recs[3].in_format = "recs_range_level_list";
	func_recs[3].out_format = "recs_stats_range_list";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:entropy_stats";
	func_recs[2].func_type = 3; // Transform - Takes array of last 120 sec (cpu util) records and binned userdefined range levels and calculate count 
	func_recs[2].func_order = 1;
	func_recs[2].func_level = 2;
	func_recs[2].flevel_start_point = 1;
	func_recs[2].in_format = "recs_stats_range_list";
	func_recs[2].out_format = "recs_entropy_stats";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:append_entropy_stats_threshold";
	func_recs[1].func_type = 3; // Transform - Adds userdefined threshold to the input data
	func_recs[1].func_order = 2;
	func_recs[1].func_level = 2;
	func_recs[1].in_format = "recs_entropy_stats";
	func_recs[1].out_format = "recs_entropy_stats_threshold";

	func_recs[0].func_name = "dll:./../functions/libfunctions.la:entropy_stats_threshold_trigger";
	func_recs[0].func_type = 4; // Router - Informs the coordinator that threshold violation occured as event
	func_recs[0].func_order = 3;
	func_recs[0].func_level = 2;
	func_recs[0].flevel_end_point = 1;
	func_recs[0].in_format = "recs_entropy_stats_threshold";
	func_recs[0].out_format = "recs_entropy_stats_threshold";
}

void populate_range_query_funcs(struct func_rec *func_recs) {
	func_recs[6].func_name = "dll:./../functions/libfunctions.la:rec_window";
	func_recs[6].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[6].func_order = 1;
	func_recs[6].func_level = 1;
	func_recs[6].flevel_start_point = 1;
	func_recs[6].in_format = "sensor";
	func_recs[6].out_format = "recs";

	func_recs[5].func_name = "dll:./../functions/libfunctions.la:append_range";
	func_recs[5].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[5].func_order = 2;
	func_recs[5].func_level = 1;
	func_recs[5].in_format = "recs";
	func_recs[5].out_format = "recs_range";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:append_level";
	func_recs[4].func_type = 3;  // Transform - Adds userdefined range to array of last 120 sec (cpu util) records
	func_recs[4].func_order = 3;
	func_recs[4].func_level = 1;
	func_recs[4].in_format = "recs_range";
	func_recs[4].out_format = "recs_range_level";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:recs_range_levels";
	func_recs[3].func_type = 3; // Transform - Takes array of last 120 sec (cpu util) records and bin records in userdefined range levels
	func_recs[3].func_order = 4;
	func_recs[3].func_level = 1;
	func_recs[3].in_format = "recs_range_level";
	func_recs[3].out_format = "recs_range_level_list";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:recs_range_stats_list";
	func_recs[2].func_type = 3; // Transform - Takes array of last 120 sec (cpu util) records and binned userdefined range levels and calculate count 
	func_recs[2].func_order = 5;
	func_recs[2].func_level = 1;
	func_recs[2].flevel_end_point = 1;
	func_recs[2].in_format = "recs_range_level_list";
	func_recs[2].out_format = "recs_stats_range_list";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:append_recs_stats_threshold";
	func_recs[1].func_type = 3; // Transform - Adds userdefined threshold to the input data
	func_recs[1].func_order = 1;
	func_recs[1].func_level = 2;
	func_recs[1].flevel_start_point = 1;
	func_recs[1].in_format = "recs_stats_range_list";
	func_recs[1].out_format = "recs_stats_threshold_range_list";

	func_recs[0].func_name = "dll:./../functions/libfunctions.la:recs_stats_threshold_trigger";
	func_recs[0].func_type = 4; // Router - Informs the coordinator that threshold violation occured as event
	func_recs[0].func_order = 2;
	func_recs[0].func_level = 2;
	func_recs[0].flevel_end_point = 1;
	func_recs[0].in_format = "recs_stats_threshold_range_list";
	func_recs[0].out_format = "recs_stats_threshold_range_list";
}

void populate_complex_range_query_funcs(struct func_rec *func_recs) {
	func_recs[8].func_name = "dll:./../functions/libfunctions.la:rec_group_map";
	func_recs[8].func_type = 4;  // Routing - Extracts information from monitors stone about their group and maps with its group_type attr
	func_recs[8].func_order = 1;
	func_recs[8].func_level = 1;
	func_recs[8].flevel_start_point = 1;
	func_recs[8].in_format = "sensor";
	func_recs[8].out_format = "sensor";

	func_recs[7].func_name = "dll:./../functions/libfunctions.la:rec_event_window";
	func_recs[7].func_type = 5;  // Multiqueue - Creates an event window queue of size n = number of events
	func_recs[7].func_order = 2;
	func_recs[7].func_level = 1;
	func_recs[7].in_format = "sensor";
	func_recs[7].out_format = "recs";

	func_recs[6].func_name = "dll:./../functions/libfunctions.la:rec_time_window";
	func_recs[6].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed
	func_recs[6].func_order = 2;
	func_recs[6].func_level = 1;
	func_recs[6].in_format = "sensor";
	func_recs[6].out_format = "recs";

	func_recs[5].func_name = "dll:./../functions/libfunctions.la:event_occurence_order";
	func_recs[5].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, where events are occurence ordered
	func_recs[5].func_order = 3;
	func_recs[5].func_level = 1;
	func_recs[5].in_format = "recs";
	func_recs[5].out_format = "recs";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:event_type";
	func_recs[4].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, where events are type ordered
	func_recs[4].func_order = 3;
	func_recs[4].func_level = 1;
	func_recs[4].in_format = "recs";
	func_recs[4].out_format = "recs";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:counting_sample";
	func_recs[3].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, keeps counts for all occurence of events
	func_recs[3].func_order = 1;
	func_recs[3].func_level = 1;
	func_recs[3].flevel_start_point = 2;
	func_recs[4].func_splits = 3;
	func_recs[3].in_format = "recs";
	func_recs[3].out_format = "recs";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:concise_sample";
	func_recs[2].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, where similar events are pruned
	func_recs[2].func_order = 1;
	func_recs[2].func_level = 1;
	func_recs[2].flevel_start_point = 2;
	func_recs[4].func_splits = 3;
	func_recs[2].in_format = "recs";
	func_recs[2].out_format = "recs";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:counting_sample_trigger";
	func_recs[1].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, where trigger enables new mon 
	func_recs[1].func_order = 1;
	func_recs[1].func_level = 1;
	func_recs[1].flevel_start_point = 2;
	func_recs[4].func_splits = 4;
	func_recs[1].in_format = "recs";
	func_recs[1].out_format = "recs";

	func_recs[0].func_name = "dll:./../functions/libfunctions.la:concise_sample_trigger";
	func_recs[0].func_type = 5;  // Multiqueue - Creates an time window queue of length n = time elapsed, where trigger deploys ctrl-loop
	func_recs[0].func_order = 1;
	func_recs[0].func_level = 1;
	func_recs[0].flevel_start_point = 2;
	func_recs[4].func_splits = 4;
	func_recs[0].in_format = "recs";
	func_recs[0].out_format = "recs";
}

void populate_multi_range_query_funcs(struct func_rec *func_recs) {
	func_recs[4].func_name = "dll:./../functions/libfunctions.la:rec_group_map";
	func_recs[4].func_type = 4;  // Routing - Extracts information from monitors stone about their group and maps with its group_type attr
	func_recs[4].func_order = 1;
	func_recs[4].func_level = 1;
	func_recs[4].func_splits = 3;
	func_recs[4].flevel_start_point = 1;
	func_recs[4].in_format = "sensor";
	func_recs[4].out_format = "sensor";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:rec_event_window";
	func_recs[3].func_type = 3;  // Transform - Creates an event window queue of size n = number of events
	func_recs[3].func_order = 2;
	func_recs[3].func_level = 1;
	func_recs[3].func_splits = 1;
	func_recs[3].in_format = "sensor";
	func_recs[3].out_format = "sensor_recs";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:rec_event_occurence_order";
	func_recs[2].func_type = 3;  // Transform - Creates an time window queue of length n = time elapsed, where events are occurence ordered
	func_recs[2].func_order = 3;
	func_recs[2].func_level = 1;
	func_recs[2].func_splits = 1;
	func_recs[2].in_format = "sensor_recs";
	func_recs[2].out_format = "sensor_recs";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:rec_event_counting_sample";
	func_recs[1].func_type = 3;  // Transform - Creates an time window queue of length n = time elapsed, keeps counts for all occurence of events
	func_recs[1].func_order = 4;
	func_recs[1].func_level = 1;
	func_recs[1].func_splits = 1;
	func_recs[1].flevel_end_point = 1;
	func_recs[1].in_format = "sensor_recs";
	func_recs[1].out_format = "count_sample_recs";

	func_recs[0].func_name = "dll:./../functions/libfunctions.la:counting_sample_decision";
	func_recs[0].func_type = 4;  // Routing - Creates an time window queue of length n = time elapsed, where trigger enables new mon 
	func_recs[0].func_order = 1;
	func_recs[0].func_level = 2;
	func_recs[0].func_splits = 2;
	func_recs[0].flevel_start_point = 1;
	func_recs[0].in_format = "count_sample_recs";
	func_recs[0].out_format = "count_sample_recs";
}

void populate_load_mon_query_funcs(struct func_rec *func_recs) {
	func_recs[5].func_name = "dll:./../functions/libfunctions.la:rec_apache_group_map";
	func_recs[5].func_type = 4;  // Routing 
	func_recs[5].func_order = 1;
	func_recs[5].func_level = 1;
	func_recs[5].func_splits = 1;
	func_recs[5].flevel_start_point = 1;
	func_recs[5].in_format = "sensor";
	func_recs[5].out_format = "sensor";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:rec_event_window";
	func_recs[4].func_type = 3;  // Transform 
	func_recs[4].func_order = 2;
	func_recs[4].func_level = 1;
	func_recs[4].func_splits = 1;
	func_recs[4].flevel_end_point = 1; // For Global
	func_recs[4].in_format = "sensor";
	func_recs[4].out_format = "sensor_recs";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:rec_event_apache_counting_sample";
	func_recs[3].func_type = 3;  // Transform 
	func_recs[3].func_order = 1;
	func_recs[3].func_level = 2;
	func_recs[3].func_splits = 1;
	//func_recs[3].flevel_end_point = 1; // For Local
	func_recs[3].flevel_start_point = 1; // For Global 
	func_recs[3].in_format = "sensor_recs";
	func_recs[3].out_format = "count_sample_recs";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:apache_busy_counting_sample_window";
	func_recs[2].func_type = 3;  // Transform  
	func_recs[2].func_order = 2;
	func_recs[2].func_level = 2;
	func_recs[2].func_splits = 1;
	//func_recs[2].flevel_start_point = 1;   // For Local
	//func_recs[2].flevel_start_point = 1; // For Global
	func_recs[2].in_format = "count_sample_recs";
	func_recs[2].out_format = "sample_window_recs";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:apache_busy_sample_decision";
	func_recs[1].func_type = 4;  // Routing 
	func_recs[1].func_order = 3;
	func_recs[1].func_level = 2;
	func_recs[1].func_splits = 1;
	func_recs[1].flevel_end_point = 1;
	func_recs[1].in_format = "sample_window_recs";
	func_recs[1].out_format = "sample_window_recs";

	func_recs[0].func_name = "apache_busy_sample_actuator";
	func_recs[0].func_type = 6;  // Terminal 
	func_recs[0].func_order = 4;
	func_recs[0].func_level = 2;
	func_recs[0].func_splits = 1;
	func_recs[0].in_format = "sample_window_recs";
	func_recs[0].out_format = "sample_window_recs";
}

void populate_fault_mon_query_funcs(struct func_rec *func_recs) {
	func_recs[5].func_name = "dll:./../functions/libfunctions.la:rec_fault_group_map";
	func_recs[5].func_type = 4;  // Routing 
	func_recs[5].func_order = 1;
	func_recs[5].func_level = 1;
	func_recs[5].func_splits = 1;
	func_recs[5].flevel_start_point = 1;	// For Local
	func_recs[5].in_format = "sensor";
	func_recs[5].out_format = "sensor";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:rec_event_window";
	func_recs[4].func_type = 3;  // Transform 
	func_recs[4].func_order = 2;
	//func_recs[4].func_level = 1; 		// For Local
	func_recs[4].func_level = 1;		// For Global
	func_recs[4].func_splits = 1;
	func_recs[4].flevel_end_point = 1;		// For Global 
	func_recs[4].in_format = "sensor";
	func_recs[4].out_format = "sensor_recs";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:rec_event_fault_counting_sample";
	func_recs[3].func_type = 3;  // Transform 
	func_recs[3].func_order = 1;
	//func_recs[3].func_level = 1; 		// For Local
	func_recs[3].func_level = 2;		// For Global
	func_recs[3].func_splits = 1;
	//func_recs[3].flevel_end_point = 1;		// For Local
	func_recs[3].flevel_start_point = 1;		// For Global
	func_recs[3].in_format = "sensor_recs";
	func_recs[3].out_format = "count_sample_recs";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:rubis_fault_counting_sample_window";
	func_recs[2].func_type = 3;  // Transform  
	func_recs[2].func_order = 2;
	func_recs[2].func_level = 2;
	func_recs[2].func_splits = 1;
	//func_recs[2].flevel_start_point = 1;	// For Local
	func_recs[2].in_format = "count_sample_recs";
	func_recs[2].out_format = "sample_window_recs";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:rubis_fault_sample_decision";
	func_recs[1].func_type = 4;  // Routing 
	func_recs[1].func_order = 3;
	func_recs[1].func_level = 2;
	func_recs[1].func_splits = 1;
	func_recs[1].flevel_end_point = 1;
	func_recs[1].in_format = "sample_window_recs";
	func_recs[1].out_format = "sample_window_recs";

	func_recs[0].func_name = "rubis_fault_sample_actuator";
	func_recs[0].func_type = 6;  // Terminal 
	func_recs[0].func_order = 4;
	func_recs[0].func_level = 2;
	func_recs[0].func_splits = 1;
	func_recs[0].in_format = "sample_window_recs";
	func_recs[0].out_format = "sample_window_recs";
}

void populate_dtree_mon_query_funcs(struct func_rec *func_recs) {
	func_recs[5].func_name = "dll:./../functions/libfunctions.la:rec_entropy_group_map";
	func_recs[5].func_type = 4;  // Routing 
	func_recs[5].func_order = 1;
	func_recs[5].func_level = 1;
	func_recs[5].func_splits = 1;
	func_recs[5].flevel_start_point = 1;
	func_recs[5].in_format = "sensor";
	func_recs[5].out_format = "sensor";

	func_recs[4].func_name = "dll:./../functions/libfunctions.la:rec_event_window";
	func_recs[4].func_type = 3;  // Transform 
	func_recs[4].func_order = 2;
	func_recs[4].func_level = 1;
	func_recs[4].func_splits = 1;
	func_recs[4].in_format = "sensor";
	func_recs[4].out_format = "sensor_recs";

	func_recs[3].func_name = "dll:./../functions/libfunctions.la:rec_local_entropy_stats";
	func_recs[3].func_type = 3;  // Transform 
	func_recs[3].func_order = 3;
	func_recs[3].func_level = 1;
	func_recs[3].func_splits = 1;
	func_recs[3].flevel_end_point = 1;
	func_recs[3].in_format = "sensor_recs";
	func_recs[3].out_format = "recs_entropy";

	func_recs[2].func_name = "dll:./../functions/libfunctions.la:rec_global_entropy_window";
	func_recs[2].func_type = 3;  // Transform  
	func_recs[2].func_order = 1;
	func_recs[2].func_level = 2;
	func_recs[2].func_splits = 1;
	func_recs[2].flevel_start_point = 1;
	func_recs[2].in_format = "recs_entropy";
	func_recs[2].out_format = "global_entropy";

	func_recs[1].func_name = "dll:./../functions/libfunctions.la:rec_entropy_threshold_decision";
	func_recs[1].func_type = 4;  // Routing 
	func_recs[1].func_order = 2;
	func_recs[1].func_level = 2;
	func_recs[1].func_splits = 1;
	func_recs[1].flevel_end_point = 1;
	func_recs[1].in_format = "global_entropy";
	func_recs[1].out_format = "global_entropy";

	func_recs[0].func_name = "dtree_sample_actuator";
	func_recs[0].func_type = 6;  // Terminal 
	func_recs[0].func_order = 3;
	func_recs[0].func_level = 2;
	func_recs[0].func_splits = 1;
	func_recs[0].in_format = "global_entropy";
	func_recs[0].out_format = "global_entropy";
}

void init_metric_list(struct metric_list *metric_list_ptr) {
	metric_list_ptr->metric_recs = (char **) calloc(metric_list_ptr->num_recs, sizeof(char **));
	if(metric_list_ptr->num_recs == 11) {
		metric_list_ptr->metric_recs[0] = "cpu_nice";
		metric_list_ptr->metric_recs[1] = "cpu_user";
		metric_list_ptr->metric_recs[2] = "cpu_system";
		metric_list_ptr->metric_recs[3] = "cpu_idle"; 
		metric_list_ptr->metric_recs[4] = "mem_buffers";
		metric_list_ptr->metric_recs[5] = "mem_free";
		metric_list_ptr->metric_recs[6] = "mem_cached";
		metric_list_ptr->metric_recs[7] = "swap_free";
		metric_list_ptr->metric_recs[8] = "load_one";
		metric_list_ptr->metric_recs[9] = "load_five";
		metric_list_ptr->metric_recs[10] = "load_fifteen";
	} else if(metric_list_ptr->num_recs == 14) {
		metric_list_ptr->metric_recs[0] = "cpu_nice";
		metric_list_ptr->metric_recs[1] = "cpu_user";
		metric_list_ptr->metric_recs[2] = "cpu_system";
		metric_list_ptr->metric_recs[3] = "cpu_idle"; 
		metric_list_ptr->metric_recs[4] = "mem_buffers";
		metric_list_ptr->metric_recs[5] = "mem_free";
		metric_list_ptr->metric_recs[6] = "mem_cached";
		metric_list_ptr->metric_recs[7] = "swap_free";
		metric_list_ptr->metric_recs[8] = "load_one";
		metric_list_ptr->metric_recs[9] = "load_five";
		metric_list_ptr->metric_recs[10] = "load_fifteen";
		metric_list_ptr->metric_recs[11] = "cpu_min_freq";
		metric_list_ptr->metric_recs[12] = "cpu_max_freq";
		metric_list_ptr->metric_recs[13] = "cpu_cur_freq";
	} else if (metric_list_ptr->num_recs == 1) {
		metric_list_ptr->metric_recs[0] = "cpu_idle";
	} else if (metric_list_ptr->num_recs == 2) {
		metric_list_ptr->metric_recs[0] = "cpu_idle";
		metric_list_ptr->metric_recs[1] = "mem_free";
	} else if (metric_list_ptr->num_recs == 3) {
		metric_list_ptr->metric_recs[0] = "load_one";
		metric_list_ptr->metric_recs[1] = "load_five";
		metric_list_ptr->metric_recs[2] = "load_fifteen";
	} else if (metric_list_ptr->num_recs == 6) { // Entropy Decision Tree 
		metric_list_ptr->metric_recs[0] = "entropy_web_ctotaltime";
		metric_list_ptr->metric_recs[1] = "entropy_serv_ctotaltime";
		metric_list_ptr->metric_recs[2] = "entropy_db_ctotaltime";
		metric_list_ptr->metric_recs[3] = "entropy_web_req_rate";
		metric_list_ptr->metric_recs[4] = "entropy_serv_req_count";
		metric_list_ptr->metric_recs[5] = "entropy_serv_req_ptime";
	/*} else if (metric_list_ptr->num_recs == 7) { // Load Balancer
		metric_list_ptr->metric_recs[0] = "apache_busy_workers";
		metric_list_ptr->metric_recs[1] = "apache_idle_workers";
		metric_list_ptr->metric_recs[2] = "apache_req_rate";
		metric_list_ptr->metric_recs[3] = "apache_bytes_rate";
		metric_list_ptr->metric_recs[4] = "apache_web_ctotaltime";
		metric_list_ptr->metric_recs[5] = "apache_serv_ctotaltime";
		metric_list_ptr->metric_recs[6] = "apache_num_vcpus"; */
	} else if (metric_list_ptr->num_recs == 7) { // Software Faults
		metric_list_ptr->metric_recs[0] = "rubis_dom0_ctotaltime";
		metric_list_ptr->metric_recs[1] = "rubis_web_ctotaltime";
		metric_list_ptr->metric_recs[2] = "rubis_serv_ctotaltime";
		metric_list_ptr->metric_recs[3] = "rubis_db_ctotaltime";
		metric_list_ptr->metric_recs[4] = "rubis_web_iftxpackets";
		metric_list_ptr->metric_recs[5] = "rubis_serv_iftxpackets";
		metric_list_ptr->metric_recs[6] = "rubis_db_iftxpackets";
	}
}

void init_func_list(struct func_list *func_list_ptr) {
	func_list_ptr->func_recs = (struct func_rec *) calloc(func_list_ptr->num_recs, sizeof(struct func_rec));
	for(int i = 0; i < func_list_ptr->num_recs; i ++) {
		func_list_ptr->func_recs[i].func_name = NULL;
		func_list_ptr->func_recs[i].in_format = NULL;
		func_list_ptr->func_recs[i].out_format = NULL;
		func_list_ptr->func_recs[i].flevel_start_point = 0;
		func_list_ptr->func_recs[i].flevel_end_point = 0;
		func_list_ptr->func_recs[i].func_splits = 0; 
		func_list_ptr->func_recs[i].func_type = -1; 
		func_list_ptr->func_recs[i].func_order = 0;
		func_list_ptr->func_recs[i].func_level = -1;
	}
}

struct stone_context *get_node_transmit_source_stone_ids(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	//for(i = 0; i < stone_ctx_num; i++) {
	//	stone_ids[i] = stone_ctx[i].split_stone;
	//}
	return stone_ctx;
}

int *get_node_transmit_target_stone_ids(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	int i;
	for(i = 0; i < stone_ctx_num; i++) {
		stone_ids[i] = stone_ctx[i].stone_id;
	}
	return stone_ids;
}

int *get_collect_target_stone_ids(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	int i;
	for(i = 0; i < stone_ctx_num; i++) {
		stone_ids[i] = stone_ctx[i].stone_id;
	}
	return stone_ids;
}

struct stone_context *get_group_source_stone_ids(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	//for(i = 0; i < stone_ctx_num; i++) {
	//	stone_ids[i] = stone_ctx[i].split_stone;
	//}
	return stone_ctx;
}

int *get_group_target_stone_ids(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	int i;
	for(i = 0; i < stone_ctx_num; i++) {
		stone_ids[i] = stone_ctx[i].stone_id;
	}
	return stone_ids;
}

int get_metric_target_stone_id(struct axon *axctx, char *node_contact_list, int qid, int stone_ctx_num, char *func_name) {
	struct stone_context *stone_ctx; int stone_num = 0; int *stone_ids;
	get_qnode_state(axctx, node_contact_list, qid, func_name, stone_ctx_num, stone_ctx); 
	if(!stone_ctx_num) {
		stone_ids = (int *)calloc(stone_ctx_num, sizeof(int));
	}
	int i;
	for(i = 0; i < stone_ctx_num; i++) {
		stone_ids[i] = stone_ctx[i].stone_id;
	}
	return stone_ids[0];
}

void create_node_transmit_source_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_split_num = 0; int pcopies;
	struct stone_context *stone_ctx = get_node_transmit_source_stone_ids(axctx, node_contact_list, qid, reconf_split_num, 
	reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_split_num == 0) {
		reconf_split_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs = (struct reconf_split_recs *) calloc(1, sizeof(struct reconf_split_recs));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num = reconf_split_num;
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits = 
	(struct reconf_split_context *) calloc(reconf_split_num, sizeof(struct reconf_split_context));
	
	int i;
	for(i = 0; i < reconf_split_num; i++) {
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_stone = stone_ctx[i].split_stone; 
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_action = stone_ctx[i].split_action;  
	}
}

void create_node_transmit_target_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_stone_num = 0; int pcopies;
	// node_contact_list == aggregator_contact_list == coordinator_contact_list
	int *target_stone_ids = get_node_transmit_target_stone_ids(axctx, node_contact_list, qid, reconf_stone_num, 
	reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_stone_num == 0) {
		reconf_stone_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_out_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs = (struct reconf_stone_recs *) calloc(1, sizeof(struct reconf_stone_recs));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num = reconf_stone_num;
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones = 
	(struct reconf_stone_context *) calloc(reconf_stone_num, sizeof(struct reconf_stone_context));
	
	int i;
	for(i = 0; i < reconf_stone_num; i++) {
		nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[i].stone_id = target_stone_ids[i]; 
	}
}

void create_collect_source_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_split_num = 0; int pcopies;
	struct stone_context *stone_ctx = get_group_source_stone_ids(axctx, node_contact_list, qid, reconf_split_num, 
	reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_split_num == 0) {
		reconf_split_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs = (struct reconf_split_recs *) calloc(1, sizeof(struct reconf_split_recs));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num = reconf_split_num;
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits = 
	(struct reconf_split_context *) calloc(reconf_split_num, sizeof(struct reconf_split_context));
	
	int i;
	for(i = 0; i < reconf_split_num; i++) {
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_stone = stone_ctx[i].split_stone; 
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_action = stone_ctx[i].split_action;  
	}
}

void create_collect_target_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_stone_num = 0; int pcopies;
	int *target_stone_ids = get_collect_target_stone_ids(axctx, node_contact_list, qid, reconf_stone_num, reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_stone_num == 0) {
		reconf_stone_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_out_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs = (struct reconf_stone_recs *) calloc(1, sizeof(struct reconf_stone_recs));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num = reconf_stone_num;
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones = 
	(struct reconf_stone_context *) calloc(reconf_stone_num, sizeof(struct reconf_stone_context));
	
	int i;
	for(i = 0; i < reconf_stone_num; i++) {
		nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[i].stone_id = target_stone_ids[i]; 
	}
}

void create_group_source_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_split_num = 0; int pcopies;
	struct stone_context *stone_ctx = get_group_source_stone_ids(axctx, node_contact_list, qid, reconf_split_num, 
	reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_split_num == 0) {
		reconf_split_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs = (struct reconf_split_recs *) calloc(1, sizeof(struct reconf_split_recs));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num = reconf_split_num;
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits = 
	(struct reconf_split_context *) calloc(reconf_split_num, sizeof(struct reconf_split_context));
	
	int i;
	for(i = 0; i < reconf_split_num; i++) {
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_stone = stone_ctx[i].split_stone; 
		nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[i].split_action = stone_ctx[i].split_action;  
	}
}

void create_group_target_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_stone_num = 0; int pcopies;
	int *target_stone_ids = get_group_target_stone_ids(axctx, node_contact_list, qid, reconf_stone_num, reconf_recs_ptr->reconf_funcs[0].func_name);
	if(reconf_stone_num == 0) {
		reconf_stone_num = pcopies;
	}
		
	nmon_reconf_rec_ptr->reconf_out_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs = (struct reconf_stone_recs *) calloc(1, sizeof(struct reconf_stone_recs));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num = reconf_stone_num;
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones = 
	(struct reconf_stone_context *) calloc(reconf_stone_num, sizeof(struct reconf_stone_context));
	
	int i;
	for(i = 0; i < reconf_stone_num; i++) {
		nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[i].stone_id = target_stone_ids[i]; 
	}
}
	
void create_metric_target_rec(struct axon *axctx, char *node_contact_list, int qid, int action, int action_type, 
struct reconf_func_recs *reconf_recs_ptr, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	int reconf_stone_num = 0;
	int target_stone_id = get_metric_target_stone_id(axctx, node_contact_list, qid, reconf_stone_num, reconf_recs_ptr->reconf_funcs[0].func_name);
	int pcopies = reconf_recs_ptr->reconf_funcs[0].func_pcopies;

	nmon_reconf_rec_ptr->reconf_out_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs = (struct reconf_stone_recs *) calloc(1, sizeof(struct reconf_stone_recs));
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num = pcopies;
	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones = 
	(struct reconf_stone_context *) calloc(pcopies, sizeof(struct reconf_stone_context));

	nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = target_stone_id; 
}

void create_mongraph_reconf_rec(char *node_contact_list, int qid, int action, int action_type, struct reconf_func_recs *reconf_recs_ptr, 
struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	nmon_reconf_rec_ptr = (struct node_mongraph_reconf_rec *) calloc(1, sizeof(struct node_mongraph_reconf_rec));
	nmon_reconf_rec_ptr->qid = qid;
	nmon_reconf_rec_ptr->node_contact_list = strdup(node_contact_list);
	nmon_reconf_rec_ptr->action = action;
	nmon_reconf_rec_ptr->action_type = action_type;
	nmon_reconf_rec_ptr->reconf_recs = reconf_recs_ptr;
	nmon_reconf_rec_ptr->reconf_in_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs = (struct reconf_split_recs *) calloc(1, sizeof(struct reconf_split_recs));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num = reconf_recs_ptr->reconf_funcs[0].func_vsplits;
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits = 
	(struct reconf_split_context *) calloc(reconf_recs_ptr->reconf_funcs[0].func_vsplits, sizeof(struct reconf_split_context));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs = (struct reconf_stone_recs *) calloc(1, sizeof(struct reconf_stone_recs));
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stone_num = reconf_recs_ptr->reconf_funcs->func_pcopies;
	nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones = 
	(struct reconf_stone_context *) calloc(reconf_recs_ptr->reconf_funcs[0].func_pcopies, sizeof(struct	reconf_stone_context));
}

void populate_metric_target_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_group_map";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_group_target_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_event_window";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 0;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 0;
	}
}

void populate_group_source_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "_metric";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_collect_target_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "_output";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 0;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 0;
	}
}

void populate_collect_source_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_group_map";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 3;
	}
}

void populate_node_transmit_target_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "event_counting_sample";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].out_format = "count_sample_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_node_transmit_source_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_event_window";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_metric_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "cpu_idle";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_group_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_group_map";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 3;
	}
}

void populate_collect_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "rec_event_window";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor";
		reconf_recs_ptr->reconf_funcs[i].out_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 3;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_reconf_node_transmit_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "event_counting_sample";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].out_format = "count_sample_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void populate_analysis_recs(struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < reconf_recs_ptr->reconf_func_num; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = "event_counting_sample";
		reconf_recs_ptr->reconf_funcs[i].in_format = "sensor_recs";
		reconf_recs_ptr->reconf_funcs[i].out_format = "count_sample_recs";
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 1;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 1;
	}
}

void init_reconf_func_recs(int num_recs, struct reconf_func_recs *reconf_recs_ptr) {
	int i;
	for(i = 0; i < num_recs; i++) {
		reconf_recs_ptr->reconf_funcs[i].func_name = NULL;
		reconf_recs_ptr->reconf_funcs[i].in_format = NULL;
		reconf_recs_ptr->reconf_funcs[i].out_format = NULL;
		reconf_recs_ptr->reconf_funcs[i].func_pcopies = 0;
		reconf_recs_ptr->reconf_funcs[i].func_vsplits = 0;
	}
}

static void update_coord_node_graph_data(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("Node updating coordinator that it deployed the graph ....\n");
	if(((struct node_graph_deploy *)msg)->node_type == 1) {
		register_aggregator_endpoints_for_query(cm, axctx, (cyctx->nomap[0]).node_coord_contact_list, ((struct node_graph_deploy *)msg)->graph_id, 
		NUM_NODES, ((struct node_graph_deploy *)msg)->stone_endpoints, ((struct node_graph_deploy *)msg)->func_endpoints);
		register_aggregator_graph_deployed_for_query(cm, (struct axon *)client_data, NUM_AGGREGATORS, 
		((struct node_graph_deploy *)msg)->node_contact_list, ((struct node_graph_deploy *)msg)->graph_id);
	} else if(((struct node_graph_deploy *)msg)->node_type == 0) {
		register_nodes_endpoints_for_query(cm, axctx, ((struct node_graph_deploy *)msg)->node_contact_list, (cyctx->nomap[0]).node_coord_contact_list, 
		((struct node_graph_deploy *)msg)->graph_id, NUM_NODES, ((struct node_graph_deploy *)msg)->stone_endpoints, 
		((struct node_graph_deploy *)msg)->func_endpoints);
		register_nodes_graph_deployed_for_query(cm, (struct axon *)client_data, NUM_NODES, ((struct node_graph_deploy *)msg)->node_contact_list, 
		((struct node_graph_deploy *)msg)->graph_id); // Changed the num_deploy from 1 to 5
	}
}

void populate_node_graph_deploy_msg(struct node_graph_deploy *node_graph_deploy_data, char *node_coord_contact_list, char *node_contact_list,
int node_type, int qid, int graph_id, struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list) {
	node_graph_deploy_data->graph_id = qid; 
	node_graph_deploy_data->is_graph_deployed = 1; 
	node_graph_deploy_data->node_contact_list = strdup(node_contact_list); 
	node_graph_deploy_data->node_type = node_type; 

	node_graph_deploy_data->stone_endpoints = (struct stone_context_list *) calloc(2, sizeof(struct stone_context_list));
	node_graph_deploy_data->stone_endpoints[0].stone_ctx_num = stone_ctx_list[0].stone_ctx_num;
	node_graph_deploy_data->stone_endpoints[0].stone_ctx = (struct stone_context *) calloc(stone_ctx_list[0].stone_ctx_num, 
	sizeof(struct stone_context));

	node_graph_deploy_data->stone_endpoints[1].stone_ctx_num = stone_ctx_list[1].stone_ctx_num;
	node_graph_deploy_data->stone_endpoints[1].stone_ctx = (struct stone_context *) calloc(stone_ctx_list[1].stone_ctx_num, 
	sizeof(struct stone_context));

	node_graph_deploy_data->func_endpoints = (struct func_context_list *) calloc(2, sizeof(struct func_context_list));
	node_graph_deploy_data->func_endpoints[0].func_ctx_num = func_ctx_list[0].func_ctx_num;
	node_graph_deploy_data->func_endpoints[0].func_ctx = (struct func_context *) calloc(func_ctx_list[0].func_ctx_num, sizeof(struct func_context));

	node_graph_deploy_data->func_endpoints[1].func_ctx_num = func_ctx_list[1].func_ctx_num;
	node_graph_deploy_data->func_endpoints[1].func_ctx = (struct func_context *) calloc(func_ctx_list[1].func_ctx_num, sizeof(struct func_context));

	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < stone_ctx_list[i].stone_ctx_num; j++) {
			node_graph_deploy_data->stone_endpoints[i].stone_ctx[j].stone_id = stone_ctx_list[i].stone_ctx[j].stone_id;
			node_graph_deploy_data->stone_endpoints[i].stone_ctx[j].stone_action = stone_ctx_list[i].stone_ctx[j].stone_action;
			node_graph_deploy_data->stone_endpoints[i].stone_ctx[j].split_stone = stone_ctx_list[i].stone_ctx[j].split_stone;
			node_graph_deploy_data->stone_endpoints[i].stone_ctx[j].split_action = stone_ctx_list[i].stone_ctx[j].split_action;
			node_graph_deploy_data->func_endpoints[i].func_ctx[j].func_name = func_ctx_list[i].func_ctx[j].func_name;
			node_graph_deploy_data->func_endpoints[i].func_ctx[j].in_format = func_ctx_list[i].func_ctx[j].in_format;
			node_graph_deploy_data->func_endpoints[i].func_ctx[j].out_format = func_ctx_list[i].func_ctx[j].out_format;
			node_graph_deploy_data->func_endpoints[i].func_ctx[j].func_order = func_ctx_list[i].func_ctx[j].func_order;
			node_graph_deploy_data->func_endpoints[i].func_ctx[j].func_level = func_ctx_list[i].func_ctx[j].func_level;
		}
	}
}

void msg_coord_for_deployed_graph_for_query(CManager cm, struct axon *axctx, char *node_coord_contact_list, char*node_contact_list, 
int node_type, int qid, int graph_id, struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list) {
	attr_list coord_attr_list = attr_list_from_string(node_coord_contact_list);
	printf("Msg coordinator %s that it deployed the graph ....\n", node_coord_contact_list);

	CMConnection conn = CMget_conn(cm, coord_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(coord_attr_list);
		printf("\n");
		exit(1);
	}
	node_graph_deploy_format = CMregister_format(cm, node_graph_deploy_format_list);
	CMregister_handler(node_graph_deploy_format, update_coord_node_graph_data, ctx);
	
	node_graph_deploy node_graph_deploy_data;
	populate_node_graph_deploy_msg(&node_graph_deploy_data, node_coord_contact_list, node_contact_list, node_type, qid, graph_id,
	stone_ctx_list, func_ctx_list);
	printf("Sending msg to the coordinator %s about graph deployment ....\n", node_coord_contact_list);
	CMwrite(conn, node_graph_deploy_format, &node_graph_deploy_data);
}

void create_node_graph_streams_network(CManager cm, struct axon *axctx, struct node_deploy *node_deploy) {
	int *stoneids = NULL;
	int stone_count = -1;
	int pcopies = 0;

	pcopies = node_deploy->func_recs[node_deploy->num_recs - 1].func_splits;

	struct split_context_list *split_ctx_list = (struct split_context_list *) calloc(1, sizeof(struct split_context_list));

	struct stone_context_list *stone_ctx_list = (struct stone_context_list *) calloc(2, sizeof(struct stone_context_list));
	stone_ctx_list[0].stone_ctx_num =  pcopies; //pcopies; // or 2
	stone_ctx_list[0].stone_ctx = (struct stone_context *) calloc(pcopies, sizeof(struct stone_context));
	init_stone_context(pcopies, stone_ctx_list[0].stone_ctx);
	stone_ctx_list[1].stone_ctx_num =  1; //pcopies; // or 2
	stone_ctx_list[1].stone_ctx = (struct stone_context *) calloc(1, sizeof(struct stone_context));
	init_stone_context(1, stone_ctx_list[1].stone_ctx);

	struct func_context_list *func_ctx_list = (struct func_context_list *)calloc(2, sizeof(struct func_context_list));
	func_ctx_list[0].func_ctx_num = pcopies; //pcopies; // or 2
	func_ctx_list[0].func_ctx = (struct func_context *)calloc(pcopies, sizeof(struct func_context));
	init_func_context(pcopies, func_ctx_list[0].func_ctx);
	func_ctx_list[1].func_ctx_num = 1; //pcopies; // or 2
	func_ctx_list[1].func_ctx = (struct func_context *)calloc(1, sizeof(struct func_context));
	init_func_context(1, func_ctx_list[1].func_ctx);

	EVSimpleHandlerFunc handler; char *handler_data;
	for(int j = 0; j < node_deploy->num_recs; j++) {
		if(node_deploy->func_recs[j].func_level == 1) { // Fixed bug for change func_level from 2 to 1
			int *local_ids = realize_vertex(cm, node_deploy->query_id, (node_deploy->func_recs[j]), pcopies, handler, handler_data, 
			stone_ctx_list, func_ctx_list, split_ctx_list); 
			stone_count += 1;
			if(stone_count > 0) {
				realize_edge(cm, node_deploy->query_id, pcopies, split_ctx_list, stoneids);
				if(node_deploy->func_recs[j].flevel_start_point == 0) {
					clean_stone_ids(pcopies, stoneids);
					clean_split_context(pcopies, split_ctx_list->split_ctx);
				} else {
					clean_stone_ids(1, stoneids);
					clean_split_context(1, split_ctx_list->split_ctx);
				}
			}
			stoneids = local_ids;
		}
	}

	printf("Finished setting up monitoring graph on the node %s ....\n", node_deploy->node_contact_list);
	printf("Aggregator side stone ctx list element 1:: stone_id %d stone_action : %d split_stone : %d split_action : %d ....\n",
	stone_ctx_list[0].stone_ctx[0].stone_id, stone_ctx_list[0].stone_ctx[0].stone_action,
	stone_ctx_list[0].stone_ctx[0].split_stone, stone_ctx_list[0].stone_ctx[0].split_action);
	/*
	printf("Aggregator side stone ctx list element 2:: stone_id %d stone_action : %d split_stone : %d split_action : %d ....\n",
	stone_ctx_list[0].stone_ctx[1].stone_id, stone_ctx_list[0].stone_ctx[1].stone_action,
	stone_ctx_list[0].stone_ctx[1].split_stone, stone_ctx_list[0].stone_ctx[1].split_action);
	printf("Aggregator side stone ctx list element 3:: stone_id %d stone_action : %d split_stone : %d split_action : %d ....\n",
	stone_ctx_list[0].stone_ctx[2].stone_id, stone_ctx_list[0].stone_ctx[2].stone_action,
	stone_ctx_list[0].stone_ctx[2].split_stone, stone_ctx_list[0].stone_ctx[2].split_action);
	*/
	msg_coord_for_deployed_graph_for_query(cm, axctx, node_deploy->node_coord_contact_list, node_deploy->node_contact_list,
	node_deploy->node_type, node_deploy->query_id, node_deploy->graph_id, stone_ctx_list, func_ctx_list);
	free(split_ctx_list);
	free(stone_ctx_list);
	free(func_ctx_list);
}

void create_node_aggregator_streams_network(CManager cm, struct axon *axctx, struct node_deploy *node_deploy) {
	int *stone_ids = NULL;
	int stone_count = -1;
	int pcopies = 0;

	pcopies = 1;
	stone_ids = (int *) calloc(pcopies, sizeof(int));
	init_stone_ids(pcopies, stone_ids);

	struct split_context_list *split_ctx_list = (struct split_context_list *) calloc(1, sizeof(struct split_context_list));

	struct stone_context_list *stone_ctx_list = (struct stone_context_list *) calloc(2, sizeof(struct stone_context_list));
	stone_ctx_list[0].stone_ctx_num =  pcopies; //pcopies; // or 2
	stone_ctx_list[0].stone_ctx = (struct stone_context *) calloc(pcopies, sizeof(struct stone_context));
	init_stone_context(pcopies, stone_ctx_list[0].stone_ctx);
	stone_ctx_list[1].stone_ctx_num =  1; //pcopies; // or 2
	stone_ctx_list[1].stone_ctx = (struct stone_context *) calloc(1, sizeof(struct stone_context));
	init_stone_context(1, stone_ctx_list[1].stone_ctx);

	struct func_context_list *func_ctx_list = (struct func_context_list *)calloc(2, sizeof(struct func_context_list));
	func_ctx_list[0].func_ctx_num = pcopies; //pcopies; // or 2
	func_ctx_list[0].func_ctx = (struct func_context *)calloc(pcopies, sizeof(struct func_context));
	init_func_context(pcopies, func_ctx_list[0].func_ctx);
	func_ctx_list[1].func_ctx_num = 1; //pcopies; // or 2
	func_ctx_list[1].func_ctx = (struct func_context *)calloc(1, sizeof(struct func_context));
	init_func_context(1, func_ctx_list[1].func_ctx);

	EVSimpleHandlerFunc handler; char *handler_data;
	for(int j = 0; j < node_deploy->num_recs; j++) {
		if(node_deploy->func_recs[j].func_level == 1) { // Fixed bug for change func_level from 2 to 1
			int *local_ids = realize_vertex(cm, node_deploy->query_id, (node_deploy->func_recs[j]), pcopies, handler, handler_data, 
			stone_ctx_list, func_ctx_list, split_ctx_list); 
			stone_count += 1;
			if(stone_count > 0) {
				realize_edge(cm, node_deploy->query_id, pcopies, split_ctx_list, stone_ids);
				if(node_deploy->func_recs[j].flevel_start_point == 0) {
					clean_stone_ids(pcopies, stone_ids);
					clean_split_context(pcopies, split_ctx_list->split_ctx);
				} else {
					clean_stone_ids(1, stone_ids);
					clean_split_context(1, split_ctx_list->split_ctx);
				}
				stone_ids = local_ids;
			}
		}
		//free(split_ctx_list->split_ctx);
	}
	msg_coord_for_deployed_graph_for_query(cm, axctx, node_deploy->node_coord_contact_list, node_deploy->node_contact_list,
	node_deploy->node_type, node_deploy->query_id, node_deploy->graph_id, stone_ctx_list, func_ctx_list);
	free(split_ctx_list);
	free(stone_ctx_list);
	free(func_ctx_list);
}

static void deploy_funcs(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("Ready to deploy monitoring graph functions on node : %s  with node type : %d ....\n", ((struct node_deploy *)msg)->node_contact_list,
	((struct node_deploy *)msg)->node_type);
	if(((struct node_deploy *)msg)->node_type == 1) {
		printf("Creating aggregator deploy monitoring graph functions on node %s ....\n", ((struct node_deploy *)msg)->node_contact_list);
		create_node_aggregator_streams_network(cm, (struct axon *)client_data, (struct node_deploy *)msg);
	} else if (((struct node_deploy *)msg)->node_type == 0){
		printf("Creating node graph deploy monitoring graph functions on node %s ....\n", ((struct node_deploy *)msg)->node_contact_list);
		create_node_graph_streams_network(cm, (struct axon *)client_data, (struct node_deploy *)msg);
	}
}

/*
char *get_mon_metric_func(struct reconf_func_context reconf_func) {
	char *mon_metric_func = NULL;
	char *mon_metric = strdup(reconf_func.func_name);

	if(strcmp(mon_metric, "cpu_nice") == 0) {
		mon_metric_func = strdup(get_cpu_nice_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_user") == 0) {
		mon_metric_func = strdup(get_cpu_user_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_system") == 0) {
		mon_metric_func = strdup(get_cpu_system_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_idle") == 0) {
		mon_metric_func = strdup(get_cpu_idle_metrics_ECL);
	}
	if(strcmp(mon_metric, "mem_buffers") == 0) {
		mon_metric_func = strdup(get_mem_buffers_metrics_ECL);
	}
	if(strcmp(mon_metric, "mem_free") == 0) {
		mon_metric_func = strdup(get_mem_free_metrics_ECL);
	}
	if(strcmp(mon_metric, "mem_cached") == 0) {
		mon_metric_func = strdup(get_mem_cached_metrics_ECL);
	}
	if(strcmp(mon_metric, "swap_free") == 0) {
		mon_metric_func = strdup(get_swap_free_metrics_ECL);
	}
	if(strcmp(mon_metric, "load_one") == 0) {
		mon_metric_func = strdup(get_load_one_metrics_ECL);
	}
	if(strcmp(mon_metric, "load_five") == 0) {
		mon_metric_func = strdup(get_load_five_metrics_ECL);
	}
	if(strcmp(mon_metric, "load_fifteen") == 0) {
		mon_metric_func = strdup(get_load_fifteen_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_min_freq") == 0) {
		mon_metric_func = strdup(get_cpu_min_freq_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_max_freq") == 0) {
		mon_metric_func = strdup(get_cpu_max_freq_metrics_ECL);
	}
	if(strcmp(mon_metric, "cpu_cur_freq") == 0) {
		mon_metric_func = strdup(get_cpu_cur_freq_metrics_ECL);
	}

	return mon_metric_func;
}

void add_metric_target(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		//nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num = reconf_stone_num;
		int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int target_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, target_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_metric(CManager cm, struct  node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		char *mon_metric_func_name = strdup(get_mon_metric_func(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i]));
		char *mon_metric_out_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].out_format);

		int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_vsplits;
		int qid = nmon_reconf_rec_ptr->qid;

		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		EVstone mon_metric_stone; EVaction mon_metric_action; 
		EVstone mon_metric_split_stone; EVaction mon_metric_split_action;
		
		do {
			mon_metric_stone = REValloc_stone (rconn);
			FMStructDescRec *metric_out_format_list = get_format_list(mon_metric_out_format);
			char *mon_action_spec = create_transform_action_spec(NULL, metric_out_format_list, mon_metric_func_name);
			mon_metric_action = REVassoc_immediate_action (rconn, mon_metric_stone, mon_action_spec);

			do {
				EVstone *targets = NULL;
				mon_metric_split_stone = REValloc_stone (rconn);
				mon_metric_split_action = REVassoc_split_action(rconn, mon_metric_split_stone, targets);
				REVaction_set_output(rconn, mon_metric_stone, mon_metric_action, vsplits - 1, mon_metric_split_stone);
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone = mon_metric_split_stone; 
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action = mon_metric_split_action; 
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			printf(" Metric : %s num %d deployed with stone id : %d and stone action id : %d \n", mon_metric_func_name, pcopies, mon_metric_stone, 
			mon_metric_action);
			printf(" Metric : %s num %d exporting split stone id : %d and split action id : %d \n", mon_metric_func_name, pcopies, 
			mon_metric_split_stone, mon_metric_split_action);

			int sample_sec = 5; int sample_usec = 0;
			REVenable_auto_stone(rconn, mon_metric_stone, sample_sec, sample_usec);
			printf(" Metric : %s num %d monitoring started with sample_sec : %d and sample_usec : %d \n", mon_metric_func_name, pcopies, sample_sec, 
			sample_usec);

			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = mon_metric_stone; 
			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_action = mon_metric_action; 
			
			pcopies = pcopies - 1;
			sleep(1);	
		} while (pcopies > 0);
	}
}

void add_group_source(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		//int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int target_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, target_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_group_target(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		//int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int target_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, target_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_group(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		char *mon_group_func_name = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_name);
		char *mon_group_in_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].in_format);

		int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_vsplits;
		int qid = nmon_reconf_rec_ptr->qid;

		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		EVstone mon_group_stone; EVaction mon_group_action; 
		EVstone mon_group_split_stone; EVaction mon_group_split_action;
		
		do {
			mon_group_stone = REValloc_stone (rconn);
			FMStructDescRec *group_in_format_list = get_format_list(mon_group_in_format);
			char *mon_action_spec = create_router_action_spec(group_in_format_list, mon_group_func_name);
			mon_group_action = REVassoc_immediate_action (rconn, mon_group_stone, mon_action_spec);

			do {
				EVstone *targets = NULL;
				mon_group_split_stone = REValloc_stone (rconn);
				mon_group_split_action = REVassoc_split_action(rconn, mon_group_split_stone, targets);
				REVaction_set_output(rconn, mon_group_stone, mon_group_action, vsplits - 1, mon_group_split_stone);
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone = mon_group_split_stone; 
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action = mon_group_split_action; 
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = mon_group_stone; 
			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_action = mon_group_action; 
			
			pcopies = pcopies - 1;
			sleep(1);	
		} while (pcopies > 0);
	}
}

void add_collect_source(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		//int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int target_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, target_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_collect_target(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		//int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int target_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, target_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_collect(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		char *mon_collect_func_name = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_name);
		char *mon_collect_in_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].in_format);
		char *mon_collect_out_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].out_format);

		int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_vsplits;
		int qid = nmon_reconf_rec_ptr->qid;

		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		EVstone mon_collect_stone; EVaction mon_collect_action; 
		EVstone mon_collect_split_stone; EVaction mon_collect_split_action;
		
		do {
			mon_collect_stone = REValloc_stone (rconn);
			FMStructDescRec *collect_in_format_list = get_format_list(mon_collect_in_format);
			FMStructDescRec *collect_out_format_list = get_format_list(mon_collect_out_format);
			char *mon_action_spec = create_transform_action_spec(collect_in_format_list, collect_out_format_list, mon_collect_func_name);
			mon_collect_action = REVassoc_immediate_action (rconn, mon_collect_stone, mon_action_spec);

			do {
				EVstone *targets = NULL;
				mon_collect_split_stone = REValloc_stone (rconn);
				mon_collect_split_action = REVassoc_split_action(rconn, mon_collect_split_stone, targets);
				REVaction_set_output(rconn, mon_collect_stone, mon_collect_action, vsplits - 1, mon_collect_split_stone);
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone = mon_collect_split_stone; 
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action = mon_collect_split_action; 
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = mon_collect_stone; 
			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_action = mon_collect_action; 
			
			pcopies = pcopies - 1;
			sleep(1);	
		} while (pcopies > 0);
	}
}

void add_node_transmit_source(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		//int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_split_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		do {
			int aggregator_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			do {
				int in_split_stone = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone; 
				int in_split_action = nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action; 
				REVaction_add_split_target(rconn, in_split_stone, in_split_action, aggregator_stone_id);
				vsplits = vsplits - 1;
			} while (vsplits > 0);
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_node_transmit_target(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		int pcopies = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stone_num;
		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 

		EVstone node_output_stone;
		do {
			int aggregator_stone_id = nmon_reconf_rec_ptr->reconf_out_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id;
			node_output_stone = REVcreate_bridge_action(rconn, node_attr_list, aggregator_stone_id);
			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = node_output_stone; 
			pcopies = pcopies -1;
		} while (pcopies > 0);
	}
}

void add_analysis(CManager cm, struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr) {
	int i;
	for (i = 0; i < nmon_reconf_rec_ptr->reconf_recs->reconf_func_num; i++) {
		char *mon_aggregator_func_name = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_name);
		char *mon_aggregator_in_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].in_format);
		char *mon_aggregator_out_format = strdup(nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].out_format);

		int pcopies = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_pcopies;
		int vsplits = nmon_reconf_rec_ptr->reconf_recs->reconf_funcs[i].func_vsplits;
		int qid = nmon_reconf_rec_ptr->qid;

		attr_list node_attr_list = attr_list_from_string(nmon_reconf_rec_ptr->node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		EVstone mon_aggregator_stone; EVaction mon_aggregator_action; 
		EVstone mon_aggregator_split_stone; EVaction mon_aggregator_split_action;
		
		do {
			mon_aggregator_stone = REValloc_stone (rconn);
			FMStructDescRec *aggregator_in_format_list = get_format_list(mon_aggregator_in_format);
			FMStructDescRec *aggregator_out_format_list = get_format_list(mon_aggregator_out_format);
			char *mon_action_spec = create_transform_action_spec(aggregator_in_format_list, aggregator_out_format_list, mon_aggregator_func_name);
			mon_aggregator_action = REVassoc_immediate_action (rconn, mon_aggregator_stone, mon_action_spec);

			do {
				EVstone *targets = NULL;
				mon_aggregator_split_stone = REValloc_stone (rconn);
				mon_aggregator_split_action = REVassoc_split_action(rconn, mon_aggregator_split_stone, targets);
				REVaction_set_output(rconn, mon_aggregator_stone, mon_aggregator_action, vsplits - 1, mon_aggregator_split_stone);
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_stone = mon_aggregator_split_stone; 
				nmon_reconf_rec_ptr->reconf_in_ctx->reconf_split_recs->reconf_splits[vsplits - 1].split_action = mon_aggregator_split_action; 
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_id = mon_aggregator_stone; 
			nmon_reconf_rec_ptr->reconf_in_ctx->reconf_stone_recs->reconf_stones[pcopies - 1].stone_action = mon_aggregator_action; 
			
			pcopies = pcopies - 1;
			sleep(1);	
		} while (pcopies > 0);
	}
}

void *mongraph_add_metric_comp(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			populate_metric_recs(reconf_recs_ptr);

			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 1, reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_metric(cm, nmon_reconf_rec_ptr); 
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_metric_target_recs(reconf_recs_ptr);
				create_metric_target_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_metric_target(cm, nmon_reconf_rec_ptr);
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

void *mongraph_add_group_comp(void *arg) {
	do {
		sleep(1200);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			populate_group_recs(reconf_recs_ptr);
			
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 2, reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_group(cm, nmon_reconf_rec_ptr); 
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_group_target_recs(reconf_recs_ptr);
				create_group_target_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_group_target(cm, nmon_reconf_rec_ptr);
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_group_source_recs(reconf_recs_ptr);
				create_group_source_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_group_source(cm, nmon_reconf_rec_ptr);
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
		break;
	} while(1);
	pthread_exit(NULL);
}

void *mongraph_add_collect_comp(void *arg) {
	do {
		sleep(1200);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			populate_collect_recs(reconf_recs_ptr);
			
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 3, reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_collect(cm, nmon_reconf_rec_ptr); 
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_collect_target_recs(reconf_recs_ptr);
				create_collect_target_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_collect_target(cm, nmon_reconf_rec_ptr);
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_collect_source_recs(reconf_recs_ptr);
				create_collect_source_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_collect_source(cm, nmon_reconf_rec_ptr);
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
		break;
	} while(1);
	pthread_exit(NULL);
}

void *mongraph_add_node_transmit_comp(void *arg) {
	do {
		sleep(1200);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 4, reconf_recs_ptr, nmon_reconf_rec_ptr);
				populate_node_transmit_target_recs(reconf_recs_ptr);
				create_node_transmit_target_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_node_transmit_target(cm, nmon_reconf_rec_ptr);
				init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
				populate_node_transmit_source_recs(reconf_recs_ptr);
				create_node_transmit_source_rec(((struct context *)arg)->axctx, deploy_ctx->deploy_nodes_contact_list[k], 111, 0, 2, 
				reconf_recs_ptr, nmon_reconf_rec_ptr);
				add_node_transmit_source(cm, nmon_reconf_rec_ptr);
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
		break;
	} while(1);
	pthread_exit(NULL);
}

void *mongraph_add_analysis_comp(void *arg) {
	do {
		sleep(1200);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			populate_analysis_recs(reconf_recs_ptr);
			
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				if(check_is_node_aggregator(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
					create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 4, reconf_recs_ptr, nmon_reconf_rec_ptr);
					add_analysis(cm, nmon_reconf_rec_ptr); 
					//set_qnode_state();
				}
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
		break;
	} while(1);
	pthread_exit(NULL);
}
*/

char bytestring[256];
FILE *byteslog;

EVstone request_trace_stone;
static int request_trace_data(CManager cm, void *event, void *client_data, attr_list attrs) {
		printf("NODE RECIEVED TRACE DATA ...\n");
		
		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		byteslog = fopen("tracebandwidthmeasure.log", "a");
		if(byteslog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(bytestring, "Time %lf \t %d \t Bytes %d \n", timestamp, sizeof((request_trace_ptr)event), sizeof((request_trace_ptr)event) + 
		(strlen(((request_trace_ptr)event)->server_timestamp)*sizeof(char)) +
		(strlen(((request_trace_ptr)event)->request_local_url)*sizeof(char)) +
		(strlen(((request_trace_ptr)event)->request_params) *sizeof(char)) + (strlen(((request_trace_ptr)event)->request_time)* sizeof(char)) + 
		(strlen(((request_trace_ptr)event)->client_ip)*sizeof(char)) + (strlen(((request_trace_ptr)event)->server_ip)*sizeof(char))) ;
		fwrite(bytestring, sizeof(char), strlen(bytestring), byteslog);
		fclose(byteslog);

		printf("Server Time Stamp : %s \n", (request_trace_ptr(event))->server_timestamp);
		printf("Request Local URL : %s \n", (request_trace_ptr(event))->request_local_url);
		printf("Request Params : %s \n", (request_trace_ptr(event))->request_params);
		printf("Request Processing Time : %s \n", (request_trace_ptr(event))->request_time);
		printf("Client IP Address : %s \n", (request_trace_ptr(event))->client_ip);
		printf("Server IP Address : %s \n", (request_trace_ptr(event))->server_ip);
		printf("Server Contact List : %s \n", (request_trace_ptr(event))->server_contactlist);
		printf("Request Trace Output Stone : %d \n", (request_trace_ptr(event))->request_trace_output_stone);
		printf("Request Trace Split Stone : %d \n", (request_trace_ptr(event))->request_trace_split_stone);
		printf("Request Trace Split Action : %d \n", (request_trace_ptr(event))->request_trace_split_action);

		CMConnection conn = CMget_conn(cm, attr_list_from_string((request_trace_ptr(event))->server_contactlist));
		if(conn == NULL) {
			printf("No connection, attr list was :");
			dump_attr_list(attr_list_from_string((request_trace_ptr(event))->server_contactlist));
			printf("\n");
			exit(1);
		}

		char *trace_filter_func = "{\n\
		\tif(input.request_time > 10) {\n\
		\toutput.server_timestamp = input.server_timestamp;\n\
	    \toutput.request_local_url = input.request_local_url;\n\
		\toutput.request_params = input.request_params;\n\
		\toutput.request_time = input.request_time;\n\
		\toutput.client_ip = input.client_ip;\n\
		\toutput.server_ip = input.server_ip;\n\
		\toutput.server_contactlist = input.server_contactlist;\n\
		\t} else {\n\
		\t return 0;\n\
		\t}\n\
		}";

		int request_trace_output_stone = (request_trace_ptr(event))->request_trace_output_stone;
		int request_trace_split_stone = (request_trace_ptr(event))->request_trace_split_stone;
		int request_trace_split_action = (request_trace_ptr(event))->request_trace_split_action;
		
		EVstone trace_filter_stone = REValloc_stone(conn);
		char *trace_filter_action_spec = create_filter_action_spec(request_trace_format_list, trace_filter_func);
		EVaction trace_filter_action = REVassoc_immediate_action(conn, trace_filter_stone, trace_filter_action_spec);
		REVaction_set_output(conn, trace_filter_stone, trace_filter_action, 0, request_trace_output_stone);

		//REVassoc_immediate_action(conn, trace_filter_stone, request_trace_format_list, trace_filter_func, request_output_stone);
		//REVaction_set_output(conn, request_trace_stone, request_trace_action, 0, trace_filter_stone);

		REVaction_add_split_target(conn, request_trace_split_stone, request_trace_split_action, trace_filter_stone);
		REVaction_remove_split_target(conn, request_trace_split_stone, request_trace_split_action, request_trace_output_stone);
}

char *boot_trace_func = "{\n\
	\tprintf(\"TESTING THE COORD BOOT REQUEST TRACE\\n\");\n\
}";

void boot_trace_stone(char *node_cm_contact_list, CManager cm) {
	printf("BOOT TRACE STONE ...\n");
	request_trace_stone = EValloc_stone(cm);
	printf("BOOT TRACE STONE ID : %d ...\n", request_trace_stone);
	//char *trace_action_spec = create_transform_action_spec(request_trace_format_list, request_trace_format_list, boot_trace_func);
	//EVaction request_trace_action = EVassoc_immediate_action(cm, request_trace_stone, trace_action_spec, 0);
	EVassoc_terminal_action(cm, request_trace_stone, request_trace_format_list, request_trace_data, NULL);
}

static void local_exec_func(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("NODE RECIEVED LOCAL EXEC ACTION FROM COORDINATOR \n");
	printf("LOCAL EXEC ACTION FROM COORDINATOR  -- FUNC TO EXEC - %s \n", ((struct local_exec_act *)msg)->exec_func);
	/*EVstone local_exec_stone = EValloc_stone(cm);
	char *local_action_spec = create_transform_action_spec(local_term_format_list, local_term_format_list, ((struct local_exec_act *)msg)->exec_func);
	EVaction local_exec_action = EVassoc_immediate_action(cm, local_exec_stone, local_action_spec, 0);

	EVsource loadsat_handle = EVcreate_submit_handle(cm, local_exec_stone, local_term_format_list);
	local_exec_act loadsat_lexec_data;
	loadsat_lexec_data.exec_id = 1; 
	loadsat_lexec_data.exec_func = "test"; 
	EVsubmit(loadsat_handle, &loadsat_lexec_data, NULL);*/
}

void populate_loadsat_lexec_msg(struct local_exec_act *loadsat_lexec_data, char *node_coord_contact_list, char *node_contact_list) {
	char *loadsat_exec_func = "{\n\
	\tint num_vcpus = 0;\n\
	\tlibvirt_init();\n\
	\tlibvirt_open();\n\
	\tnum_vcpus = (int)libvirt_get_domain_num_vcpus(\"Domain-0\");\n\
	\tnum_vcpus = num_vcpus - 1;\n\
	\tlibvirt_set_vcpus(\"Domain-0\", num_vcpus);\n\
	\tlibvirt_close();\n\
	}";
	loadsat_lexec_data->exec_id = 1; 
	loadsat_lexec_data->exec_func = strdup(loadsat_exec_func); 
}

void trigger_local_exec_action(int action_id, struct deploy_nodes_context *deploy_ctx, char *node_coord_contact_list) {
	printf("TRIGGER LOCAL EXEC ACTION ON CONSTRAINT VIOLATION.... \n");
	attr_list node_cm_attr_list =  NULL;
	int num_deploy_nodes = deploy_ctx->num_deploy_nodes;
	printf("TRIGGER LOCAL EXEC ACTION ON CONSTRAINT VIOLATION  NUM NODES TO DEPLOY  %d \n", num_deploy_nodes);
	for(int i = 0; i < num_deploy_nodes; i++) {
		if(strcmp(node_coord_contact_list, deploy_ctx->deploy_nodes_contact_list[i]) != 0) {
			attr_list deploy_node_attr_list = attr_list_from_string(deploy_ctx->deploy_nodes_contact_list[i]);
			CMConnection conn = CMget_conn(cm, deploy_node_attr_list);
			if (conn == NULL) {
				printf("No connection, attr list was :");
				dump_attr_list(deploy_node_attr_list);
				printf("\n");
				exit(1);
			}
			local_agent_term_format = CMregister_format(cm, local_term_format_list);
			
			local_exec_act loadsat_lexec_data;
			populate_loadsat_lexec_msg(&loadsat_lexec_data, node_coord_contact_list, attr_list_to_string(deploy_node_attr_list));
			printf("TRIGGER LOCAL EXEC ACTION  - POPULATED DATA 2SEND .... \n");
			printf("TRIGGER LOCAL EXEC ACTION SENDING NODE LOCAL EXEC FLAG \"%d\"\n", loadsat_lexec_data.exec_id);
			CMwrite(conn, local_agent_term_format, &loadsat_lexec_data);
		} else {
			// DO nothing ..
		}
	}
}

void *loadsat_control(void *arg) {
	do {
		sleep(60);
		printf("LOAD SAT CONTROL -- CHECK IF NODE IS COORDINATOR .... \n");
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			printf("LOAD SAT CONTROL -- CHECKED NODE IS COORDINATOR .... \n");
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);
			if(check_is_constraint_violated(((struct context *)arg)->axctx)) {
				printf("LOAD SAT CONTROL -- CONSTRAINT VIOLATION FOUND .... \n");
				int action_id = 1;
				trigger_local_exec_action(action_id, deploy_ctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list); 
				break;
			}
		} else {
			printf("LOAD SAT CONTROL -- CHECKED NODE IS NOT COORDINATOR GETTING OUT OF THE WAY.... \n");
			break; // Get out of the way for the mortal nodes.. only play with coordinator
		}
		//sleep(30);
	} while(1);
	pthread_exit(NULL);
}

/*
void *add_metric_to_analysis_comp(void *arg) {
	do {
		sleep(1200);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct reconf_func_recs *reconf_recs_ptr = (struct reconf_func_recs *) calloc(1, sizeof(struct reconf_func_recs));
			reconf_recs_ptr->reconf_func_num = 1;
			reconf_recs_ptr->reconf_funcs = (struct reconf_func_context *) calloc (reconf_recs_ptr->reconf_func_num, 
			sizeof(struct reconf_func_context));
			init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
			populate_analysis_recs(reconf_recs_ptr);
			
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			get_deployable_nodes_from_coord(((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 
			deploy_ctx);

			struct node_mongraph_reconf_rec *nmon_reconf_rec_ptr;
			int k;
			for(k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
				if(check_is_node_aggregator(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
					create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 4, reconf_recs_ptr, nmon_reconf_rec_ptr);
					add_analysis(cm, nmon_reconf_rec_ptr); 

					init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
					populate_collect_recs(reconf_recs_ptr);
					create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 3, reconf_recs_ptr, nmon_reconf_rec_ptr);
					add_collect(cm, nmon_reconf_rec_ptr); 

					init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
					populate_group_recs(reconf_recs_ptr);
					create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 2, reconf_recs_ptr, nmon_reconf_rec_ptr);
					add_group(cm, nmon_reconf_rec_ptr); 

					init_reconf_func_recs(reconf_recs_ptr->reconf_func_num, reconf_recs_ptr);
					populate_metric_recs(reconf_recs_ptr);
					create_mongraph_reconf_rec(deploy_ctx->deploy_nodes_contact_list[k], 111, 1, 1, reconf_recs_ptr, nmon_reconf_rec_ptr);
					add_metric(cm, nmon_reconf_rec_ptr); 
				}
			}
			free(nmon_reconf_rec_ptr);
			break;
		} 
		sleep(30);
		break;
	} while(1);
	pthread_exit(NULL);
}

>>>>>>>> Same as adding new analysis and pointing all the nodes to new analysis (freezing the existing analysis)
void *add_curr_sources_to_new_analysis(); // Change existing analysis with new one
	void *add_analysis(); // Change existing analysis with new one
	void *add_analysis_sources(); // All nodes transmit points to new analysis
>>>>>>>>
void *add_new_metric(); // Add new metric, new group for the new metric, new collect for the new group, and connect to node transmit
	void *add_collect();
	//void *add_node_transmit_source();
	void *add_collect_target();
	void *add_group();
	//void *add_collect_source();
	void *add_group_target();
	void *add_metric();
	//void *add_group_source();
	void *add_metric_target();
>>>>>>>>>
void *add_new_metric_to_curr_analysis();
	void *add_collect();
	void *add_group();
	void *add_group_target();
	void *add_metric();
	void *add_metric_target();
>>>>>>>>>
void *assoc_curr_metrics_to_new_analysis();
*/

void *deploy_entropy_query_single_metric(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 8;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 1;
			init_metric_list(metric_list_ptr);
			populate_entropy_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 222, 0, 222, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
		//if(current_time - start_time > 1000) break;
	} while(1);
	pthread_exit(NULL);
}

void *deploy_entropy_query_multi_metric(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 8;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 14;
			init_metric_list(metric_list_ptr);
			populate_entropy_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 222, 0, 222, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
		//if(current_time - start_time > 1000) break;
	} while(1);
	pthread_exit(NULL);
}

void *deploy_base_query_single_metric(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 7;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 1;
			init_metric_list(metric_list_ptr);
			populate_range_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 111, 0, 111, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

void *deploy_base_query_multi_metric(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 5;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 11;
			init_metric_list(metric_list_ptr);
			populate_multi_range_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 111, 0, 111, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			printf("Coming out of deploy monitoring graph call ... \n");
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

void *deploy_dtree_mon_query(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 6;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 6;
			init_metric_list(metric_list_ptr);
			populate_dtree_mon_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 111, 0, 111, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			printf("Finally out of decision monitoring graph call ... \n");
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

void *deploy_fault_mon_query(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 6;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 7;
			init_metric_list(metric_list_ptr);
			populate_fault_mon_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 111, 0, 111, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			printf("Finally out of fault monitoring graph call ... \n");
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

void *deploy_load_mon_query(void *arg) {
	do {
		sleep(60);
		if(check_is_node_coord(((struct context *)arg)->cyctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list)) {
			struct func_list *func_list_ptr = (struct func_list *) calloc(1, sizeof(struct func_list));
			func_list_ptr->num_recs = 6;
			init_func_list(func_list_ptr);
			struct metric_list *metric_list_ptr = (struct metric_list *) calloc(1, sizeof(struct metric_list));
			metric_list_ptr->num_recs = 7;
			init_metric_list(metric_list_ptr);
			populate_load_mon_query_funcs(func_list_ptr->func_recs);
			deploy_monitoring_graph(cm, ((struct context *)arg)->axctx, ((struct context *)arg)->cyctx->nomap[0].node_contact_list, 
			((struct context *)arg)->cyctx->nomap[0].node_coord_contact_list, 111, 0, 111, metric_list_ptr, func_list_ptr->num_recs, 
			func_list_ptr->func_recs);
			printf("Finally out of  load monitoring graph call ... \n");
			free(func_list_ptr);
			free(metric_list_ptr);
			break;
		} 
		sleep(30);
	} while(1);
	pthread_exit(NULL);
}

static void update_coord_node_monitors_data(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("Node updating coordinator that it deployed the %s monitor at node : %s ...\n", 
	(((struct node_monitors_deploy *)msg)->mon_endpoints)->mon_attr, ((struct node_monitors_deploy *)msg)->node_contact_list);

	register_monitors_to_axon(((struct context *)client_data)->axctx, ((struct node_monitors_deploy *)msg)->node_contact_list, 
	(((struct node_monitors_deploy *)msg)->mon_endpoints)->mon_key, (((struct node_monitors_deploy *)msg)->mon_endpoints)->mon_attr, 
	(((struct node_monitors_deploy *)msg)->stone_endpoints)->split_stone, (((struct node_monitors_deploy *)msg)->stone_endpoints)->split_action, 
	(((struct node_monitors_deploy *)msg)->func_endpoints)->out_format); 
}

void populate_node_monitors_deploy_msg(struct node_monitors_deploy *node_monitors_deploy_data, char *node_coord_contact_list, 
char *node_contact_list, struct stone_context *stone_ctx, struct func_context *func_ctx, struct mon_context *mon_ctx) {
	node_monitors_deploy_data->node_contact_list = strdup(node_contact_list);
	node_monitors_deploy_data->node_type = 0;

	node_monitors_deploy_data->stone_endpoints = (struct stone_context *) calloc(1, sizeof(struct stone_context));
	node_monitors_deploy_data->stone_endpoints[0].stone_id = stone_ctx[0].stone_id;
	node_monitors_deploy_data->stone_endpoints[0].stone_action = stone_ctx[0].stone_action;
	node_monitors_deploy_data->stone_endpoints[0].split_stone = stone_ctx[0].split_stone;
	node_monitors_deploy_data->stone_endpoints[0].split_action = stone_ctx[0].split_action;

	node_monitors_deploy_data->func_endpoints = (struct func_context *) calloc(1, sizeof(struct func_context));
	node_monitors_deploy_data->func_endpoints[0].func_name = strdup(func_ctx[0].func_name);
	node_monitors_deploy_data->func_endpoints[0].in_format = strdup(func_ctx[0].in_format);
	node_monitors_deploy_data->func_endpoints[0].out_format = strdup(func_ctx[0].out_format);
	node_monitors_deploy_data->func_endpoints[0].func_order = func_ctx[0].func_order;
	node_monitors_deploy_data->func_endpoints[0].func_level = func_ctx[0].func_level;

	node_monitors_deploy_data->mon_endpoints = (struct mon_context *) calloc(1, sizeof(struct mon_context));
	node_monitors_deploy_data->mon_endpoints[0].mon_key = mon_ctx[0].mon_key;
	node_monitors_deploy_data->mon_endpoints[0].mon_attr = mon_ctx[0].mon_attr;
	node_monitors_deploy_data->mon_endpoints[0].mon_type = mon_ctx[0].mon_type;
	node_monitors_deploy_data->mon_endpoints[0].mon_resource_type = mon_ctx[0].mon_resource_type;
}

void msg_coord_for_monitors_streams(char *node_contact_list, char *node_coord_contact_list, struct stone_context *stone_ctx, 
struct func_context *func_ctx, struct mon_context *mon_ctx, struct context *ctx) {
	printf("Sending message to the coordinator about monitoring data stream ........ \n");
	attr_list coord_attr_list = attr_list_from_string(node_coord_contact_list);

	CMConnection conn = CMget_conn(cm, coord_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(coord_attr_list);
		printf("\n");
		exit(1);
	}
	node_monitors_deploy_format = CMregister_format(cm, node_monitors_deploy_format_list);
	CMregister_handler(node_monitors_deploy_format, update_coord_node_monitors_data, ctx);
	
	node_monitors_deploy node_monitors_deploy_data;
	populate_node_monitors_deploy_msg(&node_monitors_deploy_data, node_coord_contact_list, node_contact_list, stone_ctx, func_ctx, mon_ctx);
	CMwrite(conn, node_monitors_deploy_format, &node_monitors_deploy_data);
}

void *libcurl_init(void *arg) {
	do {
		libcurl_init("http://10.0.0.111/server-status?auto");
		sleep(5);
	} while(1);
	pthread_exit(NULL);
}

void *libvirt_start(void *arg) {
	do {
		libvirt_init();
		libvirt_open();
		sleep(1200);
		libvirt_close();
	} while(1);
	pthread_exit(NULL);
}

void *boot_monitors(void *arg) {
	do {
		if(check_is_node_control_comps_assigned(cyctx, cyctx->nomap[0].node_contact_list)) {
			printf("Starting monitoring data structures ........ \n");
			struct stone_context *stone_ctx = (struct stone_context *) calloc(1, sizeof(struct stone_context));
			init_stone_context(1, stone_ctx);
			struct func_context *func_ctx = (struct func_context *) calloc(1, sizeof(struct func_context));
			init_func_context(1, func_ctx);
			struct mon_context *mon_ctx = (struct mon_context *) calloc(1, sizeof(struct mon_context));
			init_mon_context(1, mon_ctx);

			attr_list mon_attr_list = create_attr_list();
			/*
			attr_list mon_attr_list = create_attr_list();
			atom_t mon_metric_sec_rate_atom = -1;
			atom_t mon_metric_usec_rate_atom = -1;
			set_int_attr(mon_attr_list, mon_metric_sec_rate_atom, 5);
			set_int_attr(mon_attr_list, mon_metric_usec_rate_atom, 0);
			*/

			/*
			CMregister_handler(node_monitors_deploy_format, update_coord_node_monitors_data, ((struct context *)arg));
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "cpu_nice", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "cpu_user", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "cpu_system", MON_COPY, cyctx->nomap[0].node_contact_list,
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "cpu_idle", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "mem_buffers", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "mem_free", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "mem_cached", MON_COPY,
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "swap_free", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/

			/*
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "load_one", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "load_five", MON_COPY, cyctx->nomap[0].node_contact_list, 
			cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "load_fifteen", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/
			
			/*
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_req_rate", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_bytes_rate", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_busy_workers", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_idle_workers", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_web_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_serv_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "apache_num_vcpus", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/

			/*
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_dom0_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/	
			
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_web_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_serv_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_db_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_web_iftxpackets", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_serv_iftxpackets", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "rubis_db_iftxpackets", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			/*
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_web_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_serv_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_db_ctotaltime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/

			/*
			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_web_req_rate", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_serv_req_count", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);

			create_monitors_stream(cm, ((struct context *)arg), "base-perf", "node_resource", "entropy_serv_req_ptime", MON_COPY, 
			cyctx->nomap[0].node_contact_list, cyctx->nomap[0].node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, mon_attr_list);
			*/

			/*
			create_monitors_stream(cm, "base-perf", "node_resource", "cpu_min_freq", stone_ctx, func_ctx, mon_ctx);
			create_monitors_stream(cm, "base-perf", "node_resource", "cpu_max_freq", stone_ctx, func_ctx, mon_ctx);
			create_monitors_stream(cm, "base-perf", "node_resource", "cpu_cur_freq", stone_ctx, func_ctx, mon_ctx);
			*/

			free(stone_ctx);
			free(func_ctx);
			free(mon_ctx);
			break;
		}
		sleep(2);
	} while (1);
	pthread_exit(NULL);
}

static void update_node_control_comps(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	update_local_node_state_from_coord((struct cyton *)client_data, (struct control_comp *)msg);
}
 
void populate_node_ctrl_info(struct axon *axctx, struct control_comp *ctrl_comp, char *node_coord_contact_list, char *node_contact_list) {
	// For now it just copies the coordinator as the one which is also aggregator and actuator
	// But this can work otherwise also, with coordinator now looking into some of its own 'axon' state and getting the info
	ctrl_comp->node_contact_list = strdup(node_contact_list); 
	ctrl_comp->node_coord_contact_list = strdup(node_coord_contact_list); 
	ctrl_comp->node_aggr_contact_list = strdup(node_coord_contact_list); 
	ctrl_comp->node_actu_contact_list = strdup(node_coord_contact_list); 
}

static void msg_node_control_comps(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	attr_list node_attr_list = attr_list_from_string(((struct node_boot *)msg)->node_contact_list);

	CMConnection outconn = CMget_conn(cm, node_attr_list);
	if (outconn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(node_attr_list);
		printf("\n");
		exit(1);
	}
	control_info_format = CMregister_format(cm, control_format_list);
	
	control_comp ctrl_comp;
	populate_node_ctrl_info((struct axon *)client_data, &ctrl_comp, ((struct node_boot *)msg)->bootserver_contact_list, 
	((struct node_boot *)msg)->node_contact_list);
	CMwrite(outconn, control_info_format, &ctrl_comp);
}
	
void populate_node_info_msg(struct node_boot *node_info, char *node_coord_contact_list, char *node_contact_list) {
	node_info->is_control = 1; 
	node_info->bootserver_contact_list = strdup(node_coord_contact_list); 
	node_info->node_contact_list = strdup(node_contact_list); 
}

void msg_coord_for_control_comps(char *node_coord_contact_list, char *node_contact_list) {
	attr_list coord_attr_list = attr_list_from_string(node_coord_contact_list);

	CMConnection conn = CMget_conn(cm, coord_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(coord_attr_list);
		printf("\n");
		exit(1);
	}
	node_info_format = CMregister_format(cm, node_info_format_list);
	
	node_boot node_info;
	populate_node_info_msg(&node_info, node_coord_contact_list, node_contact_list);
	CMwrite(conn, node_info_format, &node_info);
}

void populate_node_live_msg(struct node_boot *nodelive_data, char *node_coord_contact_list, char *node_contact_list) {
	nodelive_data->is_control = 1; 
	nodelive_data->bootserver_contact_list = strdup(node_coord_contact_list); 
	nodelive_data->node_contact_list = strdup(node_contact_list); 
}

void msg_coord_for_live_node(char *node_coord_contact_list, char *node_contact_list) {
	attr_list coord_attr_list = attr_list_from_string(node_coord_contact_list);

	CMConnection conn = CMget_conn(cm, coord_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(coord_attr_list);
		printf("\n");
		exit(1);
	}
	live_info_format = CMregister_format(cm, live_info_format_list);
	
	node_boot nodelive_data;
	populate_node_live_msg(&nodelive_data, node_coord_contact_list, node_contact_list);
	printf("Sending node live flag \"%d\"\n", nodelive_data.is_control);
	CMwrite(conn, live_info_format, &nodelive_data);
}

static void update_coord_node(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf(" Child updating coordinator that its live now ....\n");
	add_coord_node_child_state((struct axon *)client_data, ((struct node_boot *)msg));
	/* To be done later - each node - gets there queries from coord for deployment
	pthread_t thrQuery;
	int rcQuery = pthread_create(&thrQuery, NULL, deploy_base_query, ctx);	
	if (rcQuery) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rcQuery);
		exit(-1);
	}*/
}
	
static void update_node(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("Got Message from BootServer ...\n");
	update_local_node_state_from_boot((struct cyton *)client_data, ((struct ret_node_coord_map *)msg));
	if(!check_is_node_control_comps_assigned((struct cyton *)client_data, ((struct ret_node_coord_map *)msg)->node_contact_list)) {
		if(check_is_node_coord_assigned((struct cyton *)client_data, ((struct ret_node_coord_map *)msg)->node_contact_list)) {
			if(!check_is_node_coord((struct cyton *)client_data, ((struct ret_node_coord_map *)msg)->node_contact_list)) {
				msg_coord_for_control_comps(((struct ret_node_coord_map *)msg)->node_coord_contact_list, 
				((struct ret_node_coord_map *)msg)->node_contact_list);
			}
			// Assuming coordinator gets always all the control components somehow from bootserver
			// If not it can ask bootserver again by sending another message
		}
	} // Till here everything is about node local state, updating its control components
	if(!check_is_node_coord((struct cyton *)client_data, ((struct ret_node_coord_map *)msg)->node_contact_list)) {
		msg_coord_for_live_node(((struct ret_node_coord_map *)msg)->node_coord_contact_list, 
		((struct ret_node_coord_map *)msg)->node_contact_list);
	} else {
		// Assuming coordinator gets always all the control components info i.e. coordinator, aggregator and actuators.
		init_coord_node_state(cyctx, axctx);
	}
}

void populate_node_ctrl_rec(struct node_boot *bootup_data, char *bootserver_cm_contact_list, char *node_cm_contact_list) {
	bootup_data->is_control = 1; 
	bootup_data->bootserver_contact_list = strdup(bootserver_cm_contact_list); 
	bootup_data->node_contact_list = strdup(node_cm_contact_list); 
}
	
void boot_node(char *bootserver_cm_contact_list, char *node_cm_contact_list, CManager cm) {
	attr_list bootserver_cm_attr_list = attr_list_from_string(bootserver_cm_contact_list);

	CMConnection conn = CMget_conn(cm, bootserver_cm_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(bootserver_cm_attr_list);
		printf("\n");
		exit(1);
	}
	boot_in_format = CMregister_format(cm, in_format_list);
	
	node_boot bootup_data;
	populate_node_ctrl_rec(&bootup_data, bootserver_cm_contact_list, node_cm_contact_list);
	printf("Sending local node info to bootserver \"%s\"\n", bootserver_cm_contact_list);
	CMwrite(conn, boot_in_format, &bootup_data);
}

/*
void export_dynamic_code(char *export_api, char *export_name) {
	int i; int j;
	strcpy(extern_string, export_api);
	for(i = 0; i < strlen(export_name); i++) {
		strcat(export_me, export_name[j]);
		strcat(export_me, ",");
		strcat(export_me, "(void *) 0");
		externs[j] = export_me;
	}
	for(j = 0; j < strlen(export_name); j++) {
		externs[j].extern_value = (void *) (long) export_name[j];
	}
	strcpy(externs, export_name);
	EVadd_standard_routines(cm, extern_string, externs);
}*/

int main(int argc, char *argv[]) {
    attr_list node_contact_list = NULL;
    gen_pthread_init();
    cm = CManager_create();

    attr_list listen_list = NULL;
	char *node_cm_contact_list;
    CMlisten_specific(cm, listen_list);
    node_contact_list = CMget_contact_list(cm);
    if (node_contact_list) {
		node_cm_contact_list = attr_list_to_string(node_contact_list);
		free_attr_list(node_contact_list);
    }

	printf("Local node CM contact list \"%s\"\n", node_cm_contact_list);

	ctx = (struct context *) calloc(1, sizeof(struct context));
	cyctx = create_cyton_context();
	axctx = create_axon_context();

	ctx->cyctx = cyctx;
	ctx->axctx = axctx;

	boot_out_format = CMregister_format(cm, out_format_list);
	CMregister_handler(boot_out_format, update_node, cyctx);	
	
	live_info_format = CMregister_format(cm, live_info_format_list);
	CMregister_handler(live_info_format, update_coord_node, axctx);

	node_info_format = CMregister_format(cm, node_info_format_list);
	CMregister_handler(node_info_format, msg_node_control_comps, axctx);
	
	control_info_format = CMregister_format(cm, control_format_list);
	CMregister_handler(control_info_format, update_node_control_comps, cyctx);

	node_deploy_format = CMregister_format(cm, node_deploy_format_list);
	CMregister_handler(node_deploy_format, deploy_funcs, axctx);

	node_graph_deploy_format = CMregister_format(cm, node_graph_deploy_format_list);
	CMregister_handler(node_graph_deploy_format, update_coord_node_graph_data, axctx);

	node_monitors_deploy_format = CMregister_format(cm, node_monitors_deploy_format_list);
	CMregister_handler(node_monitors_deploy_format, update_coord_node_monitors_data, ctx);

	local_agent_term_format = CMregister_format(cm, local_term_format_list);
	CMregister_handler(local_agent_term_format, local_exec_func, ctx);

	//extern_format = CMregister_format(cm, extern_format_list);
	//CMregister_handler(extern_format, export_dynamic_code, ctx);

    static char extern_string[] = "\
		void libvirt_init();\n\
		void libvirt_open();\n\
		void libvirt_close();\n\
		int libvirt_num_domains();\n\
		int libvirt_num_active_domains();\n\
		int libvirt_num_inactive_domains();\n\
		void *libvirt_get_domain_info(char *domain_name);\n\
		void *libvirt_get_domain_disk_info(char *domain_name);\n\
		void *libvirt_get_domain_interface_info(char *domain_name);\n\
		int libvirt_domain_exists(char *domain_name);\n\
		int libvirt_get_domain_id(char *domain_name);\n\
		unsigned char libvirt_get_domain_state(char *domain_name);\n\
		unsigned long libvirt_get_domain_mem_max(char *domain_name);\n\
		unsigned long libvirt_get_domain_mem_used(char *domain_name);\n\
		unsigned short libvirt_get_domain_num_vcpus(char *domain_name);\n\
		unsigned long libvirt_get_domain_cpu_total_time(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_rreq(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_wreq(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_rbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_wbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxpackets(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txpackets(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxerrors(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txerrors(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxdrops(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txdrops(char *domain_name);\n\
		int libvirt_set_vcpus(char *domain_name, int num_vcpus);\n\
		int libvirt_set_memory(char *domain_name, unsigned long mem_kbs);\n\
		int libvirt_create_domain(char *xmlDesc);\n\
		int system(char *command);\n";

    static cod_extern_entry externs[] = {
	{"libvirt_init", (void *) 0},						// 0
	{"libvirt_open", (void *) 0},						// 1
	{"libvirt_close", (void *) 0},						// 2
	{"libvirt_num_domains", (void *) 0},				// 3
	{"libvirt_num_active_domains", (void *) 0},			// 4
	{"libvirt_num_inactive_domains", (void *) 0},		// 5
	{"libvirt_get_domain_info", (void *) 0},			// 6
	{"libvirt_get_domain_disk_info", (void *) 0},		// 7	
	{"libvirt_get_domain_interface_info", (void *) 0},  // 8
	{"libvirt_domain_exists", (void *) 0}, 				// 9
	{"libvirt_get_domain_id", (void *) 0},				// 10
	{"libvirt_get_domain_state", (void *) 0}, 			// 11
	{"libvirt_get_domain_mem_max", (void *) 0}, 		// 12
	{"libvirt_get_domain_mem_used", (void *) 0}, 		// 13
	{"libvirt_get_domain_num_vcpus", (void *) 0},		// 14
	{"libvirt_get_domain_cpu_total_time", (void *) 0},	// 15
	{"libvirt_get_domain_vblock_rreq", (void *) 0},		// 16
	{"libvirt_get_domain_vblock_wreq", (void *) 0},		// 17
	{"libvirt_get_domain_vblock_rbytes", (void *) 0},	// 18
	{"libvirt_get_domain_vblock_wbytes", (void *) 0},	// 19
	{"libvirt_get_domain_if_rxbytes", (void *) 0},		// 20
	{"libvirt_get_domain_if_txbytes", (void *) 0},		// 21
	{"libvirt_get_domain_if_rxpackets", (void *) 0},	// 22
	{"libvirt_get_domain_if_txpackets", (void *) 0},	// 23
	{"libvirt_get_domain_if_rxerrors", (void *) 0}, 	// 24
	{"libvirt_get_domain_if_txerrors", (void *) 0},		// 25
	{"libvirt_get_domain_if_rxdrops", (void *) 0}, 		// 26
	{"libvirt_get_domain_if_txdrops", (void *) 0},		// 27
	{"libvirt_set_vcpus", (void *) 0},					// 28
	{"libvirt_set_memory", (void *) 0},					// 29
	{"libvirt_create_domain", (void *) 0},				// 30
	{"libvirt_shutdown_domain", (void *)0},				// 31
	{"system", (void *) 0},								// 32
	{(char *) 0, (void *) 0}
    };

    /*
     * some compilers think it isn't a static initialization to put this
     * in the structure above, so do it explicitly.
     */
    externs[0].extern_value = (void *) (long) libvirt_init;
    externs[1].extern_value = (void *) (long) libvirt_open;
    externs[2].extern_value = (void *) (long) libvirt_close;
    externs[3].extern_value = (void *) (long) libvirt_num_domains;
    externs[4].extern_value = (void *) (long) libvirt_num_active_domains;
    externs[5].extern_value = (void *) (long) libvirt_num_inactive_domains;
    externs[6].extern_value = (void *) (long) libvirt_get_domain_info;
    externs[7].extern_value = (void *) (long) libvirt_get_domain_disk_info;
    externs[8].extern_value = (void *) (long) libvirt_get_domain_interface_info;
    externs[9].extern_value = (void *) (long) libvirt_domain_exists;
    externs[10].extern_value = (void *) (long) libvirt_get_domain_id;
    externs[11].extern_value = (void *) (long) libvirt_get_domain_state;
    externs[12].extern_value = (void *) (long) libvirt_get_domain_mem_max;
    externs[13].extern_value = (void *) (long) libvirt_get_domain_mem_used;
    externs[14].extern_value = (void *) (long) libvirt_get_domain_num_vcpus;
    externs[15].extern_value = (void *) (long) libvirt_get_domain_cpu_total_time;
    externs[16].extern_value = (void *) (long) libvirt_get_domain_vblock_rreq;
    externs[17].extern_value = (void *) (long) libvirt_get_domain_vblock_wreq;
    externs[18].extern_value = (void *) (long) libvirt_get_domain_vblock_rbytes;
    externs[19].extern_value = (void *) (long) libvirt_get_domain_vblock_wbytes;
    externs[20].extern_value = (void *) (long) libvirt_get_domain_if_rxbytes;
    externs[21].extern_value = (void *) (long) libvirt_get_domain_if_txbytes;
    externs[22].extern_value = (void *) (long) libvirt_get_domain_if_rxpackets;
    externs[23].extern_value = (void *) (long) libvirt_get_domain_if_txpackets;
    externs[24].extern_value = (void *) (long) libvirt_get_domain_if_rxerrors;
    externs[25].extern_value = (void *) (long) libvirt_get_domain_if_txerrors;
    externs[26].extern_value = (void *) (long) libvirt_get_domain_if_rxdrops;
    externs[27].extern_value = (void *) (long) libvirt_get_domain_if_txdrops;
    externs[28].extern_value = (void *) (long) libvirt_set_vcpus;
    externs[29].extern_value = (void *) (long) libvirt_set_memory;
    externs[30].extern_value = (void *) (long) libvirt_create_domain;
    externs[31].extern_value = (void *) (long) libvirt_shutdown_domain;
    externs[32].extern_value = (void *) (long) system;

	EVadd_standard_routines(cm, extern_string, externs);
	boot_node(argv[1], node_cm_contact_list, cm);
	//boot_trace_stone(node_cm_contact_list, cm);

	/*
	pthread_t thrlibcurl;
	int rclibcurl = pthread_create(&thrlibcurl, NULL, libcurl_init, ctx);	
	if (rclibcurl) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rclibcurl);
        exit(-1);
	}*/
	
	/*
	pthread_t thrlibvirt;
	int rclibvirt = pthread_create(&thrlibvirt, NULL, libvirt_start, ctx);	
	if (rclibvirt) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rclibvirt);
        exit(-1);
	}*/

	pthread_t thrMonitors;
	int rcMonitors = pthread_create(&thrMonitors, NULL, boot_monitors, ctx);	
	if (rcMonitors) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rcMonitors);
        exit(-1);
	}

	/*
	pthread_t thrLoadBase;
	int rcLoadBase = pthread_create(&thrLoadBase, NULL, deploy_load_mon_query, ctx);	
	if (rcLoadBase) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rcLoadBase);
		exit(-1);
	}*/

	/*
	pthread_t thrFaultBase;
	int rcFaultBase = pthread_create(&thrFaultBase, NULL, deploy_fault_mon_query, ctx);	
	if (rcFaultBase) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rcFaultBase);
		exit(-1);
	}*/
	
	CMsleep(cm, 1);
	CMrun_network(cm);
	return 0;
}
