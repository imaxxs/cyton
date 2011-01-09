#include "axoncontext.h"

struct axon *create_axon_context() {
	struct axon *axctx = (struct axon *)calloc(1, sizeof(struct axon));

	axctx->qnmap = (struct queries_nodes_map *)calloc(NUM_NODES, sizeof(struct queries_nodes_map));
	for(int m = 0; m < NUM_NODES; m++) {
		axctx->qnmap[m].query_src_contact_list = NULL;
		axctx->qnmap[m].query_coord_contact_list = NULL;
		axctx->qnmap[m].qruns = (struct query_run_state *)calloc(NUM_QUERY, sizeof(struct query_run_state));
		for(int l = 0; l < NUM_QUERY; l++) {
			axctx->qnmap[m].qruns[l].query_id = -1;
			axctx->qnmap[m].qruns[l].query_type = 0;
			axctx->qnmap[m].qruns[l].is_query_deployed = 0;
			axctx->qnmap[m].qruns[l].is_query_on = 0;
			axctx->qnmap[m].qruns[l].graph_id = 0; // Field not in use to be removed
			axctx->qnmap[m].qruns[l].func_recs = (struct func_rec *) calloc(NUM_FUNCS, sizeof(func_rec));
			for (int k = 0; k < NUM_FUNCS; k++) {
				axctx->qnmap[m].qruns[l].func_recs[k].func_name = NULL;
				axctx->qnmap[m].qruns[l].func_recs[k].in_format = NULL;
				axctx->qnmap[m].qruns[l].func_recs[k].out_format = NULL;
				axctx->qnmap[m].qruns[l].func_recs[k].func_type = -1;
				axctx->qnmap[m].qruns[l].func_recs[k].func_order = -1;
				axctx->qnmap[m].qruns[l].func_recs[k].func_level = -1;
			}
			axctx->qnmap[m].qruns[l].query_aggregators = (struct query_deployers *)calloc(NUM_AGGREGATORS, sizeof(struct query_deployers));
			for(int n = 0; n < NUM_AGGREGATORS; n++) {
				axctx->qnmap[m].qruns[l].query_aggregators[n].node_contact_list = NULL;
				axctx->qnmap[m].qruns[l].query_aggregators[n].is_query_deployed = -1;
				axctx->qnmap[m].qruns[l].query_aggregators[n].is_query_running = -1;

				axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints = (struct stone_context_list *)calloc(2, 
				sizeof(struct stone_context_list));
				axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx_num = 1;
				axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx = (struct stone_context *) calloc(1, 
				sizeof(struct stone_context));

				axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx_num = 1;
				axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx = (struct stone_context *) calloc(1, 
				sizeof(struct stone_context));

				axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints = (struct func_context_list *) calloc(2, sizeof(struct func_context_list));
				axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx_num = 1;
				axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx = (struct func_context *) calloc(1, 
				sizeof(struct func_context));

				axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx_num = 1;
				axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx = (struct func_context *) calloc(1, 
				sizeof(struct func_context));

				for(int j = 0; j < 1; j++) {
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx[j].stone_id = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx[j].stone_action = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx[j].split_stone = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[0].stone_ctx[j].split_action = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx[j].func_name = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx[j].in_format = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx[j].out_format = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx[j].func_order = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[0].func_ctx[j].func_level = -1;
				}
				for(int j = 0; j < 1; j++) {
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx[j].stone_id = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx[j].stone_action = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx[j].split_stone = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].stone_endpoints[1].stone_ctx[j].split_action = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx[j].func_name = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx[j].in_format = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx[j].out_format = NULL;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx[j].func_order = -1;
					axctx->qnmap[m].qruns[l].query_aggregators[n].func_endpoints[1].func_ctx[j].func_level = -1;
				}
			}
			axctx->qnmap[m].qruns[l].query_nodes = (struct query_deployers *)calloc(NUM_NODES, sizeof(struct query_deployers));
			for(int p = 0; p < NUM_NODES; p++) {
				axctx->qnmap[m].qruns[l].query_nodes[p].node_contact_list = NULL;
				axctx->qnmap[m].qruns[l].query_nodes[p].is_query_deployed = -1;
				axctx->qnmap[m].qruns[l].query_nodes[p].is_query_running = -1;

				axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints = (struct stone_context_list *)calloc(2, 
				sizeof(struct stone_context_list));
				axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx_num = 1; // Depends on group map func should be 3
				axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx = (struct stone_context *) calloc(1, 
				sizeof(struct stone_context));

				axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx_num = 1; // 1 because always 1 group map func considered
				axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx = (struct stone_context *) calloc(1, 
				sizeof(struct stone_context));

				axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints = (struct func_context_list *) calloc(2, sizeof(struct func_context_list));
				axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx_num = 1; // Depends on group map func should be 3 //Sameasabove
				axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx = (struct func_context *) calloc(1, 
				sizeof(struct func_context));

				axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx_num = 1; // 1 because always 1 group map func considered
				axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx = (struct func_context *) calloc(1, sizeof(struct func_context));

				for(int j = 0; j < 1; j++) { // Will depend on group map func output splits should change to 3
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx[j].stone_id = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx[j].stone_action = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx[j].split_stone = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[0].stone_ctx[j].split_action = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx[j].func_name = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx[j].in_format = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx[j].out_format = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx[j].func_order = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[0].func_ctx[j].func_level = -1;
				}
				for(int j = 0; j < 1; j++) {
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx[j].stone_id = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx[j].stone_action = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx[j].split_stone = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].stone_endpoints[1].stone_ctx[j].split_action = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx[j].func_name = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx[j].in_format = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx[j].out_format = NULL;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx[j].func_order = -1;
					axctx->qnmap[m].qruns[l].query_nodes[p].func_endpoints[1].func_ctx[j].func_level = -1;
				}
			}
		}
	}

	axctx->qnstates = (struct query_node_states *) calloc(1, sizeof(struct query_node_states));

	axctx->mnmap = (struct monitors_nodes_map *)calloc(NUM_NODES, sizeof(struct monitors_nodes_map));
	for(int m = 0; m < NUM_NODES; m++) {
		axctx->mnmap[m].monitors_node_contact_list = NULL;
		axctx->mnmap[m].msmap = (struct monitors_stones_map *)calloc(NUM_MONS, sizeof(struct monitors_stones_map));
		for (int n = 0; n < NUM_MONS; n++) {
			axctx->mnmap[m].msmap[n].monitors_key = NULL;
			axctx->mnmap[m].msmap[n].monitors_type = NULL;
			axctx->mnmap[m].msmap[n].monitors_stream_id = 0;
			axctx->mnmap[m].msmap[n].monitors_split_action = 0;
			axctx->mnmap[m].msmap[n].monitors_data_format = NULL;
		}
	}

	axctx->comap = (struct coord_overlay_map *)calloc(NUM_COORDS, sizeof(struct coord_overlay_map));
	for(int m = 0; m < NUM_COORDS; m++) {
		axctx->comap[m].node_contact_list = NULL;
		axctx->comap[m].node_coord_contact_list = NULL;
		axctx->comap[m].node_aggr_contact_list = NULL;
		axctx->comap[m].node_actu_contact_list = NULL;
		axctx->comap[m].node_parent_contact_list = NULL;
		axctx->comap[m].node_child_contact_list = (char **)calloc(NUM_NODES, sizeof(char**));
		for (int n = 0; n < NUM_NODES; n++) {
			axctx->comap[m].node_child_contact_list[n] = NULL;
		}
	}

	return axctx;
}
static int count = 0;
int check_is_constraint_violated(struct axon *axctx) {
		printf("CHECK IS CONSTRAINT VIOLATED -- count -- %d \n", count);
		count += 1;
		if(count > 5) {
			return 1;
		} else {
			return 0;
		}
}

