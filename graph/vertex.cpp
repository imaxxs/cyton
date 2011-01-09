#include "vertex.h"

int *realize_vertex(CManager cm, int qid, struct func_rec frec, int copies, EVSimpleHandlerFunc handler, char *handler_data,
struct stone_context_list *stone_ctx_list, struct func_context_list *func_ctx_list, struct split_context_list *split_ctx_list) {
	printf("Started creating vertex for query_id : %d and for function : %s ....\n", qid, frec.func_name);
	if(frec.func_type == 3) { //Transform
		EVstone vstep_stone; EVaction vstep_action;
		EVstone vsplit_stone; EVaction vsplit_action;
		int pcopies = 0; int vsplits = 0;
		
		if(frec.flevel_start_point == 1) {
			pcopies = 1;
		} else {
			pcopies = copies;
		}
		vsplits = frec.func_splits;
		int *stone_ids = NULL;
		stone_ids = (int *) calloc(pcopies, sizeof(int));
		split_ctx_list->split_ctx_num = pcopies;
		if(split_ctx_list->split_ctx != NULL)
			free(split_ctx_list->split_ctx);
		split_ctx_list->split_ctx = (struct split_context *) calloc(pcopies, sizeof(struct split_context));
		init_split_context(pcopies, split_ctx_list->split_ctx);

		do {
			vstep_stone = EValloc_stone(cm);
			FMStructDescRec *in_format_list = get_format_list(frec.in_format);
			char *vstep_action_spec = create_transform_action_spec(in_format_list, get_format_list(frec.out_format), frec.func_name);
			vstep_action = EVassoc_immediate_action (cm, vstep_stone, vstep_action_spec, NULL);
			do {
				EVstone *targets = NULL;
				vsplit_stone = EValloc_stone (cm);
				vsplit_action = EVassoc_split_action(cm, vsplit_stone, targets);
				EVaction_set_output(cm, vstep_stone, vstep_action, 0, vsplit_stone);
				vsplits = vsplits - 1;
			} while(vsplits > 0);

			// Start Assignment
			split_ctx_list->split_ctx[pcopies - 1].split_stone = vsplit_stone;
			split_ctx_list->split_ctx[pcopies - 1].split_action = vsplit_action;
			// End Assignment

			if(frec.flevel_end_point == 1) { // Transform 
				if(stone_ctx_list[0].stone_ctx != NULL && stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id == -1) {
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
				}
				if(func_ctx_list[0].func_ctx != NULL && func_ctx_list[0].func_ctx[pcopies - 1].func_name == NULL) {
					func_ctx_list[0].func_ctx[pcopies - 1].func_name = strdup(frec.func_name); 
					func_ctx_list[0].func_ctx[pcopies - 1].in_format = strdup(frec.in_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].out_format = strdup(frec.out_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].func_order = frec.func_order;
					func_ctx_list[0].func_ctx[pcopies - 1].func_level = frec.func_level;
				}
			}
			if(frec.flevel_start_point == 1) { // Transform 
				if(stone_ctx_list[1].stone_ctx != NULL && stone_ctx_list[1].stone_ctx[0].stone_id == -1) {
					stone_ctx_list[1].stone_ctx[0].stone_id = vstep_stone;
					stone_ctx_list[1].stone_ctx[0].stone_action = vstep_action;
					stone_ctx_list[1].stone_ctx[0].split_stone = vsplit_stone;
					stone_ctx_list[1].stone_ctx[0].split_action = vsplit_action;
				}
				if(func_ctx_list[1].func_ctx != NULL && func_ctx_list[1].func_ctx[0].func_name == NULL) {
					func_ctx_list[1].func_ctx[0].func_name = strdup(frec.func_name); 
					func_ctx_list[1].func_ctx[0].in_format = strdup(frec.in_format); 
					func_ctx_list[1].func_ctx[0].out_format = strdup(frec.out_format); 
					func_ctx_list[1].func_ctx[0].func_order = frec.func_order;
					func_ctx_list[1].func_ctx[0].func_level = frec.func_level;
				}
			}
			stone_ids[pcopies - 1] = vstep_stone;
			printf("Created TRANSFORM stonenum %d with stone_id : %d stone_action_id : %d split_stone_id : %d and split_stone_action : %d....\n", 
			pcopies, vstep_stone, vstep_action, vsplit_stone, vsplit_action);
			pcopies = pcopies - 1;
		} while (pcopies > 0);
	return stone_ids;
	} else if(frec.func_type == 2) { //Filter
		EVstone vstep_stone; EVaction vstep_action; 
		EVstone vsplit_stone; EVaction vsplit_action;
		int pcopies = 0; int vsplits = 0;

		if(frec.flevel_start_point == 1) {
			pcopies = 1;
		} else {
			pcopies = copies;
		}
		vsplits = frec.func_splits;
		int *stone_ids = NULL;
		stone_ids = (int *) calloc(pcopies, sizeof(int));
		split_ctx_list->split_ctx_num = pcopies;
		if(split_ctx_list->split_ctx)
			free(split_ctx_list->split_ctx);
		split_ctx_list->split_ctx = (struct split_context *) calloc(pcopies, sizeof(struct split_context));
		init_split_context(pcopies, split_ctx_list->split_ctx);

		do {
			vstep_stone = EValloc_stone(cm);
			FMStructDescRec *in_format_list = get_format_list(frec.in_format);
			char *vstep_action_spec = create_filter_action_spec(in_format_list, frec.func_name);
			vstep_action = EVassoc_immediate_action (cm, vstep_stone, vstep_action_spec, NULL);
			do {
				EVstone *targets = NULL;
				vsplit_stone = EValloc_stone (cm);
				vsplit_action = EVassoc_split_action(cm, vsplit_stone, targets);
				EVaction_set_output(cm, vstep_stone, vstep_action, vsplits, vsplit_stone);
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			split_ctx_list->split_ctx[pcopies - 1].split_stone = -1;
			split_ctx_list->split_ctx[pcopies - 1].split_action = -1;
			split_ctx_list->split_ctx[pcopies - 1].split_stone = vsplit_stone;
			split_ctx_list->split_ctx[pcopies - 1].split_action = vsplit_action;

			if(frec.flevel_end_point == 1) { // Router 
				if(stone_ctx_list[0].stone_ctx != NULL && stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id == -1) {
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
				}
				if(func_ctx_list[0].func_ctx != NULL && func_ctx_list[0].func_ctx[pcopies - 1].func_name == NULL) {
					func_ctx_list[0].func_ctx[pcopies - 1].func_name = strdup(frec.func_name); 
					func_ctx_list[0].func_ctx[pcopies - 1].in_format = strdup(frec.in_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].out_format = strdup(frec.out_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].func_order = frec.func_order;
					func_ctx_list[0].func_ctx[pcopies - 1].func_level = frec.func_level;
				}
			}
			stone_ids[pcopies - 1] = vstep_stone;
			pcopies = pcopies - 1;
		} while (pcopies > 0);
		printf("Created FILTER with stone_id : %d and action_id : %d ....\n", vstep_stone, vstep_action);
	return stone_ids;
	} else if(frec.func_type == 4) { //Router
		EVstone vstep_stone; EVaction vstep_action; 
		EVstone vsplit_stone; EVaction vsplit_action;
		int pcopies = 0; int vsplits = 0;

		if(frec.flevel_start_point == 1) {
			pcopies = 1;
		} else {
			pcopies = copies;
		}
		vsplits = frec.func_splits;
		int *stone_ids = NULL;
		stone_ids = (int *) calloc(pcopies, sizeof(int));
		split_ctx_list->split_ctx_num = pcopies; // change this to vsplits
		if(split_ctx_list->split_ctx != NULL)
			free(split_ctx_list->split_ctx);
		split_ctx_list->split_ctx = (struct split_context *) calloc(vsplits, sizeof(struct split_context));
		init_split_context(vsplits, split_ctx_list->split_ctx);

		do {
			vstep_stone = EValloc_stone(cm);
			FMStructDescRec *in_format_list = get_format_list(frec.in_format);
			char *vstep_action_spec = create_router_action_spec(in_format_list, frec.func_name);
			vstep_action = EVassoc_immediate_action (cm, vstep_stone, vstep_action_spec, NULL);
			do {
				EVstone *targets = NULL;
				vsplit_stone = EValloc_stone (cm);
				vsplit_action = EVassoc_split_action(cm, vsplit_stone, targets);
				EVaction_set_output(cm, vstep_stone, vstep_action, vsplits - 1, vsplit_stone);
				split_ctx_list->split_ctx[vsplits - 1].split_stone = vsplit_stone;
				split_ctx_list->split_ctx[vsplits - 1].split_action = vsplit_action;
				vsplits = vsplits - 1;
			} while (vsplits > 0);

			if(frec.flevel_end_point == 1) { // Router 
				if(stone_ctx_list[0].stone_ctx != NULL && stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id == -1) {
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
				}
				if(func_ctx_list[0].func_ctx != NULL && func_ctx_list[0].func_ctx[pcopies - 1].func_name == NULL) {
					func_ctx_list[0].func_ctx[pcopies - 1].func_name = strdup(frec.func_name); 
					func_ctx_list[0].func_ctx[pcopies - 1].in_format = strdup(frec.in_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].out_format = strdup(frec.out_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].func_order = frec.func_order;
					func_ctx_list[0].func_ctx[pcopies - 1].func_level = frec.func_level;
				}
			}
			if(frec.flevel_start_point == 1) { // Router 
				if(stone_ctx_list[1].stone_ctx != NULL && stone_ctx_list[1].stone_ctx[0].stone_id == -1) {
					stone_ctx_list[1].stone_ctx[0].stone_id = vstep_stone;
					stone_ctx_list[1].stone_ctx[0].stone_action = vstep_action;
					stone_ctx_list[1].stone_ctx[0].split_stone = -1;
					stone_ctx_list[1].stone_ctx[0].split_action = -1;
				}
				if(func_ctx_list[1].func_ctx != NULL && func_ctx_list[1].func_ctx[0].func_name == NULL) {
					func_ctx_list[1].func_ctx[0].func_name = strdup(frec.func_name); 
					func_ctx_list[1].func_ctx[0].in_format = strdup(frec.in_format); 
					func_ctx_list[1].func_ctx[0].out_format = strdup(frec.out_format); 
					func_ctx_list[1].func_ctx[0].func_order = frec.func_order;
					func_ctx_list[1].func_ctx[0].func_level = frec.func_level;
				}
			}
			stone_ids[pcopies - 1] = vstep_stone;
			pcopies = pcopies - 1;
		} while (pcopies > 0);

		printf("Created ROUTER with stone_id : %d and action_id : %d ....\n", vstep_stone, vstep_action);
	return stone_ids;
	} else if(frec.func_type == 5) { //Multiqueue
		EVstone vstep_stone; EVaction vstep_action;
		EVstone vsplit_stone; EVaction vsplit_action;
		int pcopies = 0; int vsplits = 0;
		
		if(frec.flevel_start_point == 1) {
			pcopies = 1;
		} else {
			pcopies = copies;
		}
		vsplits = frec.func_splits;
		int *stone_ids = NULL;
		stone_ids = (int *) calloc(pcopies, sizeof(int));
		split_ctx_list->split_ctx_num = pcopies;
		if(split_ctx_list->split_ctx)
			free(split_ctx_list->split_ctx);
		split_ctx_list->split_ctx = (struct split_context *) calloc(pcopies, sizeof(struct split_context));
		init_split_context(pcopies, split_ctx_list->split_ctx);

		do {
			vstep_stone = EValloc_stone(cm);
			FMStructDescRec *in_format_list = get_format_list(frec.in_format);
			static FMStructDescList queue_list[] = {in_format_list, NULL};
			//char *vstep_action_spec = create_multityped_action_spec(queue_list, get_format_list(frec.out_format), frec.func_name);
			//vstep_action = EVassoc_multi_action (cm, vstep_stone, vstep_action_spec, NULL);
			do {
				EVstone *targets = NULL;
				vsplit_stone = EValloc_stone (cm);
				vsplit_action = EVassoc_split_action(cm, vsplit_stone, targets);
				EVaction_set_output(cm, vstep_stone, vstep_action, 0, vsplit_stone);
				vsplits = vsplits - 1;
			} while(vsplits > 0);

			split_ctx_list->split_ctx[pcopies - 1].split_stone = -1;
			split_ctx_list->split_ctx[pcopies - 1].split_action = -1;
			split_ctx_list->split_ctx[pcopies - 1].split_stone = vsplit_stone;
			split_ctx_list->split_ctx[pcopies - 1].split_action = vsplit_action;

			if(frec.flevel_end_point == 1) { // Multiqueue 
				if(stone_ctx_list[0].stone_ctx != NULL && stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id == -1) {
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_id = vstep_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].stone_action = vstep_action;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_stone = vsplit_stone;
					stone_ctx_list[0].stone_ctx[pcopies - 1].split_action = vsplit_action;
				}
				if(func_ctx_list[0].func_ctx != NULL && func_ctx_list[0].func_ctx[pcopies - 1].func_name == NULL) {
					func_ctx_list[0].func_ctx[pcopies - 1].func_name = strdup(frec.func_name); 
					func_ctx_list[0].func_ctx[pcopies - 1].in_format = strdup(frec.in_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].out_format = strdup(frec.out_format); 
					func_ctx_list[0].func_ctx[pcopies - 1].func_order = frec.func_order;
					func_ctx_list[0].func_ctx[pcopies - 1].func_level = frec.func_level;
				}
			}
			if(frec.flevel_start_point == 1) { // Multiqueue 
				if(stone_ctx_list[1].stone_ctx != NULL && stone_ctx_list[1].stone_ctx[0].stone_id == -1) {
					stone_ctx_list[1].stone_ctx[0].stone_id = vstep_stone;
					stone_ctx_list[1].stone_ctx[0].stone_action = vstep_action;
					stone_ctx_list[1].stone_ctx[0].split_stone = vsplit_stone;
					stone_ctx_list[1].stone_ctx[0].split_action = vsplit_action;
				}
				if(func_ctx_list[1].func_ctx != NULL && func_ctx_list[1].func_ctx[0].func_name == NULL) {
					func_ctx_list[1].func_ctx[0].func_name = strdup(frec.func_name); 
					func_ctx_list[1].func_ctx[0].in_format = strdup(frec.in_format); 
					func_ctx_list[1].func_ctx[0].out_format = strdup(frec.out_format); 
					func_ctx_list[1].func_ctx[0].func_order = frec.func_order;
					func_ctx_list[1].func_ctx[0].func_level = frec.func_level;
				}
			}
			stone_ids[pcopies - 1] = vstep_stone;
			pcopies = pcopies - 1;
		} while (pcopies > 0);

		printf("Created MULTIQUEUE with stone_id : %d and action_id : %d ....\n", vstep_stone, vstep_action);
	return stone_ids;
	} else if(frec.func_type == 6) { // Terminal
		EVstone terminal_stone; EVaction terminal_action; 

		int pcopies = copies;
		int *stone_ids = NULL;
		stone_ids = (int *) calloc(pcopies, sizeof(int));
		//typedef int(* EVSimpleHandlerFunc)(CManager cm, void *message, void *client_data, attr_list attrs)

		do {
			terminal_stone = EValloc_stone(cm);
			FMStructDescRec *in_format_list = get_format_list(frec.in_format);
			EVassoc_terminal_action(cm, terminal_stone, in_format_list, handler, strdup(handler_data));
			stone_ids[pcopies - 1] = terminal_stone;
			pcopies = pcopies - 1;
		} while (pcopies > 0);

		printf("Created TERMINAL with stone_id : %d ....\n", terminal_stone);
	return stone_ids;
	}
}

void reactivate_vertex() {
	
}

void passivate_vertex() {
	
}

void delete_vertex() {
	
}
