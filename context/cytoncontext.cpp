#include "cytoncontext.h"

struct cyton *create_cyton_context() {
	struct cyton *cyctx = (struct cyton *)calloc(1, sizeof(struct cyton));
	cyctx->nomap = (struct nodes_state_map *)calloc(1, sizeof(struct nodes_state_map));
	int m = 0;
	cyctx->nomap[m].node_contact_list = NULL;
	cyctx->nomap[m].node_coord_contact_list = NULL;
	cyctx->nomap[m].node_aggr_contact_list = NULL;
	cyctx->nomap[m].node_actu_contact_list = NULL;
	cyctx->nomap[m].node_parent_contact_list = NULL;
	cyctx->nomap[m].node_child_contact_list = (char **)calloc(5, sizeof(char**));
	for (int n = 0; n < 5; n++) {
		cyctx->nomap[m].node_child_contact_list[n] = NULL;
	}
	return cyctx;
}

void set_cyton_context() {
	
}

int check_is_node_control_comps_assigned(struct cyton *cyctx, char *node_contact_list) {
	int check = 0; int i = 0;
	if(cyctx->nomap[i].node_contact_list != NULL) {
		if(cyctx->nomap[i].node_coord_contact_list != NULL && cyctx->nomap[i].node_aggr_contact_list != NULL 
			&& cyctx->nomap[i].node_actu_contact_list != NULL) {
			check = 1;
		}
	}
	return check;
}

int check_is_node_coord(struct cyton *cyctx, char *node_contact_list) {
	int check = 0; int i = 0;
	if(cyctx->nomap[i].node_contact_list != NULL) {
		if(strcmp(cyctx->nomap[i].node_contact_list, node_contact_list) == 0) {
			if(strcmp(cyctx->nomap[i].node_coord_contact_list, cyctx->nomap[i].node_contact_list) == 0) {
				check = 1;
			}
		}
	}
	return check;
}

int check_is_node_aggregator(struct cyton *cyctx, char *node_contact_list) {
	int check = 0; int i = 0;
	if(cyctx->nomap[i].node_contact_list != NULL) {
		if(strcmp(cyctx->nomap[i].node_contact_list, node_contact_list) == 0) {
			if(strcmp(cyctx->nomap[i].node_aggr_contact_list, cyctx->nomap[i].node_contact_list) == 0) {
				check = 1;
			}
		}
	}
	return check;
}

int check_is_node_coord_assigned(struct cyton *cyctx, char *node_contact_list) {
	int check = 0; int i = 0;
	if(cyctx->nomap[i].node_contact_list != NULL) {
		if(strcmp(cyctx->nomap[i].node_contact_list, node_contact_list) == 0) {
			if(cyctx->nomap[i].node_coord_contact_list != NULL) {
				check = 1;
			}
		}
	}
	return check;
}

void update_local_node_state_from_coord(struct cyton *cyctx, struct control_comp *ctrl_cmp) {
	int i = 0;
	cyctx->nomap[i].node_contact_list = strdup((ctrl_cmp)->node_contact_list);
	cyctx->nomap[i].node_coord_contact_list = strdup((ctrl_cmp)->node_coord_contact_list);
	cyctx->nomap[i].node_aggr_contact_list = strdup((ctrl_cmp)->node_aggr_contact_list);
	cyctx->nomap[i].node_actu_contact_list = strdup((ctrl_cmp)->node_actu_contact_list);
}

void update_local_node_state_from_boot(struct cyton *cyctx, struct ret_node_coord_map *bootserver_msg) {
	int i = 0;
	if(cyctx->nomap[i].node_contact_list == NULL) {
		cyctx->nomap[i].node_contact_list = strdup((bootserver_msg)->node_contact_list);
		cyctx->nomap[i].node_coord_contact_list = strdup((bootserver_msg)->node_coord_contact_list);
		printf("Coordinator assigned to me = %s \n", cyctx->nomap[i].node_coord_contact_list);
		cyctx->nomap[i].node_aggr_contact_list = strdup((bootserver_msg)->node_aggr_contact_list);
		printf("Aggregator node assigned to me = %s \n", cyctx->nomap[i].node_aggr_contact_list);
		cyctx->nomap[i].node_actu_contact_list = strdup((bootserver_msg)->node_actu_contact_list);
		printf("Actuator node assigned to me = %s \n", cyctx->nomap[i].node_actu_contact_list);
		cyctx->nomap[i].node_parent_contact_list = strdup((bootserver_msg)->node_parent_contact_list);
		printf("Parent node assigned to me = %s \n", cyctx->nomap[i].node_parent_contact_list);
		for(int j = 0; j < 5; i++) {
			if(cyctx->nomap[i].node_child_contact_list[j] == NULL) {
				cyctx->nomap[i].node_child_contact_list[j] = strdup((bootserver_msg)->node_child_contact_list); 
				printf("Child node assigned to me = %s \n", cyctx->nomap[i].node_child_contact_list[j]);
				break;
			} else {
				printf("Child node already assigned to me = NULL \n");
			}
		}
	}
}
	