void set_qnode_state(struct axon *axctx, char *node_contact_list, int qid, char *func_name, int pcopies, int vcopies, 
int stone_id, int stone_action, int split_stone, int split_action) {
	if(!axctx->qnstates) {
		axctx->qnstates = (struct query_node_states *) calloc(1, sizeof(struct query_node_states));
		axctx->qnstates->qnode_state_recs = 1;
		axctx->qnstates->qnstates = (struct query_node_state *)calloc(axctx->qnstates->qnode_state_recs, sizeof(struct query_node_state));
		axctx->qnstates->qnstates->qid = 0;
		axctx->qnstates->qnstates->node_contact_list = NULL;
		axctx->qnstates->qnstates->func_stone_recs = 1;
		axctx->qnstates->qnstates->funcs_stones = (struct func_stone *) calloc (1, sizeof(struct func_stone));
		axctx->qnstates->qnstates->funcs_stones[0].func_name = strdup(func_name); 
		axctx->qnstates->qnstates->funcs_stones[0].stone_pcopy = pcopies; 
		axctx->qnstates->qnstates->funcs_stones[0].split_vcopy = vcopies; 
		axctx->qnstates->qnstates->funcs_stones[0].stone_id = stone_id; 
		axctx->qnstates->qnstates->funcs_stones[0].stone_action = stone_action; 
		axctx->qnstates->qnstates->funcs_stones[0].split_stone = split_stone; 
		axctx->qnstates->qnstates->funcs_stones[0].split_action = split_action; 
	} else {
		int i; int found = 0;
		for(i = 0; i < axctx->qnstates->qnode_state_recs; i++) {
			if(axctx->qnstates->qnstates->qid == qid && strcmp(axctx->qnstates->qnstates->node_contact_list, node_contact_list) == 0) {
				int fnode = axctx->qnstates->qnstates->func_stone_recs + 1;
				axctx->qnstates->qnstates->func_stone_recs = fnode;
				axctx->qnstates->qnstates->funcs_stones = (struct func_stone *) realloc (axctx->qnstates->qnstates->funcs_stones, 
				1 * sizeof(struct func_stone));
				axctx->qnstates->qnstates->funcs_stones[fnode].func_name = strdup(func_name); 
				axctx->qnstates->qnstates->funcs_stones[fnode].stone_pcopy = pcopies; 
				axctx->qnstates->qnstates->funcs_stones[fnode].split_vcopy = vcopies; 
				axctx->qnstates->qnstates->funcs_stones[fnode].stone_id = stone_id; 
				axctx->qnstates->qnstates->funcs_stones[fnode].stone_action = stone_action; 
				axctx->qnstates->qnstates->funcs_stones[fnode].split_stone = split_stone; 
				axctx->qnstates->qnstates->funcs_stones[fnode].split_action = split_action; 
				found = 1;
			}
			if(found) break;
		}
		if(!found) {
			int qnode = axctx->qnstates->qnode_state_recs + 1;
			axctx->qnstates->qnode_state_recs = qnode;
			axctx->qnstates->qnstates = (struct query_node_state *) realloc(axctx->qnstates->qnstates, 1 * sizeof(struct query_node_state));
			axctx->qnstates->qnstates->qid = 0;
			axctx->qnstates->qnstates->node_contact_list = NULL;
			axctx->qnstates->qnstates->func_stone_recs = 1;
			axctx->qnstates->qnstates->funcs_stones = (struct func_stone *) calloc (1, sizeof(struct func_stone));
			axctx->qnstates->qnstates->funcs_stones[0].func_name = strdup(func_name); 
			axctx->qnstates->qnstates->funcs_stones[0].stone_pcopy = pcopies; 
			axctx->qnstates->qnstates->funcs_stones[0].split_vcopy = vcopies; 
			axctx->qnstates->qnstates->funcs_stones[0].stone_id = stone_id; 
			axctx->qnstates->qnstates->funcs_stones[0].stone_action = stone_action; 
			axctx->qnstates->qnstates->funcs_stones[0].split_stone = split_stone; 
			axctx->qnstates->qnstates->funcs_stones[0].split_action = split_action; 
		}
	}
}

