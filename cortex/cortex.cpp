#include "cortex.h"

static FMField sensor_field_list[] = 
{
	{"timestamp", "double", sizeof(double), FMOffset(sensor_rec_ptr, timestamp)},
	{"attr_name", "string", sizeof(char*), FMOffset(sensor_rec_ptr, attr_name)},
	{"rec_val", "float", sizeof(float), FMOffset(sensor_rec_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec sensor_format_list[] = 
{
    {"sensor", sensor_field_list, sizeof(sensor_rec), NULL},
    {NULL, NULL}
};

char *get_cpu_nice_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_nice\";\n\
\toutput.rec_val = cpu_nice_func();\n\
}";

char *get_cpu_user_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_user\";\n\
\toutput.rec_val = cpu_user_func();\n\
}";

char *get_cpu_system_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_system\";\n\
\toutput.rec_val = cpu_system_func();\n\
}";

char *get_cpu_idle_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_idle\";\n\
\toutput.rec_val = cpu_idle_func();\n\
}";

char *get_mem_buffers_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"mem_buffers\";\n\
\toutput.rec_val = mem_buffers_func();\n\
}";

char *get_mem_free_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"mem_free\";\n\
\toutput.rec_val = mem_free_func();\n\
}";

char *get_mem_cached_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"mem_cached\";\n\
\toutput.rec_val = mem_cached_func();\n\
}";

char *get_swap_free_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"swap_free\";\n\
\toutput.rec_val = swap_free_func();\n\
}";

char *get_load_one_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"load_one\";\n\
\toutput.rec_val = load_one_func();\n\
}";

char *get_load_five_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"load_five\";\n\
\toutput.rec_val = load_five_func();\n\
}";

char *get_load_fifteen_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"load_fifteen\";\n\
\toutput.rec_val = load_fifteen_func();\n\
}";

char *get_cpu_min_freq_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_min_freq\";\n\
\toutput.rec_val = cpu_min_freq_func();\n\
}";

char *get_cpu_max_freq_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_max_freq\";\n\
\toutput.rec_val = cpu_max_freq_func();\n\
}";

char *get_cpu_cur_freq_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"cpu_cur_freq\";\n\
\toutput.rec_val = cpu_cur_freq_func();\n\
}";
// action = filter/tranform/route  stones = split, filter, tranform, multiqueue, route, storage
// action_type = add/modify/delete action/cyton = metric, group, collect, sample, analyze

/*void update_coord_group_state(struct context *ctx, struct nodes_mongraph_reconf_rec nodes_mongraph_reconf_rec, EVstone
metric_mon_split_stone, EVaction metric_mon_split_action, char *mon_metric_out_format, char *mon_metric) {

}

void update_coord_monitors_state(struct context *ctx, struct nodes_mongraph_reconf_rec nodes_mongraph_reconf_rec, EVstone
metric_mon_split_stone, EVaction metric_mon_split_action, char *mon_metric_out_format, char *mon_metric) {

}*/

void remote_create_event_collect_stream(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	//Transform
	EVstone vstep_stone; EVaction vstep_action; 
	EVstone vsplit_stone; EVaction vsplit_action;
	char *event_collect_func_name = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].func_name);
	char *event_collect_in_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].in_format);
	char *event_collect_out_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].out_format);

	do {
		vstep_stone = REValloc_stone(rconn);
		FMStructDescRec *event_collect_in_format_list = get_format_list(event_collect_in_format);
		FMStructDescRec *event_collect_out_format_list = get_format_list(event_collect_out_format);
		char *vstep_action_spec = create_transform_action_spec(event_collect_in_format_list, event_collect_out_format_list, event_collect_func_name);
		vstep_action = REVassoc_immediate_action (rconn, vstep_stone, vstep_action_spec);

		do {
			EVstone *targets = NULL;
			vsplit_stone = REValloc_stone (rconn);
			vsplit_action = REVassoc_split_action(rconn, vsplit_stone, targets);
			REVaction_set_output(rconn, vstep_stone, vstep_action, vsplits - 1, vsplit_stone);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_action = vsplit_action;
			vsplits = vsplits - 1;
		} while (vsplits > 0);

		// Router 
		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id == -1) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
		}

		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name == NULL) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name = strdup(event_collect_func_name);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].in_format = strdup(event_collect_in_format);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].out_format = strdup(event_collect_out_format);
		}

		pcopies = pcopies - 1;
	} while (pcopies > 0);
}

