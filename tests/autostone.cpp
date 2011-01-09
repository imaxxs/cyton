#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
extern "C" {
#include "evpath.h"
#include "revpath.h"
#include "gen_thread.h"
}

typedef struct recs {
	double timestamp;
	char *attr_name;
	float rec_val;
} recs, *recs_ptr;

static FMField recs_field_list[] = 
{
	{"timestamp", "double", sizeof(double), FMOffset(recs_ptr, timestamp)},
	{"attr_name", "string", sizeof(char*), FMOffset(recs_ptr, attr_name)},
	{"rec_val", "float", sizeof(float), FMOffset(recs_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_format_list[] = 
{
    {"recs", recs_field_list, sizeof(recs), NULL},
    {NULL, NULL}
};

int get_cpu_mon_metrics_data(CManager cm, void *message, void *client_data, attr_list attrs) {
	do {
		printf("Get the cpu monitoring data ..");
		sleep(30);
	} while(1);
	return 1;
}

int main(int argc, char *argv[]) {
    attr_list node_contact_list = NULL;
    gen_pthread_init();
    CManager cm = CManager_create();
	CMfork_comm_thread(cm);

	EVstone cpu_mon_split_stone = EValloc_stone (cm);
	EVaction cpu_mon_split_action = EVassoc_split_action(cm, cpu_mon_split_stone, NULL);
	printf("CPU mon split stone id : %d and split action id : %d \n", cpu_mon_split_stone, cpu_mon_split_action);
		
	EVstone cpu_mon_metric_stone = EValloc_stone (cm);
	EVaction mon_metric_action = EVassoc_filter_action(cm, cpu_mon_metric_stone, recs_format_list, get_cpu_mon_metrics_data,
	cpu_mon_split_stone, NULL);
	EVaction_set_output(cm, cpu_mon_metric_stone, mon_metric_action, 0, cpu_mon_split_stone);
	EVenable_auto_stone(cm, cpu_mon_metric_stone, 10, 0);
}