void get_qnode_state(struct axon *axctx, char *node_contact_list, int qid, char *func_name, int stone_ctx_num, struct stone_context *stone_ctx) {
	int i; int j; int found = 0;  
	for (i = 0; i < axctx->qnstates->qnode_state_recs; i ++) {
		if(axctx->qnstates->qnstates[i].qid == qid && strcmp(axctx->qnstates->qnstates[i].node_contact_list, node_contact_list) == 0) {
			for(j = 0; j < axctx->qnstates->qnstates[i].func_stone_recs; j++) {
				// get_group_source_stone_ids - for the call use strstr
				if(strcmp(axctx->qnstates->qnstates[i].funcs_stones[j].func_name, func_name) == 0) {
					if(!stone_ctx) {
						stone_ctx_num = stone_ctx_num + 1;
						stone_ctx = (struct stone_context *) calloc (1, sizeof(struct stone_context));
					}
					
					stone_ctx[j].stone_pcopy = axctx->qnstates->qnstates[i].funcs_stones[j].stone_pcopy;
					stone_ctx[j].split_vcopy = axctx->qnstates->qnstates[i].funcs_stones[j].split_vcopy;
					stone_ctx[j].stone_id = axctx->qnstates->qnstates[i].funcs_stones[j].stone_id;
					stone_ctx[j].stone_action = axctx->qnstates->qnstates[i].funcs_stones[j].stone_action;
					stone_ctx[j].split_stone = axctx->qnstates->qnstates[i].funcs_stones[j].split_stone;
					stone_ctx[j].split_action = axctx->qnstates->qnstates[i].funcs_stones[j].split_action;
					if(strcmp(axctx->qnstates->qnstates[i].funcs_stones[i + 1].func_name, func_name) == 0){
						stone_ctx_num = stone_ctx_num + 1;
						stone_ctx = (struct stone_context *) realloc (stone_ctx, 1 * sizeof(struct stone_context));
						continue;
					} else {
						found = 1;
						break;
					}
				}
			}
			if(found) break;
		}
	}
}

void connect_monitors_nodes_funcs_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, 
struct metric_list *metric_list) {
	printf("Connecting monitor endpoints for query : %d  for monitoring attribute : %s \n", qid, metric_list->metric_recs[0]);
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int m = 0; m < NUM_QUERY; m++) {
				if(axctx->qnmap[i].qruns[m].query_id == qid) {
					for(int n = 0; n < NUM_NODES; n++) {
						if(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list != NULL && 
							strcmp(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list, coord_contact_list) != 0) {
							attr_list dnode_attr_list = attr_list_from_string(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);
							printf("Creating graph endpoints at node : %s  \n", axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);
							CMConnection nconn = CMget_conn(cm, dnode_attr_list);
							if (nconn == NULL) {
								printf("No connection, attr list was :");
								dump_attr_list(dnode_attr_list);
								printf("\n");
								exit(1);
							}
							for(int o = 0 ; o < NUM_NODES; o++) {
								if(strcmp(axctx->mnmap[o].monitors_node_contact_list, axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list) == 0) {
									for(int p = 0; p < NUM_MONS; p++) {
										if(axctx->mnmap[o].msmap[p].monitors_key != NULL) {
											printf("Adding split target from mon split stone id : %d split action id : %d to end stone id : %d \n", 
											axctx->mnmap[o].msmap[p].monitors_stream_id,
											axctx->mnmap[o].msmap[p].monitors_split_action,
											axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[1].stone_ctx[0].stone_id);

											REVaction_add_split_target(nconn, axctx->mnmap[o].msmap[p].monitors_stream_id, 
											axctx->mnmap[o].msmap[p].monitors_split_action, 
											axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[1].stone_ctx[0].stone_id);

											printf("Added split target from stone : %d at node : %s to its output stone \n", 
											axctx->mnmap[o].msmap[p].monitors_stream_id,
											axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);
										}
									}
								}
							}
					    }
					}
				}
			}
		}
	}
}

