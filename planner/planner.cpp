#include "planner.h"

query_ctx *qctx;
int total_deploy_aggregators = 0;
int total_deploy_nodes = 0;

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

static FMStructDescRec node_deploy_format_list[] =
{
    {"node_deploy_data", node_deploy_field_list, sizeof(node_deploy), NULL},
    {"func_recs", func_field_list, sizeof(func_rec), NULL},
    {NULL, NULL}
};

static FMField local_term_field_list[] = 
{
	{"exec_id", "integer", sizeof(int), FMOffset(local_exec_act_ptr, exec_id)},
	{"exec_func", "string", sizeof(char*), FMOffset(local_exec_act_ptr, exec_func)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec local_term_format_list[] = 
{
    {"local_exec", local_term_field_list, sizeof(local_exec_act), NULL},
    {NULL, NULL}
};

char *node_coord_contact_list = NULL;

void set_axon_context(struct query_ctx *qctx) {
	paxctx = qctx->axctx;
}

struct axon *get_axon_context() {
	paxctx = qctx->axctx;
	return paxctx;
}

void set_coord_contact(struct query_ctx *qctx) {
	node_coord_contact_list = strdup((qctx->axctx)->comap[0].node_coord_contact_list);
}

char *get_coord_contact() {
	return node_coord_contact_list;
}

void *check_query_deployed(void *arg) {
	do {
		sleep(60);
		if(check_coord_is_query_graph_deployed(((struct query_ctx *)arg)->axctx, ((struct query_ctx *)arg)->qid, 
			((struct query_ctx *)arg)->graph_id, total_deploy_aggregators, total_deploy_nodes)) {
			printf("QUERY GRAPH : %d IS DEPLOYED ON NUM OF AGGREGATORS : %d and NUM OF NODES : %d \n", ((struct query_ctx *)arg)->qid, 
			total_deploy_aggregators, total_deploy_nodes);
			printf("STARTING CONNECTING QUERY GRAPH ENDPOINTS TO AGGREGATORS GRAPH ENDPOINTS \n");
			connect_nodes_aggregator_funcs_endpoints_for_query(((struct query_ctx *)arg)->cm, ((struct query_ctx *)arg)->axctx, 
			((struct query_ctx *)arg)->coord_contact_list, ((struct query_ctx *)arg)->qid);
			printf("STARTING CONNECTING MONITOR ENDPOINTS TO QUERY GRAPH ENDPOINTS \n");
			connect_monitors_nodes_funcs_endpoints_for_query(((struct query_ctx *)arg)->cm, ((struct query_ctx *)arg)->axctx, 
			((struct query_ctx *)arg)->coord_contact_list, ((struct query_ctx *)arg)->qid, ((struct query_ctx *)arg)->metric_list);
			set_axon_context((struct query_ctx *)arg);
			set_coord_contact((struct query_ctx *)arg);
			//free((struct query_ctx *)arg);
			break;
		}
		sleep(2);
	} while(1);
	pthread_exit(NULL);
}

void populate_term_msg(struct local_exec_act *loadsat_act_data, char *node_coord_contact_list, char *node_contact_list) {
	char *loadsat_act_func = "{\n\
	\tint num_vcpus = 0;\n\
	\tlibvirt_init();\n\
	\tlibvirt_open();\n\
	\tnum_vcpus = (int)libvirt_get_domain_num_vcpus(\"Domain-0\");\n\
	\tnum_vcpus = num_vcpus - 1;\n\
	\tlibvirt_set_vcpus(\"Domain-0\", num_vcpus);\n\
	\tlibvirt_close();\n\
	}";

	char *loadsat_act_func_1 = "{\n\
	\tsystem(\"xm shutdown mapache\");\n\
	\tsleep(2);\n\
	\tsystem(\"xm create /var/local/mahendra/mapache.cfg\");\n\
	}";

	char *loadsat_act_func_2 = "{\n\
	\tchar *mapache_xml = \"\\\n\
	<domain type=\'xen\'>\\\n\
	<name>mapachenew</name>\\\n\
	<memory>524288</memory>\\\n\
	<currentMemory>262144</currentMemory>\\\n\
	<vcpu>2</vcpu>\\\n\
	<os>\\\n\
	<type>linux</type>\\\n\
	<kernel>/var/local/mahendra/common/vmlinuz-3.4.1-2.6.18.8-xenU</kernel>\\\n\
	<initrd>/var/local/mahendra/common/initrd.img-2.6.18.8-xen0</initrd>\\\n\
	<cmdline>xencons=xvc console=xvc0</cmdline>\\\n\
	<root>/dev/hda1 ro</root>\\\n\
	</os>\\\n\
	<clock offset=\'utc\'/>\\\n\
	<on_poweroff>destroy</on_poweroff>\\\n\
	<on_reboot>restart</on_reboot>\\\n\
	<on_crash>restart</on_crash>\\\n\
	<devices>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/apache-httpd/newapachehttpd.img\'/>\\\n\
	<target dev=\'hda1\' bus=\'ide\'/>\\\n\
	</disk>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/apache-httpd/newapachehttpd-swap.img\'/>\\\n\
	<target dev=\'hda2\' bus=\'ide\'/>\\\n\
    </disk>\\\n\
    <interface type=\'bridge\'>\\\n\
    <mac address=\'00:16:3e:34:34:34\'/>\\\n\
    <source bridge=\'xenbr1\'/>\\\n\
    <ip address=\'10.0.0.114\'/>\\\n\
	<script path=\'vif-bridge bridge=xenbr1\'/>\\\n\
    <target dev=\'vif34.0\'/>\\\n\
	</interface>\\\n\
	<console type=\'pty\' tty=\'/dev/pts/3\'>\\\n\
    <source path=\'/dev/pts/3\'/>\\\n\
	<target port=\'0\'/>\\\n\
    </console>\\\n\
    </devices>\\\n\
	</domain>\\\n\
	\";\\\n\
	\tlibvirt_init();\n\
	\tlibvirt_open();\n\
	\tlibvirt_shutdown_domain(\"mapache\");\n\
	\tlibvirt_create_domain(mapache_xml);\n\
	\tlibvirt_close();\n\
	}";

	char *loadsat_act_func_3 = "{\n\
	\tchar *mapache_xml = \"\\\n\
	<domain type=\'xen\'>\\\n\
	<name>mapachenew</name>\\\n\
	<memory>524288</memory>\\\n\
	<currentMemory>262144</currentMemory>\\\n\
	<vcpu>2</vcpu>\\\n\
	<os>\\\n\
	<type>linux</type>\\\n\
	<kernel>/var/local/mahendra/common/vmlinuz-3.4.1-2.6.18.8-xenU</kernel>\\\n\
	<initrd>/var/local/mahendra/common/initrd.img-2.6.18.8-xen0</initrd>\\\n\
	<cmdline>xencons=xvc console=xvc0</cmdline>\\\n\
	<root>/dev/hda1 ro</root>\\\n\
	</os>\\\n\
	<clock offset=\'utc\'/>\\\n\
	<on_poweroff>destroy</on_poweroff>\\\n\
	<on_reboot>restart</on_reboot>\\\n\
	<on_crash>restart</on_crash>\\\n\
	<devices>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/apache-httpd/newapachehttpd.img\'/>\\\n\
	<target dev=\'hda1\' bus=\'ide\'/>\\\n\
	</disk>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/apache-httpd/newapachehttpd-swap.img\'/>\\\n\
	<target dev=\'hda2\' bus=\'ide\'/>\\\n\
    </disk>\\\n\
    <interface type=\'bridge\'>\\\n\
    <mac address=\'00:16:3e:34:34:34\'/>\\\n\
    <source bridge=\'xenbr1\'/>\\\n\
    <ip address=\'10.0.0.114\'/>\\\n\
	<script path=\'vif-bridge bridge=xenbr1\'/>\\\n\
    <target dev=\'vif34.0\'/>\\\n\
	</interface>\\\n\
	<console type=\'pty\' tty=\'/dev/pts/3\'>\\\n\
    <source path=\'/dev/pts/3\'/>\\\n\
	<target port=\'0\'/>\\\n\
    </console>\\\n\
    </devices>\\\n\
	</domain>\\\n\
	\";\\\n\
	\tchar *mtomcat_xml = \"\\\n\
	<domain type=\'xen\'>\\\n\
	<name>mtomcatnew</name>\\\n\
	<memory>524288</memory>\\\n\
	<currentMemory>262144</currentMemory>\\\n\
	<vcpu>2</vcpu>\\\n\
	<os>\\\n\
	<type>linux</type>\\\n\
	<kernel>/var/local/mahendra/common/vmlinuz-3.4.1-2.6.18.8-xenU</kernel>\\\n\
	<initrd>/var/local/mahendra/common/initrd.img-2.6.18.8-xen0</initrd>\\\n\
	<cmdline>xencons=xvc console=xvc0</cmdline>\\\n\
	<root>/dev/hda1 ro</root>\\\n\
	</os>\\\n\
	<clock offset=\'utc\'/>\\\n\
	<on_poweroff>destroy</on_poweroff>\\\n\
	<on_reboot>restart</on_reboot>\\\n\
	<on_crash>restart</on_crash>\\\n\
	<devices>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/tomcat/newtomcat.img\'/>\\\n\
	<target dev=\'hda1\' bus=\'ide\'/>\\\n\
	</disk>\\\n\
	<disk type=\'file\' device=\'disk\'>\\\n\
	<driver name=\'file\'/>\\\n\
	<source file=\'/var/local/mahendra/tomcat/newtomcat-swap.img\'/>\\\n\
	<target dev=\'hda2\' bus=\'ide\'/>\\\n\
    </disk>\\\n\
    <interface type=\'bridge\'>\\\n\
    <mac address=\'00:16:3e:34:34:34\'/>\\\n\
    <source bridge=\'xenbr1\'/>\\\n\
    <ip address=\'10.0.0.115\'/>\\\n\
	<script path=\'vif-bridge bridge=xenbr1\'/>\\\n\
    <target dev=\'vif34.0\'/>\\\n\
	</interface>\\\n\
	<console type=\'pty\' tty=\'/dev/pts/3\'>\\\n\
    <source path=\'/dev/pts/3\'/>\\\n\
	<target port=\'0\'/>\\\n\
    </console>\\\n\
    </devices>\\\n\
	</domain>\\\n\
	\";\\\n\
	\tlibvirt_init();\n\
	\tlibvirt_open();\n\
	\tlibvirt_shutdown_domain(\"mapache\");\n\
	\tlibvirt_shutdown_domain(\"mtomcat\");\n\
	\tsleep(5);\n\
	\tlibvirt_create_domain(mtomcat_xml);\n\
	\tsleep(10);\n\
	\tlibvirt_create_domain(mapache_xml);\n\
	\tlibvirt_close();\n\
	}";

	char *loadsat_act_func_4 = "{\n\
	\tbalance_disable_gchannel(\"10.0.0.141\");\n\
	\treturn 0;\n\
	}";

	char *loadsat_act_func_5 = "{\n\
	\treturn 0;\n\
	}";

	char *loadsat_act_func_6 = "{\n\
	\tfloat apache_cpu_util = libvirt_get_domain_cpu_util(\"apache\"); \n\
	\tfloat tomcat_cpu_util = libvirt_get_domain_cpu_util(\"tomcat\"); \n\
	\tfloat mysql_cpu_util = libvirt_get_domain_cpu_util(\"mysql\"); \n\
	\tint req_count = get_tomcat_req_count(\"jdk-8009\"); \n\
	\tint req_ptime = get_tomcat_req_ptime(\"jdk-8009\"); \n\
	\tint busy_threads = get_tomcat_busy_threads(\"jdk-8009\"); \n\
	\tint tomcat_avail = get_tomcat_availability();\n\
	\tint apache_busy_threads = get_apache_busy_workers();\n\
	\tint apache_avail = get_apache_availability();\n\
	\tdouble apache_req_rate = get_apache_req_rate();\n\
	\treturn 0;\n\
	}";

	loadsat_act_data->exec_id = 1; 
	loadsat_act_data->exec_func = strdup(loadsat_act_func_5); 
}

int handler_threshold = 200;
int handler_count = 0; char *node_contact_list;
char bstring[512];
FILE *blog;

static int terminal_handler(CManager cm, void *event, void *client_data, attr_list attrs) {
 	handler_count += 1;
	printf("Got inside terminal handler count %d ...\n", handler_count);
	if(handler_count > 0) {
		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		blog = fopen("entropyfaulttriggertimewithlocalflag.log", "a");
		if(blog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(bstring, "%d \t %lf %d \n", handler_count, timestamp, (((sizeof(char)*64)*3) + ((sizeof(int)*3))));
		fwrite(bstring, sizeof(char), strlen(bstring), blog);
		fclose(blog);
	}
	if(strcmp((char *)client_data, "apache_busy_sample_actuator") == 0 || 
		strcmp((char *)client_data, "rubis_fault_sample_actuator") == 0 ) {
		if(handler_count > handler_threshold) {
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			struct axon* axctx = get_axon_context();
			printf("Node coord contact list : %s \n ", node_coord_contact_list);
			get_deployable_nodes_from_coord(axctx, node_coord_contact_list, deploy_ctx);
			attr_list node_cm_attr_list =  NULL;
			int num_deploy_nodes = deploy_ctx->num_deploy_nodes;
			for(int i = 0; i < num_deploy_nodes; i++) {
				printf("Creating connection to the node : %d contact list : %s \n ", i, deploy_ctx->deploy_nodes_contact_list[i]);
				if(strcmp(node_coord_contact_list, deploy_ctx->deploy_nodes_contact_list[i]) != 0) {
					attr_list deploy_node_attr_list = attr_list_from_string(deploy_ctx->deploy_nodes_contact_list[i]);
					CMConnection conn = CMget_conn(cm, deploy_node_attr_list);
					if(conn == NULL) {
						printf("No connection, attr list was :");
						dump_attr_list(deploy_node_attr_list);
						printf("\n");
						exit(1);
					}
					local_term_format = CMregister_format(cm, local_term_format_list);
					
					local_exec_act loadsat_act_data;
					populate_term_msg(&loadsat_act_data, node_coord_contact_list, attr_list_to_string(deploy_node_attr_list));

					struct timeval now;
					gettimeofday(&now, NULL);
					double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
					if(strcmp((char *)client_data, "rubis_fault_sample_actuator") == 0) {
						blog = fopen("faulttriggertime.log", "a");
					} else {
						blog = fopen("busytriggertime.log", "a");
					}
					if(blog == NULL){
						printf("Cannot open file.\n");
						return 1;
					}
				    sprintf(bstring, "Final Trigger Time : %lf \n", timestamp);
				    fwrite(bstring, sizeof(char), strlen(bstring), blog);
				    fclose(blog);
					printf("THRESHOLD TRIGGER ACTIVATED TIMESTAMP %lf \n", timestamp);
					printf("TRIGGER LOCAL EXEC ACTION SENDING NODE LOCAL EXEC ACTION \n");
					CMwrite(conn, local_term_format, &loadsat_act_data);
				} else {
					// DO nothing ..
				}
			}
			handler_count = 0;
			handler_threshold = 250;
			printf("RESETTING HANDLER COUNT %d ... \n", handler_count);
			printf("RESETTING HANDLER THRESHOLD %d ... \n", handler_threshold);
		}
	} else if(strcmp((char *)client_data, "dtree_sample_actuator") == 0) {
		if(handler_count > handler_threshold) {
			struct deploy_nodes_context *deploy_ctx;
			deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
			struct axon* axctx = get_axon_context();
			printf("Node coord contact list : %s \n ", node_coord_contact_list);
			get_deployable_nodes_from_coord(axctx, node_coord_contact_list, deploy_ctx);
			attr_list node_cm_attr_list =  NULL;
			int num_deploy_nodes = deploy_ctx->num_deploy_nodes;
			for(int i = 0; i < num_deploy_nodes; i++) {
				printf("Creating connection to the node : %d contact list : %s \n ", i, deploy_ctx->deploy_nodes_contact_list[i]);
				if(strcmp(node_coord_contact_list, deploy_ctx->deploy_nodes_contact_list[i]) != 0) {
					attr_list deploy_node_attr_list = attr_list_from_string(deploy_ctx->deploy_nodes_contact_list[i]);
					CMConnection conn = CMget_conn(cm, deploy_node_attr_list);
					if(conn == NULL) {
						printf("No connection, attr list was :");
						dump_attr_list(deploy_node_attr_list);
						printf("\n");
						exit(1);
					}
					local_term_format = CMregister_format(cm, local_term_format_list);
					
					local_exec_act loadsat_act_data;
					populate_term_msg(&loadsat_act_data, node_coord_contact_list, attr_list_to_string(deploy_node_attr_list));

					struct timeval now;
					gettimeofday(&now, NULL);
					double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
					blog = fopen("entropytriggertime.log", "a");
					if(blog == NULL){
						printf("Cannot open file.\n");
						return 1;
					}
				    sprintf(bstring, "Final Trigger Time : %lf \n", timestamp);
				    fwrite(bstring, sizeof(char), strlen(bstring), blog);
				    fclose(blog);
					printf("THRESHOLD TRIGGER ACTIVATED TIMESTAMP %lf \n", timestamp);
					printf("TRIGGER LOCAL EXEC ACTION SENDING NODE LOCAL EXEC ACTION \n");
					CMwrite(conn, local_term_format, &loadsat_act_data);
				} else {
					// DO nothing ..
				}
			}
			handler_count = 0;
			handler_threshold = 10;
			printf("RESETTING HANDLER COUNT %d ... \n", handler_count);
			printf("RESETTING HANDLER THRESHOLD %d ... \n", handler_threshold);
		}
	}
}

void create_coord_aggregator_streams_network(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, int graph_id, int num_recs, 
struct func_rec *func_recs) {
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

	EVSimpleHandlerFunc handler;
	handler = terminal_handler;
	char *handler_data;

	for(int j = 0; j < num_recs; j++) {
		if(func_recs[j].func_level == 2) {
			if(strcmp(func_recs[j].func_name, "dtree_sample_actuator") == 0) {
				handler_data = strdup(func_recs[j].func_name);
			}else if(strcmp(func_recs[j].func_name, "rubis_fault_sample_actuator") == 0) {
				handler_data = strdup(func_recs[j].func_name);
			}else if(strcmp(func_recs[j].func_name, "apache_busy_sample_actuator") == 0) {
				handler_data = strdup(func_recs[j].func_name);
			}

			int *local_ids = realize_vertex(cm, qid, (func_recs[j]), pcopies, handler, handler_data, stone_ctx_list, func_ctx_list, split_ctx_list); 
			stone_count += 1;
			if(stone_count > 0) {
				realize_edge(cm, qid, pcopies, split_ctx_list, stone_ids);
				clean_stone_ids(1, stone_ids);
				clean_split_context(1, split_ctx_list->split_ctx);
			}
			stone_ids = local_ids;
		}
		//free(split_ctx_list->split_ctx);
	}
	register_aggregator_endpoints_for_query(cm, axctx, coord_contact_list, qid, num_recs, stone_ctx_list, func_ctx_list);
	free(split_ctx_list);
	free(stone_ctx_list);
	free(func_ctx_list);
}

void populate_node_deploy_rec(int query_id, struct node_deploy *per_node_deploy_data, struct func_list *func_list, char *node_contact_list, 
char *node_coord_contact_list) {
	per_node_deploy_data->node_contact_list = strdup(node_contact_list);
	per_node_deploy_data->node_coord_contact_list = strdup(node_coord_contact_list);
	per_node_deploy_data->query_id = query_id;
	per_node_deploy_data->graph_id = query_id;
	per_node_deploy_data->node_type = 0;
	per_node_deploy_data->num_recs = func_list->num_recs;
	per_node_deploy_data->func_recs = (struct func_rec *) calloc(func_list->num_recs, sizeof(struct func_rec));
	for(int i = 0; i < func_list->num_recs; i++) {
		per_node_deploy_data->func_recs[i].func_name = strdup(func_list->func_recs[i].func_name);
		per_node_deploy_data->func_recs[i].in_format = strdup(func_list->func_recs[i].in_format);
		per_node_deploy_data->func_recs[i].out_format = strdup(func_list->func_recs[i].out_format);
		per_node_deploy_data->func_recs[i].flevel_start_point = func_list->func_recs[i].flevel_start_point;
		per_node_deploy_data->func_recs[i].flevel_end_point = func_list->func_recs[i].flevel_end_point;
		per_node_deploy_data->func_recs[i].func_splits = func_list->func_recs[i].func_splits;
		per_node_deploy_data->func_recs[i].func_type = func_list->func_recs[i].func_type;
		per_node_deploy_data->func_recs[i].func_order = func_list->func_recs[i].func_order;
		per_node_deploy_data->func_recs[i].func_level = func_list->func_recs[i].func_level;
	}
}

void deploy_funcs_to_nodes(struct query_ctx *qctx, int num_deploy_nodes, char **deploy_nodes_contact_list, char *node_coord_contact_list) {
	attr_list node_cm_attr_list =  NULL;
	for(int i = 0; i < num_deploy_nodes; i++) {
		if(strcmp(node_coord_contact_list, deploy_nodes_contact_list[i]) != 0) {
			node_cm_attr_list = attr_list_from_string(deploy_nodes_contact_list[i]);
			CMConnection conn = CMget_conn(qctx->cm, node_cm_attr_list);
			if (conn == NULL) {
				printf("No connection, attr list was :");
				dump_attr_list(node_cm_attr_list);
				printf("\n");
				exit(1);
			}
			per_node_deploy_format = CMregister_format(qctx->cm, node_deploy_format_list);
		
			node_deploy node_deploy_data;
			populate_node_deploy_rec(qctx->graph_id, &node_deploy_data, qctx->func_list, deploy_nodes_contact_list[i], node_coord_contact_list);
			CMwrite(conn, per_node_deploy_format, &node_deploy_data);
		} else {
			// deploy funcs for on node assuming coordinator role
			// TO BE DONE..
		}
	}
}

void populate_aggr_node_deploy_rec(int query_id, struct node_deploy *node_aggr_deploy_data, struct func_list *func_list, char *node_aggr_contact_list, 
char *node_coord_contact_list) {
	node_aggr_deploy_data->node_contact_list = strdup(node_aggr_contact_list);
	node_aggr_deploy_data->node_coord_contact_list = strdup(node_coord_contact_list);
	node_aggr_deploy_data->query_id = query_id;
	node_aggr_deploy_data->graph_id = query_id;
	node_aggr_deploy_data->node_type = 1;
	node_aggr_deploy_data->num_recs = func_list->num_recs;
	node_aggr_deploy_data->func_recs = (struct func_rec *) calloc(func_list->num_recs, sizeof(struct func_rec));
	for(int i = 0; i < func_list->num_recs; i++) {
		node_aggr_deploy_data->func_recs[i].func_name = strdup(func_list->func_recs[i].func_name);
		node_aggr_deploy_data->func_recs[i].in_format = strdup(func_list->func_recs[i].in_format);
		node_aggr_deploy_data->func_recs[i].out_format = strdup(func_list->func_recs[i].out_format);
		node_aggr_deploy_data->func_recs[i].flevel_start_point = func_list->func_recs[i].flevel_start_point;
		node_aggr_deploy_data->func_recs[i].flevel_end_point = func_list->func_recs[i].flevel_end_point;
		node_aggr_deploy_data->func_recs[i].func_type = func_list->func_recs[i].func_type;
		node_aggr_deploy_data->func_recs[i].func_order = func_list->func_recs[i].func_order;
		node_aggr_deploy_data->func_recs[i].func_level = func_list->func_recs[i].func_level;
	}
}

void deploy_funcs_to_aggregators(struct query_ctx *qctx, int num_deploy_aggregators, char **node_aggregators_contact_list, 
char *node_coord_contact_list) {
	attr_list node_cm_attr_list = attr_list_from_string(node_aggregators_contact_list[num_deploy_aggregators - 1]);

	CMConnection conn = CMget_conn(qctx->cm, node_cm_attr_list);
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(node_cm_attr_list);
		printf("\n");
		exit(1);
	}
	planner_node_deploy_format = CMregister_format(qctx->cm, node_deploy_format_list);
	
	node_deploy node_deploy_data;
	populate_aggr_node_deploy_rec(qctx->graph_id, &node_deploy_data, qctx->func_list, node_aggregators_contact_list[0], node_coord_contact_list);
	CMwrite(conn, planner_node_deploy_format, &node_deploy_data);
}

void create_streams_network(struct control_comp *ctrl_comp, struct query_ctx *qctx, char *coord_contact_list) {
	struct deploy_nodes_context *deploy_ctx = (struct deploy_nodes_context *) calloc(1, sizeof(struct deploy_nodes_context));
	get_deployable_aggregators_from_coord(qctx->axctx, coord_contact_list, deploy_ctx);
	printf("Number of deployable aggregators for monitoring graph id : %d : %d \n", qctx->qid, deploy_ctx->num_deploy_nodes);
	printf("Deployable aggregators for monitoring graph id : %d : %s \n", qctx->qid, deploy_ctx->deploy_nodes_contact_list[0]);
	register_aggregators_for_query(qctx->axctx, qctx->qid, qctx->coord_contact_list, deploy_ctx->num_deploy_nodes, 
	deploy_ctx->deploy_nodes_contact_list, 0, 0);
	if(check_is_aggregator_coord(qctx->axctx, deploy_ctx->num_deploy_nodes, deploy_ctx->deploy_nodes_contact_list, coord_contact_list)) {
		if(!check_coord_is_aggregator_graph_deployed_for_query(qctx->axctx, deploy_ctx->num_deploy_nodes, 
			deploy_ctx->deploy_nodes_contact_list[deploy_ctx->num_deploy_nodes - 1], qctx->qid, qctx->graph_id)) {
			create_coord_aggregator_streams_network(qctx->cm, qctx->axctx, qctx->coord_contact_list, qctx->qid, qctx->graph_id, 
			qctx->func_list->num_recs, qctx->func_list->func_recs);
		}
		register_aggregator_graph_deployed_for_query(qctx->cm, qctx->axctx, deploy_ctx->num_deploy_nodes,
		deploy_ctx->deploy_nodes_contact_list[deploy_ctx->num_deploy_nodes - 1], qctx->qid);
	} else {
		// Should send qctx->func_recs after checking if its type i.e. 2
		if(!check_coord_is_aggregator_graph_deployed_for_query(qctx->axctx, deploy_ctx->num_deploy_nodes,
			deploy_ctx->deploy_nodes_contact_list[deploy_ctx->num_deploy_nodes - 1], 
			qctx->qid, qctx->graph_id)) {
			deploy_funcs_to_aggregators(qctx, deploy_ctx->num_deploy_nodes, deploy_ctx->deploy_nodes_contact_list, 
			coord_contact_list);
		}
		register_aggregator_graph_deployed_for_query(qctx->cm, qctx->axctx, deploy_ctx->num_deploy_nodes, 
		deploy_ctx->deploy_nodes_contact_list[deploy_ctx->num_deploy_nodes - 1], qctx->qid);
	}
	total_deploy_aggregators = deploy_ctx->num_deploy_nodes;
	clean_deploy_nodes_context(1, deploy_ctx);
	get_deployable_nodes_from_coord(qctx->axctx, coord_contact_list, deploy_ctx);
	printf("Number of deployable nodes for monitoring graph id : %d : %d \n", qctx->qid, deploy_ctx->num_deploy_nodes);
	register_nodes_for_query(qctx->axctx, qctx->qid, coord_contact_list, deploy_ctx->num_deploy_nodes, 
	deploy_ctx->deploy_nodes_contact_list, 0, 0);
	// Should send qctx->func_recs after checking if its type i.e. 1
	int is_nodes_graph_deployed_for_query = 0;
	for(int k = 0; k < deploy_ctx->num_deploy_nodes; k ++) {
		printf("Deployable nodes for monitoring graph id : %d : %s \n", qctx->qid, deploy_ctx->deploy_nodes_contact_list[k]);
		is_nodes_graph_deployed_for_query =+ check_coord_is_nodes_graph_deployed_for_query(qctx->axctx, deploy_ctx->num_deploy_nodes, 
		deploy_ctx->deploy_nodes_contact_list[k], qctx->qid, qctx->graph_id);
	}

	if(is_nodes_graph_deployed_for_query != deploy_ctx->num_deploy_nodes) {
		// Check nodes which do not have query deploy and deploy only to those
		deploy_funcs_to_nodes(qctx, deploy_ctx->num_deploy_nodes, deploy_ctx->deploy_nodes_contact_list, coord_contact_list);
	}

	total_deploy_nodes = deploy_ctx->num_deploy_nodes;
	free(deploy_ctx);
	pthread_t thrQueryDeployed;
	int rcQueryDeployed = pthread_create(&thrQueryDeployed, NULL, check_query_deployed, qctx);	
	if (rcQueryDeployed) {
		printf("ERROR; Query thread return code from pthread_create() is %d\n", rcQueryDeployed);
		exit(-1);
	}
}

void *boot_processing_graph(void *arg) {
	do {
		if(check_is_coord_control_comps_assigned(((struct query_ctx *)arg)->axctx, ((struct query_ctx *)arg)->coord_contact_list)) {
			struct control_comp *ctrl_comp = get_coord_control_comps(((struct query_ctx*)arg)->axctx ,((struct query_ctx *)arg)->coord_contact_list);
			printf("Start creating streams network for monitoring graph .. \n");
			create_streams_network(ctrl_comp, ((struct query_ctx *)arg), ((struct query_ctx *)arg)->coord_contact_list);
			break;
		}
		sleep(2);
	} while(1);
	pthread_exit(NULL);
}

void deploy_monitoring_graph(CManager cm, struct axon *axctx, char *src_contact_list, char *coord_contact_list, int qid, int qtype, 
int graph_id, struct metric_list *metric_list, int num_funcs, struct func_rec *func_recs) {
	//graph id not required to be removed
	register_queries_to_axon(cm, axctx, src_contact_list, coord_contact_list, qid, qtype, 0, 0, graph_id, num_funcs, func_recs);
	qctx = (struct query_ctx *) calloc(1, sizeof(struct query_ctx));
	qctx->qid = qid;
	qctx->qtype = qtype;
	qctx->graph_id = graph_id;
	qctx->metric_list = (struct metric_list *) calloc(1, sizeof(struct metric_list));
	qctx->metric_list->num_recs = metric_list->num_recs;
	qctx->metric_list->metric_recs = (char **) calloc(metric_list->num_recs, sizeof(char **));	
	for(int j = 0; j < metric_list->num_recs; j++) {
		qctx->metric_list->metric_recs[j] = strdup(metric_list->metric_recs[j]);	
	}
	qctx->src_contact_list = strdup(src_contact_list);
	qctx->coord_contact_list = strdup(coord_contact_list);
	qctx->axctx = axctx;
	qctx->cm = cm;
	qctx->func_list = (struct func_list *) calloc(1, sizeof(func_list));
	qctx->func_list->num_recs = num_funcs;
	qctx->func_list->func_recs = (struct func_rec *) calloc(num_funcs, sizeof(struct func_rec));
	for(int j = 0; j < num_funcs; j++) {
		qctx->func_list->func_recs[j].func_name = strdup(func_recs[j].func_name);
		qctx->func_list->func_recs[j].in_format = strdup(func_recs[j].in_format);
		qctx->func_list->func_recs[j].out_format = strdup(func_recs[j].out_format);
		qctx->func_list->func_recs[j].flevel_start_point = func_recs[j].flevel_start_point;
		qctx->func_list->func_recs[j].flevel_end_point = func_recs[j].flevel_end_point;
		qctx->func_list->func_recs[j].func_splits = func_recs[j].func_splits;
		qctx->func_list->func_recs[j].func_type = func_recs[j].func_type;
		qctx->func_list->func_recs[j].func_order = func_recs[j].func_order;
		qctx->func_list->func_recs[j].func_level = func_recs[j].func_level;
	}
	// api to take query_context and create a thread internally inside control layer
	if(check_is_node_coord(axctx, coord_contact_list, src_contact_list)) {
		pthread_t thrQuery;
		int rcQuery = pthread_create(&thrQuery, NULL, boot_processing_graph, qctx);	
		if (rcQuery) {
			printf("ERROR; Query boot thread return code from pthread_create() is %d\n", rcQuery);
			exit(-1);
		}
	}
}