void reconf_event_collect_graph(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs = (struct split_context_list *) calloc(1, sizeof(struct split_context_list));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx_num = vsplits;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx = (struct split_context *) calloc(vsplits, sizeof(struct split_context));
	init_split_context(vsplits, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx);
	
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx = (struct stone_context *) calloc(pcopies, sizeof(struct stone_context));
	init_stone_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx);

	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx = (struct func_context *) calloc(pcopies, sizeof(struct func_context));
	init_func_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx);

	//remote_create_group_stream(rconn, ctx, pcopies, vsplits, node_mongraph_reconf_rec);
	remote_create_event_collect_stream(rconn, pcopies, vsplits, node_mongraph_reconf_rec);
}

void remote_create_time_collect_stream(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	//Transform
	EVstone vstep_stone; EVaction vstep_action; 
	EVstone vsplit_stone; EVaction vsplit_action;
	char *time_collect_func_name = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].func_name);
	char *time_collect_in_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].in_format);
	char *time_collect_out_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].out_format);

	do {
		vstep_stone = REValloc_stone(rconn);
		FMStructDescRec *time_collect_in_format_list = get_format_list(time_collect_in_format);
		FMStructDescRec *time_collect_out_format_list = get_format_list(time_collect_out_format);
		char *vstep_action_spec = create_transform_action_spec(time_collect_in_format_list, time_collect_out_format_list, time_collect_func_name);
		vstep_action = REVassoc_immediate_action (rconn, vstep_stone, vstep_action_spec);

		do {
			EVstone *targets = NULL;
			vsplit_stone = REValloc_stone (rconn);
			vsplit_action = REVassoc_split_action(rconn, vsplit_stone, targets);
			REVaction_set_output(rconn, vstep_stone, vstep_action, vsplits - 1, vsplit_stone);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_action = vsplit_action;
			vsplits = vsplits - 1;
		} while (vsplits > 0);

		// Router 
		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id == -1) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
		}

		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name == NULL) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name = strdup(time_collect_func_name);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].in_format = strdup(time_collect_in_format);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].out_format = strdup(time_collect_out_format);
		}

		pcopies = pcopies - 1;
	} while (pcopies > 0);
}

void reconf_time_collect_graph(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs = (struct split_context_list *) calloc(1, sizeof(struct split_context_list));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx_num = vsplits;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx = (struct split_context *) calloc(vsplits, sizeof(struct split_context));
	init_split_context(vsplits, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx);
	
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx = (struct stone_context *) calloc(pcopies, sizeof(struct stone_context));
	init_stone_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx);

	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx = (struct func_context *) calloc(pcopies, sizeof(struct func_context));
	init_func_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx);

	//remote_create_group_stream(rconn, ctx, pcopies, vsplits, node_mongraph_reconf_rec);
	remote_create_time_collect_stream(rconn, pcopies, vsplits, node_mongraph_reconf_rec);
}

void remote_create_group_stream(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	//Router
	EVstone vstep_stone; EVaction vstep_action; 
	EVstone vsplit_stone; EVaction vsplit_action;
	char *group_func_name = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].func_name);
	char *group_in_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[0].func_ctx[2].in_format);

	do {
		vstep_stone = REValloc_stone(rconn);
		FMStructDescRec *group_in_format_list = get_format_list(group_in_format);
		char *vstep_action_spec = create_router_action_spec(group_in_format_list, group_func_name);
		vstep_action = REVassoc_immediate_action (rconn, vstep_stone, vstep_action_spec);

		do {
			EVstone *targets = NULL;
			vsplit_stone = REValloc_stone (rconn);
			vsplit_action = REVassoc_split_action(rconn, vsplit_stone, targets);
			REVaction_set_output(rconn, vstep_stone, vstep_action, vsplits - 1, vsplit_stone);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx[vsplits - 1].split_action = vsplit_action;
			vsplits = vsplits - 1;
		} while (vsplits > 0);

		// Router 
		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id == -1) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
		}

		if(node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx != NULL && 
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name == NULL) {
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].func_name = strdup(group_func_name);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].in_format = strdup(group_in_format);
			node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx[pcopies - 1].out_format = strdup(group_in_format);
		}

		pcopies = pcopies - 1;
	} while (pcopies > 0);
}