void connect_nodes_aggregator_funcs_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid) {
	printf("Connecting aggregator endpoints for query : %d  \n", qid);
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int m = 0; m < NUM_QUERY; m++) {
				if(axctx->qnmap[i].qruns[m].query_id == qid) {
					for(int n = 0; n < NUM_NODES; n++) {
						if(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list != NULL && 
							strcmp(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list, coord_contact_list) != 0) {
							printf("Creating aggregator endpoints at node : %s  \n", axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);
							attr_list dnode_attr_list = attr_list_from_string(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);
							CMConnection nconn = CMget_conn(cm, dnode_attr_list);
							if (nconn == NULL) {
								printf("No connection, attr list was :");
								dump_attr_list(dnode_attr_list);
								printf("\n");
								exit(1);
							}

							EVstone node_output_stone = REVcreate_bridge_action(nconn, 
							attr_list_from_string(axctx->qnmap[i].qruns[m].query_aggregators[0].node_contact_list), 
							axctx->qnmap[i].qruns[m].query_aggregators[0].stone_endpoints[1].stone_ctx[0].stone_id);

							printf("Created output stone : %d at node : %s connected to aggregator \n", node_output_stone,
							axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list);

							printf(" Number of Endpoints : %d at node : %s to connect to its output stone: %d \n", 1, 
							axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list, node_output_stone);

							for(int p = 0; p < 1; p++) { // Depends on group map func splits ..should change to 3
								printf("Adding split target from endpoint id : %d with split_action_id : %d at node : %s to its output stone: %d \n", 
								axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_stone,
								axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_action, 
								axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list,
								node_output_stone);

								REVaction_add_split_target(nconn, axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_stone, 
								axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_action, 
								node_output_stone);

								printf("Added split target from stone : %d with split_action_id : %d at node : %s to its output stone: %d \n", 
								axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_stone,
								axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[0].stone_ctx[p].split_action,
								axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list, node_output_stone);
							}
					    }
					}
				}
			}
		}
	}
}

