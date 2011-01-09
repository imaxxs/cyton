#include "edge.h"

void realize_edge(CManager cm, int qid, int pcopies, struct split_context_list *split_ctx_list, int *end_stone) {
	for(int i = 0; i < pcopies; i++) {
		printf("Created EDGE from split stone_id : %d and action_id : %d to end stone id : %d ....\n", split_ctx_list->split_ctx[i].split_stone,
		split_ctx_list->split_ctx[i].split_action, end_stone[i]);
		EVaction_add_split_target(cm, split_ctx_list->split_ctx[i].split_stone, split_ctx_list->split_ctx[i].split_action, end_stone[i]);
	}
}

void passivate_edge(struct edge_ctx *ectx, CManager cm) {

}

void reactivate_edge(struct edge_ctx *ectx, CManager cm) {

}
