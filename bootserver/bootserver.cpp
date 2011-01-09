#include "bootserver.h"

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
	{"node_parent_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_parent_contact_list)},
	{"node_aggr_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_aggr_contact_list)},
	{"node_actu_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_actu_contact_list)},
	{"node_child_contact_list", "string", sizeof(char*), FMOffset(ret_node_coord_map_ptr, node_child_contact_list)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec in_format_list[] =
{
    {"in_data", in_field_list, sizeof(node_boot), NULL},
    {NULL, NULL}
};

static FMStructDescRec out_format_list[] =
{
    {"out_data", out_field_list, sizeof(ret_node_coord_map), NULL},
    {NULL, NULL}
};

void create_nodes_hierarchy_map(int num_levels, int num_coords, int num_nodes) {
	ncmap = (struct nodes_coords_map *) calloc(1, sizeof(struct nodes_coords_map));
	ncmap->num_coords_assign = 0;
	ncmap->num_nodes_boot = 0;
	ncmap->num_allow_coord = num_coords;
	ncmap->num_nodes_per_coord = num_nodes;

	ncmap->nostatelists = (struct nodes_ostate_list *) calloc(num_coords, sizeof(struct nodes_ostate_list));
	for(int j = 0; j < num_coords; j ++) {
		ncmap->nostatelists[j].is_node_children_full = 0;
		ncmap->nostatelists[j].node_contact_list = NULL;
		ncmap->nostatelists[j].node_coord_contact_list = NULL;
		ncmap->nostatelists[j].node_aggr_contact_list = NULL;
		ncmap->nostatelists[j].node_actu_contact_list = NULL;
		ncmap->nostatelists[j].node_parent_contact_list = NULL;
		ncmap->nostatelists[j].node_child_contact_list = (char **) calloc(num_nodes, sizeof(char**));
		for(int k = 0; k < num_nodes; k ++) {
			ncmap->nostatelists[j].node_child_contact_list[k] =  NULL;
		}
	}
}

void send_boot_coord_info(CManager cm, struct ret_node_coord_map *ret_node_coord_map) {
	boot_out_format = CMregister_format(cm, out_format_list);
	CMConnection conn = CMget_conn(cm, attr_list_from_string(ret_node_coord_map->node_contact_list));
	if (conn == NULL) {
		printf("No connection, attr list was :");
		dump_attr_list(attr_list_from_string(ret_node_coord_map->node_contact_list));
		printf("\n");
		exit(1);
	}
	printf("Node coordinator assigned \"%s\"\n",  ret_node_coord_map->node_coord_contact_list);
	printf("Node aggregator assigned \"%s\"\n",  ret_node_coord_map->node_aggr_contact_list);
	printf("Node actuator assigned \"%s\"\n",  ret_node_coord_map->node_actu_contact_list);
	printf("Node parent assigned \"%s\"\n",  ret_node_coord_map->node_parent_contact_list);
	printf("Node child assigned \"%s\"\n",  ret_node_coord_map->node_child_contact_list);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	CMwrite(conn, boot_out_format, ret_node_coord_map);
}

static void search_assign_coord(CManager cm, CMConnection conn, void *msg, void *client_data, attr_list attrs) {
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Who is coordinator for node \"%s\"\n", ((struct node_boot *)msg)->node_contact_list);
	if(ncmap->num_coords_assign == 0) {
		int i = 0;
		ncmap->nostatelists[i].node_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->nostatelists[i].node_coord_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->nostatelists[i].node_parent_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->nostatelists[i].node_aggr_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->nostatelists[i].node_actu_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->nostatelists[i].node_child_contact_list[ncmap->num_nodes_boot] = strdup(((struct node_boot *)msg)->node_contact_list);
		ncmap->num_coords_assign += 1;
		ncmap->num_nodes_boot += 1;

		retncmap = (struct ret_node_coord_map *) calloc(1, sizeof(struct ret_node_coord_map));
		retncmap->node_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		retncmap->node_coord_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		retncmap->node_parent_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		retncmap->node_aggr_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		retncmap->node_actu_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
		retncmap->node_child_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
	} else {
		for(int i = 0; i < ncmap->num_allow_coord; i++) {
			if(!ncmap->nostatelists[i].is_node_children_full) {
				for(int j = 0; j < ncmap->num_nodes_per_coord; j++) {
					if(ncmap->nostatelists[i].node_child_contact_list[j] == NULL) {
						if(ncmap->nostatelists[i].node_contact_list == NULL) {
							ncmap->nostatelists[i].node_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						}
						if(ncmap->nostatelists[i].node_coord_contact_list == NULL) {
							ncmap->nostatelists[i].node_coord_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
							ncmap->num_coords_assign += 1;
						}
						if(ncmap->nostatelists[i].node_parent_contact_list == NULL) {
							ncmap->nostatelists[i].node_parent_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						}
						if(ncmap->nostatelists[i].node_aggr_contact_list == NULL) {
							ncmap->nostatelists[i].node_aggr_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						}
						if(ncmap->nostatelists[i].node_actu_contact_list == NULL) {
							ncmap->nostatelists[i].node_actu_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						}
						ncmap->nostatelists[i].node_child_contact_list[j] = strdup(((struct node_boot *)msg)->node_contact_list);
						ncmap->num_nodes_boot += 1;

						retncmap = (struct ret_node_coord_map *) calloc(1, sizeof(struct ret_node_coord_map));
						retncmap->node_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						retncmap->node_coord_contact_list = strdup(ncmap->nostatelists[i].node_coord_contact_list);
						retncmap->node_parent_contact_list = strdup(ncmap->nostatelists[i].node_parent_contact_list);
						retncmap->node_aggr_contact_list = strdup(ncmap->nostatelists[i].node_aggr_contact_list);
						retncmap->node_actu_contact_list = strdup(ncmap->nostatelists[i].node_actu_contact_list);
						retncmap->node_child_contact_list = strdup(((struct node_boot *)msg)->node_contact_list);
						if(ncmap->num_nodes_boot == ncmap->num_nodes_per_coord) {
							ncmap->nostatelists[i].is_node_children_full = 1;
							ncmap->num_nodes_boot = 0;
						}
						break;
					}
				}
				break;
			} 
		}
	}
	send_boot_coord_info(cm, retncmap);
}
	
void *listenerThread(void *data) {
	attr_list listen_list = NULL; 
    CMlisten_specific(cm, listen_list);

	boot_in_format = CMregister_format(cm, in_format_list);
	CMregister_handler(boot_in_format, search_assign_coord, NULL);

	CMsleep(cm, 1);
	CMrun_network(cm);
	pthread_exit(NULL);
}
	
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

	printf("Bootstrap node CM contact list \"%s\"\n", node_cm_contact_list);

	int num_levels = atoi(argv[1]);
	int num_coords = atoi(argv[2]);
	int num_nodes = atoi(argv[3]);

	create_nodes_hierarchy_map(num_levels, num_coords, num_nodes);
	
	struct server_config *serv_config = (struct server_config *) calloc(1, sizeof(struct server_config));
	serv_config->num_allow_coord = num_coords;
	serv_config->num_nodes_per_coord = num_nodes;

	/* Create coordinator thread for coordinator matrix and its changes for updates from nodes */	
	pthread_t thrCoord;
	int rcListen = pthread_create(&thrCoord, NULL, listenerThread, serv_config);	
	if (rcListen) {
		printf("ERROR; listener thread return code from pthread_create() is %d\n", rcListen);
        exit(-1);
	}

	pthread_exit(NULL);
	return 0;
}