void register_nodes_endpoints_for_query(CManager cm, struct axon *axctx, char *node_contact_list, char *coord_contact_list, int qid, int num_recs, 
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list) {
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int m = 0; m < NUM_QUERY; m++) {
				if(axctx->qnmap[i].qruns[m].query_id == qid) {
					for(int n = 0; n < NUM_NODES; n++) {
						if(strcmp(axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list, node_contact_list) == 0) {
							printf("Registering graph endpoints at node : %s \n", axctx->qnmap[i].qruns[m].query_nodes[n].node_contact_list); 
							int epoints = 0;
							for(int k = 0; k < 2; k++) {
								if(k == 0) {
									epoints = 1; // Depends on group map func splits ..may change to 3 
								} else {
									epoints = 1; 
								}
								for(int j = 0; j < epoints; j++) {
									//printf("Graph stone endpoint index : %d stone_ctx index : %j stone id : %d .. \n", 
									//	k, j, stone_ctx_list[k].stone_ctx[j].stone_id); 
									axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[k].stone_ctx[j].stone_id =
										stone_ctx_list[k].stone_ctx[j].stone_id;
									//printf("Graph stone endpoint index : %d stone_ctx index : % j stone action id : %d .. \n", 
									//	k, j, stone_ctx_list[k].stone_ctx[j].stone_action); 
									axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[k].stone_ctx[j].stone_action =
										stone_ctx_list[k].stone_ctx[j].stone_action;
									//printf("Graph stone endpoint index : %d stone_ctx index : %j split stone id : %d .. \n", 
									//	k, j, stone_ctx_list[k].stone_ctx[j].split_stone); 
									axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[k].stone_ctx[j].split_stone = 
										stone_ctx_list[k].stone_ctx[j].split_stone;
									printf("Graph stone endpoint index : %d stone_ctx index : %d split action id : %d .. \n", 
										k, j, stone_ctx_list[k].stone_ctx[j].split_action); 
									if(stone_ctx_list[k].stone_ctx[j].split_action == -1) {
										stone_ctx_list[k].stone_ctx[j].split_action = 0;
									}
									axctx->qnmap[i].qruns[m].query_nodes[n].stone_endpoints[k].stone_ctx[j].split_action = 
										stone_ctx_list[k].stone_ctx[j].split_action;
									axctx->qnmap[i].qruns[m].query_nodes[n].func_endpoints[k].func_ctx[j].func_name = 
										func_ctx_list[k].func_ctx[j].func_name;
									axctx->qnmap[i].qruns[m].query_nodes[n].func_endpoints[k].func_ctx[j].in_format = 
										func_ctx_list[k].func_ctx[j].in_format;
									axctx->qnmap[i].qruns[m].query_nodes[n].func_endpoints[k].func_ctx[j].out_format =
										func_ctx_list[k].func_ctx[j].out_format;
									axctx->qnmap[i].qruns[m].query_nodes[n].func_endpoints[k].func_ctx[j].func_order =
										func_ctx_list[k].func_ctx[j].func_order;
									axctx->qnmap[i].qruns[m].query_nodes[n].func_endpoints[k].func_ctx[j].func_level =
										func_ctx_list[k].func_ctx[j].func_level;
								}
							}
							break;
					    }
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	}
}

void register_aggregator_endpoints_for_query(CManager cm, struct axon *axctx, char *coord_contact_list, int qid, int num_recs, 
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list) {
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int m = 0; m < NUM_QUERY; m++) {
				if(axctx->qnmap[i].qruns[m].query_id == qid) {
					printf("Registering aggregator endpoint at node : %s \n", axctx->qnmap[i].qruns[m].query_aggregators[0].node_contact_list); 
					for(int n = 0; n < 2; n++) {
						for(int j = 0; j < 1; j++) {
							//printf("Aggregator stone endpoint index : %d stone_ctx index : %j stone id : %d .. \n", n, j,
							//	stone_ctx_list[n].stone_ctx[j].stone_id); 
							axctx->qnmap[i].qruns[m].query_aggregators[0].stone_endpoints[n].stone_ctx[j].stone_id = 
								stone_ctx_list[n].stone_ctx[j].stone_id;
							//printf("Aggregator stone endpoint index : %d stone_ctx index : %j stone_action id : %d .. \n", n, j,
							//	stone_ctx_list[n].stone_ctx[j].stone_action); 
							axctx->qnmap[i].qruns[m].query_aggregators[0].stone_endpoints[n].stone_ctx[j].stone_action = 
								stone_ctx_list[n].stone_ctx[j].stone_action;
							//printf("Aggregator stone endpoint index : %d stone_ctx index : %j split_stone id : %d .. \n", n, j,
							//	stone_ctx_list[n].stone_ctx[j].split_stone); 
							axctx->qnmap[i].qruns[m].query_aggregators[0].stone_endpoints[n].stone_ctx[j].split_stone = 
								stone_ctx_list[n].stone_ctx[j].split_stone;
							//printf("Aggregator stone endpoint index : %d stone_ctx index : %j split_action id : %d .. \n", n, j,
							//	stone_ctx_list[n].stone_ctx[j].split_action); 
							axctx->qnmap[i].qruns[m].query_aggregators[0].stone_endpoints[n].stone_ctx[j].split_action = 
								stone_ctx_list[n].stone_ctx[j].split_action;
							axctx->qnmap[i].qruns[m].query_aggregators[0].func_endpoints[n].func_ctx[j].func_name = 
								func_ctx_list[n].func_ctx[j].func_name;
							axctx->qnmap[i].qruns[m].query_aggregators[0].func_endpoints[n].func_ctx[j].in_format = 
								func_ctx_list[n].func_ctx[j].in_format;
							axctx->qnmap[i].qruns[m].query_aggregators[0].func_endpoints[n].func_ctx[j].out_format = 
								func_ctx_list[n].func_ctx[j].out_format;
							axctx->qnmap[i].qruns[m].query_aggregators[0].func_endpoints[n].func_ctx[j].func_order = 
								func_ctx_list[n].func_ctx[j].func_order;
							axctx->qnmap[i].qruns[m].query_aggregators[0].func_endpoints[n].func_ctx[j].func_level = 
								func_ctx_list[n].func_ctx[j].func_level;
						}
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	}
}

void register_nodes_for_query(struct axon *axctx, int query_id, char*coord_contact_list, int num_deploy_nodes, char **deploy_nodes_contact_list, 
int is_query_deployed, int is_query_running) {
	int is_registered = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int j = 0 ; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == query_id) {
					for(int k = 0; k < num_deploy_nodes; k++) {
						axctx->qnmap[i].qruns[j].query_nodes[k].node_contact_list = strdup(deploy_nodes_contact_list[k]);
						axctx->qnmap[i].qruns[j].query_nodes[k].is_query_deployed = is_query_deployed;
						axctx->qnmap[i].qruns[j].query_nodes[k].is_query_running = is_query_running;
					}
					is_registered = 1;
				}
			}
		}
		if(is_registered) break;
	}
}

void register_aggregators_for_query(struct axon *axctx, int query_id, char *coord_contact_list, int num_deploy_aggregators, 
char **deploy_aggregators_contact_list, int is_query_deployed, int is_query_running) {
	int is_registered = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(strcmp(axctx->qnmap[i].query_coord_contact_list, coord_contact_list) == 0) {
			for(int j = 0 ; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == query_id) {
					for(int k = 0; k < num_deploy_aggregators; k++) {
						axctx->qnmap[i].qruns[j].query_aggregators[k].node_contact_list = strdup(deploy_aggregators_contact_list[k]);
						axctx->qnmap[i].qruns[j].query_aggregators[k].is_query_deployed = is_query_deployed;
						axctx->qnmap[i].qruns[j].query_aggregators[k].is_query_running = is_query_running;
					}
					is_registered = 1;
				}
			}
		}
		if(is_registered) break;
	}
}

