#include "monitors.h"

static FMField sensor_field_list[] = 
{
	{"timestamp", "double", sizeof(double), FMOffset(sensor_rec_ptr, timestamp)},
	{"attr_name", "string", sizeof(char*), FMOffset(sensor_rec_ptr, attr_name)},
	{"rec_val_int", "integer", sizeof(int), FMOffset(sensor_rec_ptr, rec_val_int)},
	{"rec_val_float", "float", sizeof(float), FMOffset(sensor_rec_ptr, rec_val_float)},
	{"rec_val", "unsigned integer", sizeof(unsigned long), FMOffset(sensor_rec_ptr, rec_val)},
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

char *get_apache_busy_workers_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_busy_workers\";\n\
\toutput.rec_val_int = get_apache_busy_workers();\n\
}";

char *get_apache_idle_workers_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_idle_workers\";\n\
\toutput.rec_val_int = get_apache_idle_workers();\n\
}";

char *get_apache_req_rate_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_req_rate\";\n\
\toutput.rec_val_float = get_apache_req_rate();\n\
}";

char *get_apache_bytes_rate_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_bytes_rate\";\n\
\toutput.rec_val_float = get_apache_bytes_rate();\n\
}";

char *get_apache_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_web_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"apache\");\n\
}";

char *get_tomcat_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_serv_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"tomcat\");\n\
}";

char *get_apache_num_vcpus_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"apache_samoa1_num_vcpus\";\n\
\toutput.rec_val_int = libvirt_get_domain_num_vcpus(\"apache\");\n\
}";

char *get_rubis_dom0_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_dom0_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"Domain-0\");\n\
}";

char *get_rubis_apache_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_web_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"apache\");\n\
}";

char *get_rubis_apache_tx_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_web_iftxpackets\";\n\
\toutput.rec_val = libvirt_get_domain_if_txpackets(\"apache\");\n\
}";

char *get_rubis_tomcat_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_serv_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"tomcat\");\n\
}";

char *get_rubis_tomcat_tx_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_serv_iftxpackets\";\n\
\toutput.rec_val = libvirt_get_domain_if_txpackets(\"tomcat\");\n\
}";

char *get_rubis_mysql_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_db_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"mysql\");\n\
}";

char *get_rubis_mysql_tx_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"rubis_samoa1_db_iftxpackets\";\n\
\toutput.rec_val = libvirt_get_domain_if_txpackets(\"mysql\");\n\
}";

char *get_entropy_web_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_web_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"apache\");\n\
}";

char *get_entropy_serv_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_serv_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"tomcat\");\n\
}";

char *get_entropy_db_ctime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_db_ctotaltime\";\n\
\toutput.rec_val = libvirt_get_domain_cpu_total_time(\"mysql\");\n\
}";

char *get_entropy_web_req_rate_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_web_req_rate\";\n\
\toutput.rec_val_float = get_apache_req_rate();\n\
}";

char *get_entropy_serv_req_count_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_serv_req_count\";\n\
\toutput.rec_val_int = get_tomcat_req_count(\"jdk-8009\");\n\
}";

char *get_entropy_serv_req_ptime_metrics_ECL = "{\n\
\toutput.timestamp = gettimeofday_func();\n\
\toutput.attr_name = \"entropy_samoa1_serv_req_ptime\";\n\
\toutput.rec_val_int = get_tomcat_req_ptime(\"jdk-8009\");\n\
}";

void create_monitors_stream(CManager cm, struct context *ctx, char *mon_type, char *resource_type, char *mon_metric, int metric_num, char *node_contact_list, char *node_coord_contact_list, struct stone_context *stone_ctx, struct func_context *func_ctx, struct mon_context *mon_ctx, attr_list mon_metric_attr_list) {
	char *mon_metric_func = NULL;
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
	if(strcmp(mon_metric, "apache_busy_workers") == 0) {
		mon_metric_func = strdup(get_apache_busy_workers_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_idle_workers") == 0) {
		mon_metric_func = strdup(get_apache_idle_workers_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_req_rate") == 0) {
		mon_metric_func = strdup(get_apache_req_rate_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_bytes_rate") == 0) {
		mon_metric_func = strdup(get_apache_bytes_rate_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_web_ctotaltime") == 0) {
		mon_metric_func = strdup(get_apache_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_serv_ctotaltime") == 0) {
		mon_metric_func = strdup(get_tomcat_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "apache_num_vcpus") == 0) {
		mon_metric_func = strdup(get_apache_num_vcpus_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_dom0_ctotaltime") == 0) {
		mon_metric_func = strdup(get_rubis_dom0_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_web_ctotaltime") == 0) {
		mon_metric_func = strdup(get_rubis_apache_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_serv_ctotaltime") == 0) {
		mon_metric_func = strdup(get_rubis_tomcat_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_db_ctotaltime") == 0) {
		mon_metric_func = strdup(get_rubis_mysql_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_web_iftxpackets") == 0) {
		mon_metric_func = strdup(get_rubis_apache_tx_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_serv_iftxpackets") == 0) {
		mon_metric_func = strdup(get_rubis_tomcat_tx_metrics_ECL);
	}
	if(strcmp(mon_metric, "rubis_db_iftxpackets") == 0) {
		mon_metric_func = strdup(get_rubis_mysql_tx_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_web_ctotaltime") == 0) {
		mon_metric_func = strdup(get_entropy_web_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_serv_ctotaltime") == 0) {
		mon_metric_func = strdup(get_entropy_serv_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_db_ctotaltime") == 0) {
		mon_metric_func = strdup(get_entropy_db_ctime_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_web_req_rate") == 0) {
		mon_metric_func = strdup(get_entropy_web_req_rate_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_serv_req_count") == 0) {
		mon_metric_func = strdup(get_entropy_serv_req_count_metrics_ECL);
	}
	if(strcmp(mon_metric, "entropy_serv_req_ptime") == 0) {
		mon_metric_func = strdup(get_entropy_serv_req_ptime_metrics_ECL);
	}

	do {
		EVstone mon_metric_split_stone = EValloc_stone (cm);
		EVaction mon_metric_split_action = EVassoc_split_action(cm, mon_metric_split_stone, NULL);

		stone_ctx->stone_id = -1;
		stone_ctx->stone_action = -1;
		stone_ctx->split_stone = mon_metric_split_stone;
		stone_ctx->split_action = mon_metric_split_action;
		func_ctx->func_name = strdup(mon_metric);
		func_ctx->in_format = "sensor";
		func_ctx->out_format = "sensor";
		func_ctx->func_level = 0;
		mon_ctx->mon_key = strdup(mon_metric);
		mon_ctx->mon_attr = strdup(mon_metric);
		mon_ctx->mon_type = strdup(mon_type);
		mon_ctx->mon_resource_type = strdup(resource_type);

		EVstone mon_metric_stone = EValloc_stone (cm);
		FMStructDescRec *mon_format_list = get_format_list(func_ctx->out_format);
		char *mon_action_spec = create_transform_action_spec(NULL, mon_format_list, mon_metric_func);
		EVaction mon_metric_action = EVassoc_immediate_action (cm, mon_metric_stone, mon_action_spec, 0);
		EVaction_set_output(cm, mon_metric_stone, mon_metric_action, 0, mon_metric_split_stone);
		printf(" Metric : %s num %d deployed with stone id : %d and stone action id : %d \n", mon_metric, metric_num, mon_metric_stone, 
		mon_metric_action);
		printf(" Metric : %s num %d exporting split stone id : %d and split action id : %d \n", mon_metric, metric_num, mon_metric_split_stone, 
		mon_metric_split_action);
		//EVset_attr_list(cm, mon_metric_stone, mon_metric_attr_list);

		int *sample_secvp; int *sample_usecvp;
		int sample_sec; int sample_usec;
		sample_secvp = &sample_sec; sample_usecvp = &sample_usec;
		atom_t mon_metric_sec_rate_atom; atom_t mon_metric_usec_rate_atom;

		//get_int_attr(mon_metric_attr_list, mon_metric_sec_rate_atom, sample_secvp);
		//get_int_attr(mon_metric_attr_list, mon_metric_usec_rate_atom, sample_usecvp);

		sample_sec = 1200; sample_usec = 0;
		//EVenable_auto_stone(cm, mon_metric_stone, sample_sec, sample_usec);
		EVenable_auto_stone(cm, mon_metric_stone, sample_sec, sample_usec);
		printf(" Metric : %s num %d monitoring started with sample_sec : %d and sample_usec : %d \n", mon_metric, metric_num, sample_sec, sample_usec);
		
		msg_coord_for_monitors_streams(node_contact_list, node_coord_contact_list, stone_ctx, func_ctx, mon_ctx, ctx);

		clean_stone_context(1, stone_ctx);
		clean_func_context(1, func_ctx);
		clean_mon_context(1, mon_ctx);
		metric_num = metric_num - 1;

		sleep(2);	
	} while (metric_num > 0);
}