void reconf_group_graph(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	node_mongraph_reconf_rec.reconf_out_ctx = (struct reconf_cyton_context *) calloc(1, sizeof(struct reconf_cyton_context));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs = (struct split_context_list *) calloc(1, sizeof(struct split_context_list));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx_num = vsplits;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx = (struct split_context *) calloc(vsplits, sizeof(struct split_context));
	init_split_context(vsplits, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_split_recs[0].split_ctx);
	
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs = (struct stone_context_list *) calloc(1, sizeof(struct stone_context_list));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx = (struct stone_context *) calloc(pcopies, sizeof(struct stone_context));
	init_stone_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_stone_recs[0].stone_ctx);

	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs = (struct func_context_list *) calloc(1, sizeof(struct func_context_list));
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx_num = pcopies;
	node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx = (struct func_context *) calloc(pcopies, sizeof(struct func_context));
	init_func_context(pcopies, node_mongraph_reconf_rec.reconf_out_ctx[0].reconf_func_recs[0].func_ctx);

	//remote_create_group_stream(rconn, ctx, pcopies, vsplits, node_mongraph_reconf_rec);
	remote_create_group_stream(rconn, pcopies, vsplits, node_mongraph_reconf_rec);
}

void remote_create_monitors_stream(CMConnection rconn, char *mon_metric, int pcopies, int vsplits, 
struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	char *mon_metric_func_name = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[1].func_ctx[2].func_name);
	char *mon_metric_out_format = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[1].func_ctx[2].out_format);
	do {
		EVstone mon_metric_split_stone = REValloc_stone (rconn);
		EVaction mon_metric_split_action = REVassoc_split_action(rconn, mon_metric_split_stone, NULL);

		EVstone mon_metric_stone = REValloc_stone (rconn);
		FMStructDescRec *metric_out_format_list = get_format_list(mon_metric_out_format);
		char *mon_action_spec = create_transform_action_spec(NULL, metric_out_format_list, mon_metric_func_name);
		EVaction mon_metric_action = REVassoc_immediate_action (rconn, mon_metric_stone, mon_action_spec);
		REVaction_set_output(rconn, mon_metric_stone, mon_metric_action, 0, mon_metric_split_stone);
		printf(" Metric : %s num %d deployed with stone id : %d and stone action id : %d \n", mon_metric, pcopies, mon_metric_stone, 
		mon_metric_action);
		printf(" Metric : %s num %d exporting split stone id : %d and split action id : %d \n", mon_metric, pcopies, mon_metric_split_stone, 
		mon_metric_split_action);

		int sample_sec = 5; int sample_usec = 0;
		REVenable_auto_stone(rconn, mon_metric_stone, sample_sec, sample_usec);
		printf(" Metric : %s num %d monitoring started with sample_sec : %d and sample_usec : %d \n", mon_metric, pcopies, sample_sec, sample_usec);
		
		//update_coord_monitors_state(ctx, nodes_mongraph_reconf_rec, metric_mon_split_stone, metric_mon_split_action,
		//node_mongraph_reconf_rec->reconf_in_ctx[0].reconf_func_recs[1].out_format, mon_metric);

		pcopies = pcopies - 1;
		sleep(1);	
	} while (pcopies > 0);
}

void reconf_metric_graph(CMConnection rconn, int pcopies, int vsplits, struct node_mongraph_reconf_rec node_mongraph_reconf_rec) {
	char *mon_metric_func = NULL;
	char *mon_metric = strdup(node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[1].func_ctx[2].func_name);

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

	node_mongraph_reconf_rec.reconf_in_ctx[0].reconf_func_recs[1].func_ctx[2].func_name = strdup(mon_metric_func);
	//remote_create_monitors_stream(rconn, ctx, mon_metric, pcopies, vsplits, node_mongraph_reconf_rec);
	remote_create_monitors_stream(rconn, mon_metric, pcopies, vsplits, node_mongraph_reconf_rec);
}

void reconf_mon_graph(CManager cm, int pcopies, int vsplits, struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs) {
	if(nodes_mongraph_reconfs->node_mongraph_reconf_num == 0) {
		printf("No reconfigurations to be done ..");
		exit(1);
	}

	int i;
	for (i = 0; i < nodes_mongraph_reconfs->node_mongraph_reconf_num; i++) {
		int qid = nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].qid;
		//nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].node_contact_list = strdup(node_contact_list); 
		attr_list node_attr_list = attr_list_from_string(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].node_contact_list);
		CMConnection rconn = CMget_conn(cm, node_attr_list);
		if(!rconn) {
			printf("Connection failed .. \n ");
		} 
		
		if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 1) { // metric
			reconf_metric_graph(rconn, pcopies, vsplits, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i]);
		} else if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 2) { // group
			reconf_group_graph(rconn, pcopies, vsplits, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i]);
		} else if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 31) { // collect
			reconf_time_collect_graph(rconn, pcopies, vsplits, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i]);
		} else if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 32) { // collect
			reconf_event_collect_graph(rconn, pcopies, vsplits, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i]);
		} else if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 4) { // sample
			//reconf_sample_graph(rconn, pcopies, vsplits, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i]);
		}
	}
}