void register_queries_to_axon(CManager cm, struct axon *axctx, char *src_contact_list, char *coord_contact_list, int qid, int qtype, 
int is_query_deployed, int is_query_on, int graph_id, int num_funcs, struct func_rec *func_recs) {
	int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].query_coord_contact_list == NULL) {
			axctx->qnmap[i].query_src_contact_list = strdup(src_contact_list); 
			axctx->qnmap[i].query_coord_contact_list = strdup(coord_contact_list); 
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == -1) {
					axctx->qnmap[i].qruns[j].query_id = qid;
					axctx->qnmap[i].qruns[j].query_type = qtype;
					axctx->qnmap[i].qruns[j].is_query_deployed = is_query_deployed;
					axctx->qnmap[i].qruns[j].is_query_on = is_query_on;
					axctx->qnmap[i].qruns[j].graph_id = graph_id; // Field not required to be removed
					for(int k = 0; k < num_funcs; k++) {
						axctx->qnmap[i].qruns[j].func_recs[k].func_name = strdup(func_recs[k].func_name);
						axctx->qnmap[i].qruns[j].func_recs[k].in_format = strdup(func_recs[k].in_format);
						axctx->qnmap[i].qruns[j].func_recs[k].out_format = strdup(func_recs[k].out_format);
						axctx->qnmap[i].qruns[j].func_recs[k].func_type = func_recs[k].func_type;
						axctx->qnmap[i].qruns[j].func_recs[k].func_order = func_recs[k].func_order;
						axctx->qnmap[i].qruns[j].func_recs[k].func_level = func_recs[k].func_level;
					}
					break;
				}
			}
		}
	}
}

void register_monitors_to_axon(struct axon *axctx, char *node_contact_list, char *key, char *mon_attr, EVstone split_stone, EVaction split_action, 
char *out_format) {
	int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->mnmap[i].monitors_node_contact_list == NULL || strcmp(axctx->mnmap[i].monitors_node_contact_list, node_contact_list) == 0 ) {
			axctx->mnmap[i].monitors_node_contact_list = strdup(node_contact_list); 
			//printf("Monitor deployed at node : %s ..\n " , axctx->mnmap[i].monitors_node_contact_list);
			for(int j = 0; j < NUM_MONS; j++) {
				if(axctx->mnmap[i].msmap[j].monitors_key == NULL) {
					axctx->mnmap[i].msmap[j].monitors_key = strdup(key);
					axctx->mnmap[i].msmap[j].monitors_type = strdup(mon_attr);
					axctx->mnmap[i].msmap[j].monitors_stream_id = split_stone;
					axctx->mnmap[i].msmap[j].monitors_split_action = split_action;
					axctx->mnmap[i].msmap[j].monitors_data_format = strdup(out_format);
					found = 1;
					break;
				}
			}
			if(found) break;
		}
	}
}

int check_coord_is_query_graph_deployed(struct axon *axctx, int qid, int graph_id, int total_deploy_aggregators, int total_deploy_nodes) {
	int query_deploy = 0; int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].qruns != NULL) {
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == qid && axctx->qnmap[i].qruns[j].graph_id == graph_id) {
					printf("IS MONITORING GRAPH ID : %d DEPLOYED COUNT : %d EQUALS NUM_DEPLOY_NODES %d ...\n", 
					axctx->qnmap[i].qruns[j].graph_id, axctx->qnmap[i].qruns[j].is_query_deployed, 
					(total_deploy_aggregators + total_deploy_nodes - 1));
					if(axctx->qnmap[i].qruns[j].is_query_deployed == (total_deploy_aggregators + total_deploy_nodes - 1)) {
						query_deploy = 1;
						break;
					}
				}
			}
			break;
		}
	}
	if(query_deploy) {
		printf("Monitoring Graph : %d is deployed ...\n", qid);
	}
	return query_deploy;
}
	
void register_nodes_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_nodes, char *deploy_contact_list, 
int qid) {
	int query_deploy = 0; int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].qruns != NULL) {
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == qid && axctx->qnmap[i].qruns[j].graph_id == qid) {
					for(int k = 0; k < num_deploy_nodes; k++) {
						if(strcmp(axctx->qnmap[i].qruns[j].query_nodes[k].node_contact_list, deploy_contact_list) == 0) {
							printf("Monitoring node graph deployed at node : %s ...\n", 
							axctx->qnmap[i].qruns[j].query_nodes[k].node_contact_list);
							axctx->qnmap[i].qruns[j].query_nodes[k].is_query_deployed = 1;
							found = 1;
							break;
						}
					}
					if(found) {
						axctx->qnmap[i].qruns[j].is_query_deployed += 1;
						break;
					}
				}
			}
			if(found) break;
		}
	}
}

