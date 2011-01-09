#ifndef BOOTSERVER_H
#define BOOTSERVER_H

#include "../util/common.h" 

typedef struct node_boot {
	int is_control;
	char *bootserver_contact_list;
	char *node_contact_list;
} node_boot, *node_boot_ptr;

typedef struct nodes_coords_map {
	int num_allow_coord;
	int num_nodes_per_coord;
	int num_coords_assign;
	int num_nodes_boot;
	struct nodes_ostate_list *nostatelists;
} nodes_coords_map, *nodes_coords_map_ptr;

typedef struct nodes_coords_list {
	char **coord_contact_list;
} nodes_coords_list, *nodes_coords_list_ptr;

typedef struct nodes_ostate_list {
	int is_node_children_full;
	char *node_contact_list;
	char *node_coord_contact_list;
	char *node_parent_contact_list; //can have more than 1 parent but considered 1 parent for now
	char *node_aggr_contact_list;
	char *node_actu_contact_list;
	char **node_child_contact_list;
} nodes_ostate_list, *nodes_ostate_list_ptr;

struct nodes_coords_map *ncmap;
struct ret_node_coord_map *retncmap;

typedef struct server_config {
	int num_allow_coord;
	int num_nodes_per_coord;
	CManager cmgr;	
} server_config, *server_config_ptr;

CManager cm; 
CMFormat boot_in_format;
CMFormat boot_out_format;

#endif