/*void start_reconf() {
	if(nodes_mongraph_reconfs->node_mongraph_reconf_num > 0)
		continue;

	int i;
	for (i = 0; i < nodes_mongraph_reconfs->node_mongraph_reconf_num; i++) {
		int qid = nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].qid;
		attr_list node_attr_list = attr_list_from_string(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].node_contact_list);
		CMconnection conn = CM_get_conn(node_attr_list);
		if(!conn) {
			printf("Connection failed .. \n ");
		} else {
			if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action_type == 1) {
				if(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].action == 1) {
					produce_reconf_out_context(nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].node_contact_list, qid
					nodes_mongraph_reconfs->node_mongraph_reconfs_recs[i].reconf_out_context);	

					reconf_mon_metric(conn, nodes_mongraph_reconfs->node_mongraph_reconf_recs[i].reconf_in_context,
					nodes_mongraph_reconfs->node_mongraph_reconfs_recs[i].reconf_out_context);
				}
			}
			REVcreate_bridge_action
		}
	}
}*/

void init_mongraph_reconf_recs(char *node_contact_list, int qid, int action, int action_type, int reconf_atoms, struct reconf_func_recs *reconf_recs, 
struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs) {
	nodes_mongraph_reconfs->node_mongraph_reconf_recs = (struct node_mongraph_reconf_rec *) calloc(reconf_atoms, 
	sizeof(struct node_mongraph_reconf_rec));

	int i; int h;
	for (h = 0; h < reconf_atoms; h++) {
		for(i = 0; i < reconf_recs->reconf_func_num; i++) {
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].node_contact_list = strdup(node_contact_list); 
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].qid = qid; 

			//action = 1 == add, = 2 == modify, = 3 == delete
			if(action > 0 && action_type != 0) {
				nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].action_type = action_type; //  
			}
			if(action_type > 0) { // 1 = metric, 2 = group, 3 = collect, 4 = sample
				nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].action = action;  
			}
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx = (struct reconf_cyton_context *) calloc(1, 
			sizeof(struct reconf_cyton_context));
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs = 
			(struct func_context_list *) calloc(1, sizeof(struct func_context_list));
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs->func_ctx_num =
			reconf_recs->reconf_func_num;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs->func_ctx =
			(struct func_context *) calloc(reconf_recs->reconf_func_num, sizeof(struct func_context));

			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs[0].func_ctx[i].func_name =
			strdup(reconf_recs[0].reconf_func_recs[i].func_name);
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs[0].func_ctx[i].in_format = 
			strdup(reconf_recs[0].reconf_func_recs[i].in_format);
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_func_recs[0].func_ctx[i].out_format = 
			strdup(reconf_recs[0].reconf_func_recs[i].out_format);

			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs = 
			(struct stone_context_list *) calloc(1, sizeof(struct stone_context_list));
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs->stone_ctx_num =
			reconf_recs->reconf_func_num;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs->stone_ctx =
			(struct stone_context *) calloc(reconf_recs->reconf_func_num, sizeof(struct stone_context));

			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs[0].stone_ctx[i].stone_id = -1;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs[0].stone_ctx[i].stone_action = -1;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs[0].stone_ctx[i].split_stone = -1;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_stone_recs[0].stone_ctx[i].split_action = -1;

			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_split_recs = 
			(struct split_context_list *) calloc(1, sizeof(struct split_context_list));
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_split_recs->split_ctx_num =
			reconf_recs->reconf_func_num;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_split_recs->split_ctx =
			(struct split_context *) calloc(reconf_recs->reconf_func_num, sizeof(struct split_context));

			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_split_recs[0].split_ctx[i].split_stone = -1;
			nodes_mongraph_reconfs->node_mongraph_reconf_recs[h].reconf_in_ctx[0].reconf_split_recs[0].split_ctx[i].split_action = -1;
		}
	}
}

void init_mongraph_reconf(char *node_contact_list, int qid, int action, int action_type, int reconf_atoms, struct reconf_func_recs *reconf_recs) {
	if(!nodes_mongraph_reconfs) {
		nodes_mongraph_reconfs = (struct nodes_mongraph_reconf_recs *) calloc(1, sizeof(struct nodes_mongraph_reconf_recs));
		nodes_mongraph_reconfs->node_mongraph_reconf_num = 1;
		init_mongraph_reconf_recs(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs, nodes_mongraph_reconfs);
	} else {
		int reconf_num = nodes_mongraph_reconfs->node_mongraph_reconf_num;
		nodes_mongraph_reconfs = (struct nodes_mongraph_reconf_recs *) realloc(nodes_mongraph_reconfs, 1 * sizeof(struct nodes_mongraph_reconf_recs));	
		nodes_mongraph_reconfs->node_mongraph_reconf_num = reconf_num + 1;
		// This is wrong below needs change.do it later..
		init_mongraph_reconf_recs(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs, nodes_mongraph_reconfs);
	}
}