void register_aggregator_graph_deployed_for_query(CManager cm, struct axon *axctx, int num_deploy_aggregators, char *deploy_contact_list, 
int qid) {
	int query_deploy = 0; int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].qruns != NULL) {
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == qid && axctx->qnmap[i].qruns[j].graph_id == qid) {
					for(int k = 0; k < num_deploy_aggregators; k++) {
						if(strcmp(axctx->qnmap[i].qruns[j].query_aggregators[k].node_contact_list, deploy_contact_list) == 0) {
							printf("Marking monitoring aggregator graph deployed at node : %s ...\n", 
							axctx->qnmap[i].qruns[j].query_aggregators[k].node_contact_list);
							axctx->qnmap[i].qruns[j].query_aggregators[k].is_query_deployed = 1;
							found = 1;
							break;
						}
					}
					if(found) {
						axctx->qnmap[i].qruns[j].is_query_deployed += 1;
						break;
					}
				}
			}
			if(found) break;
		}
	}
}

int check_coord_is_nodes_graph_deployed_for_query(struct axon *axctx, int num_deploy_nodes, char *deploy_contact_list, int qid, int graph_id) {
	int query_deploy = 0; int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].qruns != NULL) {
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == qid && axctx->qnmap[i].qruns[j].graph_id == graph_id) {
					for(int k = 0; k < num_deploy_nodes; k++) {
						if(strcmp(axctx->qnmap[i].qruns[j].query_nodes[k].node_contact_list, deploy_contact_list) == 0) {
							query_deploy = axctx->qnmap[i].qruns[j].query_nodes[k].is_query_deployed;
							found = 1;
							break;
						}
					}
					if(found) break;
				}
			}
			if(found) break;
		}
	}
	printf("Coordinator confirm monitoring node graph id : %d DEPLOYED : %d on node : %s ...\n", qid, query_deploy, deploy_contact_list);
	return query_deploy;
}

int check_coord_is_aggregator_graph_deployed_for_query(struct axon *axctx, int num_deploy_aggregators, char *deploy_contact_list, 
int qid, int graph_id) {
	int query_deploy = 0; int found = 0;
	for(int i = 0; i < NUM_NODES; i++) {
		if(axctx->qnmap[i].qruns != NULL) {
			for(int j = 0; j < NUM_QUERY; j++) {
				if(axctx->qnmap[i].qruns[j].query_id == qid && axctx->qnmap[i].qruns[j].graph_id == graph_id) {
					for(int k = 0; k < num_deploy_aggregators; k++) {
						if(strcmp(axctx->qnmap[i].qruns[j].query_aggregators[k].node_contact_list, deploy_contact_list) == 0) {
							query_deploy = axctx->qnmap[i].qruns[j].query_aggregators[k].is_query_deployed;
							found = 1;
							break;
						}
					}
					if(found) break;
				}
			}
			if(found) break;
		}
	}
	printf("Coordinator confirm monitoring aggregator graph id : %d DEPLOYED : %d on node : %s ...\n", qid, query_deploy, deploy_contact_list);
	return query_deploy;
}

void get_deployable_nodes_from_coord(struct axon *axctx, char *coord_contact_list, struct deploy_nodes_context *deploy_nodes_ctx) {
	int deployable_nodes_count = 0;
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_coord_contact_list != NULL) {
			if(strcmp(axctx->comap[i].node_coord_contact_list, coord_contact_list) == 0) {
				for(int j = 0; j < NUM_NODES; j++) {
					if(axctx->comap[i].node_child_contact_list[j] != NULL) {
						deployable_nodes_count += 1; 
					}
				}
				deploy_nodes_ctx->num_deploy_nodes = deployable_nodes_count;
				deploy_nodes_ctx->deploy_nodes_contact_list = (char**) calloc(deployable_nodes_count, sizeof(char**));
				for(int j = 0; j < NUM_NODES; j++) {
					if(axctx->comap[i].node_child_contact_list[j] != NULL) {
						for(int k = 0; k < deploy_nodes_ctx->num_deploy_nodes; k++) {
							if(deploy_nodes_ctx->deploy_nodes_contact_list[k] == NULL) {
								deploy_nodes_ctx->deploy_nodes_contact_list[k] = strdup(axctx->comap[i].node_child_contact_list[j]); 
								break;
							}
						}
					}
				}
				break;
			}
		}
	}
}

void get_deployable_aggregators_from_coord(struct axon *axctx, char *coord_contact_list, struct deploy_nodes_context *deploy_aggregators_ctx) {
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_coord_contact_list != NULL) {
			if(strcmp(axctx->comap[i].node_coord_contact_list, coord_contact_list) == 0) {
				if(axctx->comap[i].node_aggr_contact_list != NULL) {
					deploy_aggregators_ctx->num_deploy_nodes = 1;
					deploy_aggregators_ctx->deploy_nodes_contact_list = (char**) calloc(1, sizeof(char**));
					deploy_aggregators_ctx->deploy_nodes_contact_list[0] = strdup(axctx->comap[i].node_aggr_contact_list);
					break;
				}
			}
		}
	}
}