/*void populate_mongraph_reconf_rec(char *node_contact_list, int qid) {
	// struct nodes_mongraph_reconf_list *nodes_mongraph_reconfs = NULL;
	// Add a metric to the query - qid on node - node_contact_list;
	// If all part of same reconf keep 0, if all individual then -1 or NULL, if 1 then mark the complete symbol at the end

	struct reconf_func_context *reconf_func_ctx = (struct reconf_func_context *) calloc (1, sizeof(struct reconf_func_context));
	reconf_func_ctx->operator_func = "cpu_test_func";
	reconf_func_ctx->input_func = NULL;
	reconf_func_ctx->output_func = "base_group_func"; // With cpu, mem and load grouping
	// Start - Add metric, group, collection, sample
	init_mongraph_reconf(char *node_contact_list, int qid, 0, 1, 1, reconf_func_ctx);
	clean_reconf_func_context(1, reconf_func_ctx);

	init_mongraph_reconf(char *node_contact_list, int qid, 0, 2, 1, reconf_func_ctx);
	clean_reconf_func_context(1, reconf_func_ctx);

	init_mongraph_reconf(char *node_contact_list, int qid, 0, 3, 1, reconf_func_ctx);
	clean_reconf_func_context(1, reconf_func_ctx);

	init_mongraph_reconf(char *node_contact_list, int qid, 1, 4, 1, reconf_func_ctx);
	clean_reconf_func_context(1, reconf_func_ctx);
	free(reconf_func_ctx);
	// End - Add metric, group, collection, sample

	// Add only metrics
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 1, 1);

	// Add only group AIAAJTJ8o2QZQAAATkCmK11z4I=
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 2, 1);

	// Add only collection
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 3, 1);

	// Add only sample 
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 4, 1);

	// Add a new aggregator 
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 4, 1);

	// Move an existing aggregator 
	init_mongraph_reconf(char *node_contact_list, int qid, 1, 4, 1);
}*/

//struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs = NULL;
void add_metric(CManager cm, char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, int reconf_atoms, 
struct nodes_mongraph_reconf_recs *nodes_mongraph_reconfs, struct reconf_func_recs *reconf_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs);
	reconf_mon_graph(cm, pcopies, vsplits, nodes_mongraph_reconfs);
	free(nodes_mongraph_reconfs);
	//update_coord_monitors_state(ctx, nodes_mongraph_reconf_rec, metric_mon_split_stone, metric_mon_split_action,
	//nodes_mongraph_reconf_rec->reconf_in_ctx[0].reconf_func_recs[1].out_format, mon_metric);
}

void add_group(CManager cm, char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, int reconf_atoms, 
struct reconf_func_recs *reconf_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs);
	reconf_mon_graph(cm, pcopies, vsplits, nodes_mongraph_reconfs);
	free(nodes_mongraph_reconfs);
	//update_coord_group_state(ctx, nodes_mongraph_reconf_rec, metric_mon_split_stone, metric_mon_split_action,
	//nodes_mongraph_reconf_rec->reconf_in_ctx[0].reconf_func_recs[1].out_format, mon_metric);
}

void add_time_collect(CManager cm, char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, int reconf_atoms, 
struct reconf_func_recs *reconf_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs);
	reconf_mon_graph(cm, pcopies, vsplits, nodes_mongraph_reconfs);
}

void add_sample(CManager cm, char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, int reconf_atoms, 
struct reconf_func_recs *reconf_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_atoms, reconf_recs);
	reconf_mon_graph(cm, pcopies, vsplits, nodes_mongraph_reconfs);
}

/*void add_time_window(char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, 
struct reconf_func_context *reconf_func_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_func_recs);
	reconf_mon_graph(pcopies, vsplits, nodes_mongraph_reconfs);
}

void add_event_window(char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, 
struct reconf_func_context *reconf_func_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_func_recs);
	reconf_mon_graph(pcopies, vsplits, nodes_mongraph_reconfs);
}*/

/*void add_counting_sample(char *node_contact_list, int qid, int action, int action_type, int pcopies, int vsplits, 
struct reconf_func_context *reconf_func_recs) {
	init_mongraph_reconf(node_contact_list, qid, action, action_type, reconf_func_recs);
	reconf_mon_graph(pcopies, vsplits, nodes_mongraph_reconfs);
}*/