int check_is_aggregator_coord(struct axon *axctx, int num_deploy_aggregators, char **deploy_aggregators_contact_list, char *coord_contact_list) {
	int check = 0; // Why not just do strcmp of both aggregator and coord contact list, they should be same 
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_contact_list != NULL && (strcmp(axctx->comap[i].node_contact_list, coord_contact_list) == 0)) {
			if(axctx->comap[i].node_coord_contact_list != NULL 
				&& (strcmp(axctx->comap[i].node_coord_contact_list, deploy_aggregators_contact_list[num_deploy_aggregators - 1]) == 0)){
				check = 1;
				break;
			}
		}
	}
	if(check) {
		printf("Node marked aggregator : %s is also coordinator ...\n", deploy_aggregators_contact_list[0]);
	}
	return check;
}

int check_is_coord_aggregator(struct axon *axctx, char *coord_contact_list) {
	int check = 0; 
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_contact_list != NULL && (strcmp(axctx->comap[i].node_contact_list, coord_contact_list) == 0)) {
			if(axctx->comap[i].node_aggr_contact_list != NULL 
				&& (strcmp(axctx->comap[i].node_aggr_contact_list, coord_contact_list) == 0)) {
				check = 1;
				break;
			}
		}
	}
	if(check) {
		printf("Coordinator : %s marked is aggregator...\n", coord_contact_list);
	}
	return check;
}

struct control_comp *get_coord_control_comps(struct axon *axctx, char *coord_contact_list) {
	int check = 0; struct control_comp *ctrl_comp = NULL; 
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_contact_list != NULL) {
			if(strcmp(axctx->comap[i].node_contact_list, coord_contact_list) == 0) {
				ctrl_comp = (control_comp *) calloc(1, sizeof(struct control_comp));
				ctrl_comp->node_contact_list = axctx->comap[i].node_contact_list;
				ctrl_comp->node_coord_contact_list = axctx->comap[i].node_coord_contact_list;
				ctrl_comp->node_aggr_contact_list = axctx->comap[i].node_aggr_contact_list;
				ctrl_comp->node_actu_contact_list = axctx->comap[i].node_actu_contact_list;
				break;
			}
		}
	}
	return ctrl_comp;
}

int check_is_coord_control_comps_assigned(struct axon *axctx, char *coord_contact_list) {
	int check = 0; 
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_contact_list != NULL && (strcmp(axctx->comap[i].node_contact_list, coord_contact_list) == 0)) {
			if(axctx->comap[i].node_coord_contact_list != NULL && axctx->comap[i].node_aggr_contact_list != NULL 
				&& axctx->comap[i].node_actu_contact_list != NULL) {
				check = 1;
				break;
			}
		}
	}
	if(check) {
		printf("All control components are assigned to coordinator ...\n");
	}
	return check;
}

int check_is_node_coord(struct axon *axctx, char *coord_contact_list, char *node_contact_list) {
	int check = 0; int i = 0;
	for(int i = 0; i < NUM_COORDS ; i++) {
		if(axctx->comap[i].node_contact_list != NULL && (strcmp(axctx->comap[i].node_contact_list, coord_contact_list) == 0)) {
			check = 1;
			break;
		}
	}
	if(check) {
		printf("Node is marked as coordinator...\n");
	}
	return check;
}

void init_coord_node_state(struct cyton *cyctx, struct axon *axctx) {
	for(int i = 0; i < NUM_COORDS; i++) {
		if(axctx->comap[i].node_contact_list == NULL) {
			// TODO Change node contact list to populate from node coord contact list
			axctx->comap[i].node_contact_list = strdup(cyctx->nomap[0].node_contact_list);
			axctx->comap[i].node_coord_contact_list = strdup(cyctx->nomap[0].node_coord_contact_list);
			axctx->comap[i].node_aggr_contact_list = strdup(cyctx->nomap[0].node_aggr_contact_list);
			axctx->comap[i].node_actu_contact_list = strdup(cyctx->nomap[0].node_actu_contact_list);
			for(int j = 0; j < NUM_NODES; j++) {
				if(axctx->comap[i].node_child_contact_list[j] == NULL) {
					axctx->comap[i].node_child_contact_list[j] = strdup(cyctx->nomap[0].node_contact_list);
					break;
				}
			}
			break;
		}
	}
}

int check_if_nodes_deployed_count(struct axon *axctx, int num_to_deploy, char *coord_contact_list) {
	int found = 0; int num_deploy_count = 0;
	for(int i = 0; i < NUM_COORDS; i++) {
		if(strcmp(axctx->comap[i].node_coord_contact_list, coord_contact_list) == 0) {
			for(int j = 0; j < NUM_NODES; j++) {
				if(axctx->comap[i].node_child_contact_list[j] != NULL) {
					num_deploy_count += 1; 
				}
			}
			if(num_deploy_count == num_to_deploy) {
				found = 1;
			} 
		}
		if(found) break;
	}
	return found;
}

void add_coord_node_child_state(struct axon *axctx, struct node_boot *node_msg) {
	int found = 0;
	for(int i = 0; i < NUM_COORDS; i++) {
		if(strcmp(axctx->comap[i].node_coord_contact_list, (node_msg)->bootserver_contact_list) == 0) {
			for(int j = 0; j < NUM_NODES; j++) {
				if(axctx->comap[i].node_child_contact_list[j] == NULL) {
					axctx->comap[i].node_child_contact_list[j] = strdup((node_msg)->node_contact_list); 
					found = 1; 
					break;
				}
			}
			if(found) break;
		}
	}
}	
