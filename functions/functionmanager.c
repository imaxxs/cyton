#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "evpath.h"
#include "revpath.h"
#include "gen_thread.h"

typedef struct sensor_rec {
	double timestamp;
	char *attr_name;
	int rec_val_int;
	float rec_val_float;
	unsigned long rec_val;
} sensor_rec, *sensor_rec_ptr;

typedef struct sensor_recs {
	int sensor_recs_num;
	struct sensor_rec *sensor_recs;
} sensor_recs, *sensor_recs_ptr;

typedef struct count_sample_rec {
	char *metric_type;
	int sample_count;
} count_sample_rec, count_sample_rec_ptr;

typedef struct count_sample_recs {
	int csample_recs_num;
	struct count_sample_rec *count_sample_recs;
} count_sample_recs, *count_sample_recs_ptr;

typedef struct sample_window_recs {
	char *swindow_recs_type;
	int swindow_recs_num;
	int *swindow_recs;
} sample_window_recs, *sample_window_recs_ptr;

typedef struct recs_entropy {
	char *recs_type;
	int web_recs_num;
	float web_recs_entropy;
	int serv_recs_num;
	float serv_recs_entropy;
	int db_recs_num;
	float db_recs_entropy;
} recs_entropy, *recs_entropy_ptr;

/*
typedef struct global_entropy {
	int recs_num;
	struct recs_entropy *local_entropy;
	float web_recs_entropy;
	float serv_recs_entropy;
	float db_recs_entropy;
	float global_total_entropy;
} global_entropy, *global_entropy_ptr;
*/

typedef struct global_entropy {
	int global_recs_num;
	float global_entropy;
	float web_recs_entropy;
	float serv_recs_entropy;
	float db_recs_entropy;
} global_entropy, *global_entropy_ptr;

typedef struct entropy_trace {
    float *entropy_apache;
    float *entropy_mysql;
    float *entropy_tomcat;
} entropy_trace, *entropy_trace_p;

/*
typedef struct recs {
	double timestamp;
	char *attr_name;
	int num_recs;
	float *rec_val;
} recs, *recs_ptr;

typedef struct recs_range {
	char *attr_name;
	float key_srng;
	float key_erng;
	int num_recs;
	float *rec_val;
} recs_range, *recs_range_ptr;

typedef struct recs_range_level {
	char *attr_name;
	float key_srng;
	float key_erng;
	int range_level;
	int num_recs;
	float *rec_val;
} recs_range_level, *recs_range_level_ptr;

typedef struct recs_range_level_list {
	int num_range_recs;
	struct recs_range_level *recs_range;
} recs_range_level_list, *recs_range_level_list_ptr;

typedef struct rec_stats {
	char *attr_name;
	float key_srng;
	float key_erng;
	int rec_int_cnt;
	float rec_float_cnt;
} rec_stats, *rec_stats_ptr;

typedef struct recs_stats_range_list {
	int num_stats_recs;
	struct rec_stats *recs_stats;
} recs_stats_range_list, *recs_stats_range_list_ptr;

typedef struct rec_stats_threshold {
	char *attr_name;
	float key_srng;
	float key_erng;
	int threshold_val;
	int rec_int_cnt;
	float rec_float_cnt;
} rec_stats_threshold, *rec_stats_threshold_ptr;

typedef struct recs_stats_threshold_range_list {
	int num_stats_recs;
	struct rec_stats_threshold *recs_stats_threshold;
} recs_stats_threshold_range_list, *recs_stats_threshold_range_list_ptr;

typedef struct recs_entropy_stats {
	char *attr_name;
	int total_recs;
	float recs_entropy;
} recs_entropy_stats, *recs_entropy_stats_ptr;

typedef struct recs_entropy_stats_threshold {
	char *attr_name;
	int total_recs;
	float recs_entropy;
	float recs_entropy_threshold;
} recs_entropy_stats_threshold, *recs_entropy_stats_threshold_ptr;

struct recs *precs;
struct recs_range *precs_range;
struct recs_range_level *precs_range_level;
struct recs_range_level_list *precs_range_level_list;
struct recs_stats_range_list *precs_stats_range_list;
struct recs_stats_threshold_range_list *precs_stats_threshold_range_list;
*/

struct sensor_recs *psensor_recs;
struct sensor_recs *cpu_psensor_recs;
struct sensor_recs *mem_psensor_recs;
struct sensor_recs *load_psensor_recs;
struct sensor_recs *apache_psensor_recs;
struct sensor_recs *rubis_psensor_recs;
struct global_entropy *global_entropy_psensor_recs;
struct count_sample_recs *pcount_sample_recs;
struct sample_window_recs *apache_busy_swindow_recs;
struct sample_window_recs *rubis_fault_swindow_recs;

static time_t start; static time_t now; static double elapsed; 
static int event_buffer_size; static int dummy_event_buffer_size;
static double sampling_window; static int sampling_rate; static int marker_event = 1;

int cpu_event_buffer_size = 0; int cpu_dummy_event_buffer_size = 0; int cpu_marker_event = 1;
int mem_event_buffer_size = 0; int mem_dummy_event_buffer_size = 0; int mem_marker_event = 1;
int load_event_buffer_size = 0; int load_dummy_event_buffer_size = 0; int load_marker_event = 1;
int apache_event_buffer_size = 0; int apache_dummy_event_buffer_size = 0; int apache_marker_event = 1;
int apache_busy_event_buffer_size = 0; int apache_busy_dummy_event_buffer_size = 0; int apache_busy_marker_event = 1;
int rubis_event_buffer_size = 0; int rubis_dummy_event_buffer_size = 0; int rubis_marker_event = 1;
int rubis_fault_event_buffer_size = 0; int rubis_fault_dummy_event_buffer_size = 0; int rubis_fault_marker_event = 1;
int global_entropy_event_buffer_size = 0; int global_entropy_dummy_event_buffer_size = 0; int global_entropy_marker_event = 1;

int apache_load_sat_sample_count = 0;
int rubis_fault_sample_count = 0;
float global_entropy_count = 0;

int apache_tx_sample_bit = 0; int tomcat_tx_sample_bit = 0; int mysql_tx_sample_bit = 0;

int apache_3_tx_sample_bit = 0; int tomcat_3_tx_sample_bit = 0; int mysql_3_tx_sample_bit = 0;
int apache_4_tx_sample_bit = 0; int tomcat_4_tx_sample_bit = 0; int mysql_4_tx_sample_bit = 0;
int apache_5_tx_sample_bit = 0; int tomcat_5_tx_sample_bit = 0; int mysql_5_tx_sample_bit = 0;
int apache_6_tx_sample_bit = 0; int tomcat_6_tx_sample_bit = 0; int mysql_6_tx_sample_bit = 0;

int dom0_3_ctime_sample_bit = 0; int dom0_4_ctime_sample_bit = 0; int dom0_5_ctime_sample_bit = 0; int dom0_6_ctime_sample_bit = 0;
int web_3_ctime_sample_bit = 0; int web_4_ctime_sample_bit = 0; int web_5_ctime_sample_bit = 0; int web_6_ctime_sample_bit = 0;
int serv_3_ctime_sample_bit = 0; int serv_4_ctime_sample_bit = 0; int serv_5_ctime_sample_bit = 0; int serv_6_ctime_sample_bit = 0;
int db_3_ctime_sample_bit = 0; int db_4_ctime_sample_bit = 0; int db_5_ctime_sample_bit = 0; int db_6_ctime_sample_bit = 0;

int ch1_bytes_recv_sample_bit = 0; int ch3_bytes_recv_sample_bit = 0; int ch4_bytes_recv_sample_bit = 0; int ch5_bytes_recv_sample_bit = 0;
int ch6_bytes_recv_sample_bit = 0;

/*
int entropy_stats_threshold_trigger(recs_entropy_stats_threshold_ptr input) {
	printf("Inside the libfunctions.la RECS_STATS_THRESHOLD_TRIGGER function .... \n");

	printf("Recs entropy value : %d and entropy threshold value : %d ...\n", 
	((recs_entropy_stats_threshold_ptr)input)->recs_entropy,
	((recs_entropy_stats_threshold_ptr)input)->recs_entropy_threshold);

	if(((recs_entropy_stats_threshold_ptr)input)->recs_entropy >
		((recs_entropy_stats_threshold_ptr)input)->recs_entropy_threshold) {
			printf("ENTROPY THRESHOLD TRIGGER ACTIVATED ..\n");
		return 0;
	} else {
		return -1;
	}
}

int append_entropy_stats_threshold (recs_entropy_stats_ptr input, recs_entropy_stats_threshold_ptr output) {
	printf("Inside the libfunctions.la APPEND_ENTROPY_STATS_THRESHOLD function .... \n");

	if(input->attr_name != NULL) {
		output->attr_name = strdup(input->attr_name);
	}
	output->total_recs = input->total_recs;
	output->recs_entropy = input->recs_entropy;
	output->recs_entropy_threshold = 0.5;

	printf("OUT RECORD FROM ENTROPY_STATS_THRESHOLD FUNCTION === \n");
	printf("OUT RECORD ==> recs entropy threshold attr_name : %s ...\n", output->attr_name);
	printf("OUT RECORD ==> recs entropy threshold total_recs : %d ...\n", output->total_recs);
	printf("OUT RECORD ==> recs entropy threshold entropy val : %f ...\n", output->recs_entropy);
	printf("OUT RECORD ==> recs entropy threshold entropy val threshold : %f ...\n", output->recs_entropy_threshold);

	return 1;
}

int entropy_stats(recs_stats_range_list_ptr input, recs_entropy_stats_ptr output) {
printf("Inside the libfunctions.la ENTROPY_STATS function .... \n");
	int num_stats = input->num_stats_recs;
	int i; int j; int total_rec_cnt = 0;
	float entropy_val = 0.0; float prob = 0.0;
	char *entropy_rec_name = NULL;
	for (i = 0; i < num_stats; i++) {
		if(i > 0) {
			if(entropy_rec_name != NULL && strcmp(input->recs_stats[i].attr_name, entropy_rec_name) != 0) {
				return 0;
			}
		} 
		if(input->recs_stats[i].attr_name != NULL) {
			entropy_rec_name = strdup(input->recs_stats[i].attr_name);
		}
		if(input->recs_stats[i].rec_int_cnt == -1 || input->recs_stats[i].rec_int_cnt == 0) {
			return 0;
		}
		total_rec_cnt = total_rec_cnt + input->recs_stats[i].rec_int_cnt;
		if(total_rec_cnt == 0) {
			return 0;
		}
	}  
	for (j = 0; j < num_stats; j++) {
		prob = ((input->recs_stats[i]).rec_int_cnt)/total_rec_cnt;
		printf("ENTROPY_STATS FUNCTION PROB VALUE === %f \n", prob);
		entropy_val = entropy_val - prob*log2(prob);
		printf("ENTROPY_STATS FUNCTION ENTROPY VALUE === %f \n", entropy_val);
	} 
	output->attr_name = strdup(entropy_rec_name);
	output->total_recs = total_rec_cnt;
	output->recs_entropy = entropy_val;

	printf("OUT RECORD FROM ENTROPY_STATS FUNCTION === \n");
	printf("OUT RECORD ==> recs entropy attr_name : %s ...\n", output->attr_name);
	printf("OUT RECORD ==> recs entropy total_rec_cnt : %d ...\n", output->total_recs);
	printf("OUT RECORD ==> recs entropy entropy val : %f ...\n", output->recs_entropy);
	return 1;
}

int recs_stats_threshold_trigger(recs_stats_threshold_range_list_ptr input) {
	printf("Inside the libfunctions.la RECS_STATS_THRESHOLD_TRIGGER function .... \n");

	int num_stats_recs = ((recs_stats_threshold_range_list_ptr)input)->num_stats_recs;
	int i;
	for(i = 0; i < num_stats_recs; i++) {
		printf("Recs count : %d and threshold value : %d ...\n", 
		((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].rec_int_cnt,
		((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].threshold_val);

		if(((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].rec_int_cnt >
			((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].threshold_val) {
				printf("THRESHOLD_TRIGGER ACTIVATED as recs count : %d crossed threshold value : %d ...\n", 
				((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].rec_int_cnt,
				((recs_stats_threshold_range_list_ptr)input)->recs_stats_threshold[i].threshold_val);
			return 0;
		} else {
			return -1;
		}
	}
}

int append_recs_stats_threshold (recs_stats_range_list_ptr input, recs_stats_threshold_range_list_ptr output) {
	printf("Inside the libfunctions.la APPEND_RECS_STATS_THRESHOLD function .... \n");

	int num_stats_recs = input->num_stats_recs;
	output->num_stats_recs = input->num_stats_recs;

	precs_stats_threshold_range_list = output;
	precs_stats_threshold_range_list->recs_stats_threshold = (struct rec_stats_threshold *) calloc (num_stats_recs, sizeof(struct rec_stats_threshold));

	int i;
	for(i = 0; i < num_stats_recs; i++) {
		precs_stats_threshold_range_list->recs_stats_threshold[i].attr_name = NULL;
		precs_stats_threshold_range_list->recs_stats_threshold[i].key_srng = -1;
		precs_stats_threshold_range_list->recs_stats_threshold[i].key_erng = -1;
		precs_stats_threshold_range_list->recs_stats_threshold[i].rec_int_cnt = -1;
		precs_stats_threshold_range_list->recs_stats_threshold[i].rec_float_cnt = -1;
		precs_stats_threshold_range_list->recs_stats_threshold[i].threshold_val = -1;
	}

	int j;
	for(j = 0; j < num_stats_recs; j++) {
		if(input->recs_stats[j].attr_name != NULL) {
			precs_stats_threshold_range_list->recs_stats_threshold[j].attr_name = strdup(input->recs_stats[j].attr_name);
		}
		precs_stats_threshold_range_list->recs_stats_threshold[j].key_srng = input->recs_stats[j].key_srng;				
		precs_stats_threshold_range_list->recs_stats_threshold[j].key_erng = input->recs_stats[j].key_erng;				
		precs_stats_threshold_range_list->recs_stats_threshold[j].threshold_val = 20;				
		precs_stats_threshold_range_list->recs_stats_threshold[j].rec_int_cnt = input->recs_stats[j].rec_int_cnt;				
		precs_stats_threshold_range_list->recs_stats_threshold[j].rec_float_cnt = input->recs_stats[j].rec_float_cnt;				
	}
	printf("OUT RECORD FROM APPEND_RECS_STATS_THRESHOLD FUNCTION === \n");
	printf("OUT RECORD ==> num_stats_recs : %d ...\n", output->num_stats_recs);
	printf("OUT RECORD ==> recs stats threshold level 1 attr_name : %s ...\n", output->recs_stats_threshold[0].attr_name);
	printf("OUT RECORD ==> recs stats threshold level 1 key_srng : %f ...\n", output->recs_stats_threshold[0].key_srng);
	printf("OUT RECORD ==> recs stats threshold level 1 key_erng : %f ...\n", output->recs_stats_threshold[0].key_erng);
	printf("OUT RECORD ==> recs stats threshold level 1 threshold val : %d ...\n", output->recs_stats_threshold[0].threshold_val);
	printf("OUT RECORD ==> recs stats threshold level 1 rec_int_cnt : %d ...\n", output->recs_stats_threshold[0].rec_int_cnt);
	printf("OUT RECORD ==> recs stats threshold level 1 rec_float_cnt : %f ...\n", output->recs_stats_threshold[0].rec_float_cnt);
	return 1;
}

int recs_range_stats_list (recs_range_level_list_ptr input, recs_stats_range_list_ptr output) {
	printf("Inside the libfunctions.la RECS_RANGE_STATS_LIST function .... \n");

	int num_range_recs = input->num_range_recs;
	output->num_stats_recs = input->num_range_recs;
	int num_stats_recs = output->num_stats_recs;

	precs_stats_range_list = output;
	precs_stats_range_list->recs_stats = (struct rec_stats *) calloc (num_stats_recs, sizeof(struct rec_stats));

	int i;
	for(i = 0; i < num_stats_recs; i++) {
		precs_stats_range_list->recs_stats[i].attr_name = NULL;
		precs_stats_range_list->recs_stats[i].key_srng = -1;
		precs_stats_range_list->recs_stats[i].key_erng = -1;
		precs_stats_range_list->recs_stats[i].rec_int_cnt = -1;
		precs_stats_range_list->recs_stats[i].rec_float_cnt = -1;
	}

	int j;
	for(j = 0; j < num_range_recs; j++) {
		float start = input->recs_range[j].key_srng;
		float end = input->recs_range[j].key_erng;
		int num_recs = input->recs_range[j].num_recs;
		if(num_recs != 0) {
			int k;
			for(k = 0; k < num_recs; k++) {
				if(start < input->recs_range[j].rec_val[k]  && input->recs_range[j].rec_val[k] < end) {
					precs_stats_range_list->recs_stats[j].rec_int_cnt += 1;				
				}
			}
			precs_stats_range_list->recs_stats[j].key_srng += start;				
			precs_stats_range_list->recs_stats[j].key_erng += end;				
			precs_stats_range_list->recs_stats[j].attr_name = strdup(input->recs_range[j].attr_name);
			precs_stats_range_list->recs_stats[j].rec_float_cnt = -1;
		} else {
			return 0;
		}
	}
	printf("OUT RECORD FROM RECS_RANGE_STATS_LIST FUNCTION === \n");
	printf("OUT RECORD ==> num_stats_recs : %d ...\n", output->num_stats_recs);
	printf("OUT RECORD ==> recs stats level 1 attr_name : %s ...\n", output->recs_stats[0].attr_name);
	printf("OUT RECORD ==> recs stats level 1 key_srng : %f ...\n", output->recs_stats[0].key_srng);
	printf("OUT RECORD ==> recs stats level 1 key_erng : %f ...\n", output->recs_stats[0].key_erng);
	printf("OUT RECORD ==> recs stats level 1 rec_int_cnt : %d ...\n", output->recs_stats[0].rec_int_cnt);
	printf("OUT RECORD ==> recs stats level 1 rec_float_cnt : %f ...\n", output->recs_stats[0].rec_float_cnt);
	return 1;
}

int recs_range_levels(recs_range_level_ptr input, recs_range_level_list_ptr output) {
	printf("Inside the libfunctions.la RECS_RANGE_LEVELS function .... \n");

	float start = input->key_srng;
	float end = input->key_erng;
	int range_level = input->range_level;
	float step = (end - start)/(range_level);
	int num_recs = input->num_recs;

	precs_range_level_list = output;
	output->num_range_recs = range_level;
	precs_range_level_list->recs_range = (struct recs_range_level *) calloc (range_level, sizeof(struct recs_range_level));

	int i; int j; int k; int l;
	for(i = 0; i < range_level; i++) {
		int num_per_range_recs = 0;
		for(j = 0; j < num_recs; j++) {
			if(start < input->rec_val[j] && input->rec_val[j] < (start+step)) {
				num_per_range_recs += 1;				
			}
		}
		precs_range_level_list->recs_range[i].num_recs = num_per_range_recs;
		precs_range_level_list->recs_range[i].rec_val = (float *)calloc(num_per_range_recs, sizeof(float*));
		for(k = 0; k < num_recs; k++) {
			if(start < input->rec_val[k] && input->rec_val[k] < (start + step)) {
				for(l = 0; l < num_per_range_recs; l++) {
					if(output->recs_range[i].rec_val[l] == -1) {
						precs_range_level_list->recs_range[i].rec_val[l] = input->rec_val[k];
						break;
					}
				}
			}
		}
		precs_range_level_list->recs_range[i].key_srng = start;				
		precs_range_level_list->recs_range[i].key_erng = start + step;				
		precs_range_level_list->recs_range[i].range_level = range_level;				
		precs_range_level_list->recs_range[i].attr_name = strdup(input->attr_name);
		start = start + step;
		if(start == end) break;
	}
	printf("OUT RECORD FROM RECS RANGE LEVEL FUNCTION === \n");
	printf("OUT RECORD ==> num_range_recs : %d ...\n", output->num_range_recs);
	printf("OUT RECORD ==> recs range level 1 attr_name : %s ...\n", output->recs_range[0].attr_name);
	printf("OUT RECORD ==> recs range level 1 key_srng : %f ...\n", output->recs_range[0].key_srng);
	printf("OUT RECORD ==> recs range level 1 key_erng : %f ...\n", output->recs_range[0].key_erng);
	printf("OUT RECORD ==> recs range_level 1 range_level : %d ...\n", output->recs_range[0].range_level);
	printf("OUT RECORD ==> recs range level 1 num_recs : %d ...\n", output->recs_range[0].num_recs);
	printf("OUT RECORD ==> recs range level 1 at index : %d rec_val : %f ...\n", 10, output->recs_range[0].rec_val[10]);
	return 1;
}

int append_level(recs_range_ptr input, recs_range_level_ptr output) {
	printf("Inside the libfunctions.la APPEND_LEVEL function .... \n");

	output->attr_name = strdup(input->attr_name);
	output->key_srng = input->key_srng;
	output->key_erng = input->key_erng;
	output->num_recs = input->num_recs;
	int num_recs = input->num_recs;

	precs_range_level = output;
	precs_range_level->rec_val = (float *) calloc (num_recs, sizeof(float*));

	int i;
	for(i = 0; i < num_recs; i++) {
		output->rec_val[i] = input->rec_val[i];
	}
	output->range_level=1;
	printf("OUT RECORD FROM APPEND LEVEL FUNCTION === \n");
	printf("OUT RECORD ==> attr_name : %s ...\n", output->attr_name);
	printf("OUT RECORD ==> key_srng : %f ...\n", output->key_srng);
	printf("OUT RECORD ==> key_erng : %f ...\n", output->key_erng);
	printf("OUT RECORD ==> range_level : %d ...\n", output->range_level);
	printf("OUT RECORD ==> num_recs : %d ...\n", output->num_recs);
	printf("OUT RECORD ==> at index : %d rec_val : %f ...\n", 10, output->rec_val[10]);
	return 1;
}

int append_range(recs_ptr input, recs_range_ptr output) {
	printf("Inside the libfunctions.la APPEND_RANGE function .... \n");

	output->attr_name = strdup(input->attr_name);
	output->key_srng = 59;
	output->key_erng = 101;
	output->num_recs = input->num_recs;
	int num_recs = input->num_recs;

	precs_range = output;
	precs_range->rec_val = (float *) calloc (num_recs, sizeof(float*));

	int i;
	for(i = 0; i < num_recs; i++) {
		output->rec_val[i] = input->rec_val[i];
	}
	printf("OUT RECORD FROM APPEND RANGE FUNCTION === \n");
	printf("OUT RECORD ==> attr_name : %s ...\n", output->attr_name);
	printf("OUT RECORD ==> key_srng : %f ...\n", output->key_srng);
	printf("OUT RECORD ==> key_erng : %f ...\n", output->key_erng);
	printf("OUT RECORD ==> num_recs : %d ...\n", output->num_recs);
	printf("OUT RECORD ==> at index : %d rec_val : %f ...\n", 0, output->rec_val[0]);
	return 1;
}

int rec_window(sensor_rec_ptr input, recs_ptr output) {
	printf("Inside the libfunctions.la REC_WINDOW function .... \n");
	if(marker_event) {
		start = time(NULL);
		sampling_rate = 5; //input->sampling_rate;
		sampling_window = 10; //input->sampling_window;
		event_buffer_size = sampling_window/sampling_rate;
		dummy_event_buffer_size = 0;
		precs = (struct recs *) calloc(1, sizeof(struct recs));  
		precs->rec_val = (float *) calloc (event_buffer_size, sizeof(float*));
		marker_event = 0;
		return 0;
	} else {
		now = time(NULL);
		elapsed = difftime(now, start);
		if(elapsed < sampling_window) {
			printf("Inside the libfunctions.la REC_WINDOW function ELAPSED TIME : %f .... \n", elapsed);
			precs->rec_val[dummy_event_buffer_size] = input->rec_val;
			dummy_event_buffer_size = dummy_event_buffer_size + 1;
			return 0;
		} else {
			printf("Inside the libfunctions.la REC_WINDOW function Sampling Window is Crossed ....\n");
			start = 0; now = 0; marker_event = 1;  
			//precs = output;
			output->attr_name = strdup(input->attr_name);
			output->num_recs = dummy_event_buffer_size;
			output->rec_val = (float *) calloc (dummy_event_buffer_size, sizeof(float*));

			int i = 0;
			for(i; i < dummy_event_buffer_size; i++) {
				if(precs->rec_val[i] != 0) {
					output->rec_val[i] = precs->rec_val[i];
				}
			}
			dummy_event_buffer_size = 0; event_buffer_size = 0;
			printf("OUT RECORD FROM REC WINDOW FUNCTION === \n");
			printf("OUT RECORD ==> attr_name : %s ...\n", output->attr_name);
			printf("OUT RECORD ==> num_recs : %d ...\n", output->num_recs);
			return 1;
		}
	}
}

int rec_group_map(sensor_rec_ptr input) {
	printf("Inside the libfunctions.la REC_GROUP function .... \n");
	if(input->attr_name == NULL) return 0;
	if(strstr(input->attr_name, "cpu")) {
		return 2;
	} else if(strstr(input->attr_name, "mem")) {
		return 1;
	} else if(strstr(input->attr_name, "load")) {
		return 0;
	} else {
		return -1;
	}
}

int rec_load_group_map(sensor_rec_ptr input) {
	printf("Inside the libfunctions.la REC_GROUP function .... \n");
	if(input->attr_name == NULL) return 0;
	if(strstr(input->attr_name, "load")) { // May need to change to "load"
		return 0;
	} else {
		return -1;
	}
}
*/

int rec_apache_group_map(sensor_rec_ptr input) {
	printf("Inside the libfunctions.la REC_GROUP function .... \n");
	if(input->attr_name == NULL) return 0;
	if(strstr(input->attr_name, "apache")) { // May need to change to "load"
        printf("REC_GROUP sensor attr value : %s \n", input->attr_name);
		printf("REC_GROUP sensor rec value_int : %d \n", input->rec_val_int);
		printf("REC_GROUP sensor rec value_float : %f \n", input->rec_val_float);
		printf("REC_GROUP sensor rec value_long : %lu \n", input->rec_val);
		return 0;
	} else {
		return -1;
	}
}

int rec_fault_group_map(sensor_rec_ptr input) {
	printf("Inside the libfunctions.la REC_GROUP function .... \n");
	if(input->attr_name == NULL) return 0;
	if(strstr(input->attr_name, "rubis_")) {
        printf("REC_GROUP sensor attr value : %s \n", input->attr_name);
		printf("REC_GROUP sensor rec value_int : %d \n", input->rec_val_int);
		printf("REC_GROUP sensor rec value_float : %d \n", input->rec_val_float);
		printf("REC_GROUP sensor rec value_long : %lu \n", input->rec_val);
		return 0;
	} else {
		return -1;
	}
}

/*
int rec_time_window(sensor_rec_ptr input, sensor_recs_ptr output) {
printf("Inside the libfunctions.la REC_TIME_WINDOW function .... \n");
	//if(marker_event) {
		start = time(NULL);
		sampling_rate = input->sampling_rate;
		sampling_window = input->sampling_window;
		event_buffer_size = sampling_window/sampling_rate;
		dummy_event_buffer_size = 0;
		psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
		psensor_recs->sensor_recs_num = event_buffer_size;
		psensor_recs->sensor_recs = (struct sensor_rec *) calloc (event_buffer_size, sizeof(struct sensor_rec));
		marker_event = 0;
		return 0;
	} else {
		now = time(NULL);
		elapsed = difftime(now, start);
		if(elapsed < sampling_window) {
			printf("Inside the libfunctions.la REC_TIME_WINDOW function ELAPSED TIME : %f .... \n", elapsed);
			psensor_recs->sensor_recs[dummy_event_buffer_size].timestamp = input->timestamp;
			psensor_recs->sensor_recs[dummy_event_buffer_size].attr_name = strdup(input->attr_name);
			psensor_recs->sensor_recs[dummy_event_buffer_size].rec_val = input->rec_val;
			dummy_event_buffer_size = dummy_event_buffer_size + 1;
			return 0;
		} else {
			printf("Inside the libfunctions.la REC_TIME_WINDOW function TIME WINDOW is Crossed ....\n");
			start = 0; now = 0; marker_event = 1;  
			psensor_recs = output;
			output->sensor_recs_num = dummy_event_buffer_size - 1;

			int i = 0;
			for(i; i < dummy_event_buffer_size; i++) {
				//if(!psensor_recs->sensor_recs[i].timestamp) {
					output->sensor_recs[i].timestamp = psensor_recs->sensor_recs[i].timestamp;
					output->sensor_recs[i].attr_name = strdup(psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val = psensor_recs->sensor_recs[i].rec_val;
				//}
			}
			dummy_event_buffer_size = 0; event_buffer_size = 0;
			printf("OUT RECORD FROM REC WINDOW FUNCTION === \n");
			printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
			printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
			return 1;
		}
	}//
}
*/

int rec_event_window(sensor_rec_ptr input, sensor_recs_ptr output) {
	//printf("Inside the libfunctions.la REC_EVENT_WINDOW function .... \n");
	if(input == NULL) return 0;	
	if(input->attr_name == NULL) return 0;	

	if(strstr(input->attr_name, "cpu") != 0) { 
		if(cpu_marker_event) {
			cpu_event_buffer_size = 10;
			//printf("REC_EVENT_WINDOW function CPU_EVENT_BUFFER_SIZE %d.... \n", cpu_event_buffer_size);
			cpu_dummy_event_buffer_size = 1;
			cpu_psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
			cpu_psensor_recs->sensor_recs_num = cpu_event_buffer_size;
			cpu_psensor_recs->sensor_recs = (struct sensor_rec *) calloc (cpu_event_buffer_size, sizeof(struct sensor_rec));
			cpu_psensor_recs->sensor_recs[0].timestamp = input->timestamp;
			if(input->attr_name != NULL)
				cpu_psensor_recs->sensor_recs[0].attr_name = strdup(input->attr_name);
			cpu_psensor_recs->sensor_recs[0].rec_val = input->rec_val;
			cpu_marker_event = 0;
			return 0;
		} else {
			printf("REC_EVENT_WINDOW function CPU_EVENT_BUFFER_SIZE %d.... \n", cpu_event_buffer_size);
			printf("REC_EVENT_WINDOW function CPU_DUMMY_EVENT_BUFFER_SIZE %d.... \n", cpu_dummy_event_buffer_size);
			if(cpu_dummy_event_buffer_size < cpu_event_buffer_size ) {
				//printf("REC_EVENT_WINDOW function CPU ELAPSED WINDOW SIZE : %d \n",(cpu_event_buffer_size - cpu_dummy_event_buffer_size));
				cpu_psensor_recs->sensor_recs[cpu_dummy_event_buffer_size].timestamp = input->timestamp;
				if(input->attr_name != NULL)
					cpu_psensor_recs->sensor_recs[cpu_dummy_event_buffer_size].attr_name = strdup(input->attr_name);
				cpu_psensor_recs->sensor_recs[cpu_dummy_event_buffer_size].rec_val = input->rec_val;
				cpu_dummy_event_buffer_size = cpu_dummy_event_buffer_size + 1;
				return 0;
			} else {
				printf("REC_EVENT_WINDOW function CPU EVENT WINDOW Crossed ....\n");
				cpu_marker_event = 1;  
				//output = cpu_psensor_recs;
				output->sensor_recs_num = cpu_event_buffer_size; 
				output->sensor_recs = (struct sensor_rec *) calloc (output->sensor_recs_num, sizeof(struct sensor_rec));

				int i = 0;
				for(i; i < cpu_event_buffer_size; i++) {
					output->sensor_recs[i].timestamp = cpu_psensor_recs->sensor_recs[i].timestamp;
					if(cpu_psensor_recs->sensor_recs[i].attr_name != NULL)
						output->sensor_recs[i].attr_name = strdup(cpu_psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val = cpu_psensor_recs->sensor_recs[i].rec_val;
				}

				cpu_dummy_event_buffer_size = 0; cpu_event_buffer_size = 0;
				free(cpu_psensor_recs);
				printf("OUT RECORD FROM CPU EVENT WINDOW === \n");
				printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
				printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
				return 1;
			}
		}
	} else if(strstr(input->attr_name, "mem") != 0) { 
		if(mem_marker_event) {
			mem_event_buffer_size = 10;
			printf("REC_EVENT_WINDOW function MEM_EVENT_BUFFER_SIZE %d.... \n", mem_event_buffer_size);
			mem_dummy_event_buffer_size = 1;
			mem_psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
			mem_psensor_recs->sensor_recs_num = mem_event_buffer_size;
			mem_psensor_recs->sensor_recs = (struct sensor_rec *) calloc (mem_event_buffer_size, sizeof(struct sensor_rec));
			mem_psensor_recs->sensor_recs[0].timestamp = input->timestamp;
			if(input->attr_name != NULL)
				mem_psensor_recs->sensor_recs[0].attr_name = strdup(input->attr_name);
			mem_psensor_recs->sensor_recs[0].rec_val = input->rec_val;
			mem_marker_event = 0;
			return 0;
		} else {
			printf("REC_EVENT_WINDOW function MEM_EVENT_BUFFER_SIZE %d.... \n", mem_event_buffer_size);
			printf("REC_EVENT_WINDOW function MEM_DUMMY_EVENT_BUFFER_SIZE %d.... \n", mem_dummy_event_buffer_size);
			if(mem_dummy_event_buffer_size < mem_event_buffer_size) {
				//printf("REC_EVENT_WINDOW function MEM ELAPSED WINDOW SIZE : %d \n",(mem_event_buffer_size - mem_dummy_event_buffer_size));
				mem_psensor_recs->sensor_recs[mem_dummy_event_buffer_size].timestamp = input->timestamp;
				if(input->attr_name != NULL)
					mem_psensor_recs->sensor_recs[mem_dummy_event_buffer_size].attr_name = strdup(input->attr_name);
				mem_psensor_recs->sensor_recs[mem_dummy_event_buffer_size].rec_val = input->rec_val;
				mem_dummy_event_buffer_size = mem_dummy_event_buffer_size + 1;
				return 0;
			} else {
				printf("REC_EVENT_WINDOW function MEM EVENT WINDOW Crossed ....\n");
				mem_marker_event = 1;  
				//output = mem_psensor_recs;
				output->sensor_recs_num = mem_event_buffer_size; 
				output->sensor_recs = (struct sensor_rec *) calloc (output->sensor_recs_num, sizeof(struct sensor_rec));

				int i = 0;
				for(i; i < mem_event_buffer_size; i++) {
					output->sensor_recs[i].timestamp = mem_psensor_recs->sensor_recs[i].timestamp;
					if(mem_psensor_recs->sensor_recs[i].attr_name != NULL)
						output->sensor_recs[i].attr_name = strdup(mem_psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val = mem_psensor_recs->sensor_recs[i].rec_val;
				}

				mem_dummy_event_buffer_size = 0; mem_event_buffer_size = 0;
				free(mem_psensor_recs);
				printf("OUT RECORD FROM MEM EVENT WINDOW === \n");
				printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
				printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
				return 1;
			}
		}
	} else if(strstr(input->attr_name, "load") != 0) { 
		if(load_marker_event) {
			load_event_buffer_size = 10;
			printf("REC_EVENT_WINDOW function LOAD_EVENT_BUFFER_SIZE %d.... \n", load_event_buffer_size);
			load_dummy_event_buffer_size = 1;
			load_psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
			load_psensor_recs->sensor_recs_num = load_event_buffer_size;
			load_psensor_recs->sensor_recs = (struct sensor_rec *) calloc (load_event_buffer_size, sizeof(struct sensor_rec));
			load_psensor_recs->sensor_recs[0].timestamp = input->timestamp;
			if(input->attr_name != NULL)
				load_psensor_recs->sensor_recs[0].attr_name = strdup(input->attr_name);
			load_psensor_recs->sensor_recs[0].rec_val = input->rec_val;
			load_marker_event = 0;
			return 0;
		} else {
			printf("REC_EVENT_WINDOW function LOAD_EVENT_BUFFER_SIZE %d.... \n", load_event_buffer_size);
			printf("REC_EVENT_WINDOW function LOAD_DUMMY_EVENT_BUFFER_SIZE %d.... \n", load_dummy_event_buffer_size);
			if(load_dummy_event_buffer_size < load_event_buffer_size) {
				printf("REC_EVENT_WINDOW function LOAD ELAPSED WINDOW SIZE : %d \n",(load_event_buffer_size - load_dummy_event_buffer_size));
				load_psensor_recs->sensor_recs[load_dummy_event_buffer_size].timestamp = input->timestamp;
				if(input->attr_name != NULL)
					load_psensor_recs->sensor_recs[load_dummy_event_buffer_size].attr_name = strdup(input->attr_name);
				load_psensor_recs->sensor_recs[load_dummy_event_buffer_size].rec_val = input->rec_val;
				load_dummy_event_buffer_size = load_dummy_event_buffer_size + 1;
				return 0;
			} else {
				printf("REC_EVENT_WINDOW function LOAD EVENT WINDOW Crossed ....\n");
				load_marker_event = 1;  
				//output = load_psensor_recs;
				output->sensor_recs_num = load_event_buffer_size; 
				output->sensor_recs = (struct sensor_rec *) calloc (output->sensor_recs_num, sizeof(struct sensor_rec));

				int i = 0;
				for(i; i < load_event_buffer_size; i++) {
					output->sensor_recs[i].timestamp = load_psensor_recs->sensor_recs[i].timestamp;
					if(load_psensor_recs->sensor_recs[i].attr_name != NULL)
						output->sensor_recs[i].attr_name = strdup(load_psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val = load_psensor_recs->sensor_recs[i].rec_val;
				}

				load_dummy_event_buffer_size = 0; load_event_buffer_size = 0;
				free(load_psensor_recs);
				printf("OUT RECORD FROM LOAD EVENT WINDOW === \n");
				printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
				printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
				return 1;
			}
		}
	} else if(strstr(input->attr_name, "apache") != 0) { 
		if(apache_marker_event) {
			apache_event_buffer_size = 10;
			//printf("REC_EVENT_WINDOW function APACHE_EVENT_BUFFER_SIZE %d.... \n", apache_event_buffer_size);
			apache_dummy_event_buffer_size = 1;
			apache_psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
			apache_psensor_recs->sensor_recs_num = apache_event_buffer_size;
			apache_psensor_recs->sensor_recs = (struct sensor_rec *) calloc (apache_event_buffer_size, sizeof(struct sensor_rec));
			apache_psensor_recs->sensor_recs[0].timestamp = input->timestamp;
			if(input->attr_name != NULL)
				apache_psensor_recs->sensor_recs[0].attr_name = strdup(input->attr_name);
			apache_psensor_recs->sensor_recs[0].rec_val_int = input->rec_val_int;
			apache_psensor_recs->sensor_recs[0].rec_val_float = input->rec_val_float;
			apache_psensor_recs->sensor_recs[0].rec_val = input->rec_val;
			apache_marker_event = 0;
			return 0;
		} else {
			//printf("REC_EVENT_WINDOW function APACHE_EVENT_BUFFER_SIZE %d.... \n", apache_event_buffer_size);
			//printf("REC_EVENT_WINDOW function APACHE_DUMMY_EVENT_BUFFER_SIZE %d.... \n", apache_dummy_event_buffer_size);
			if(apache_dummy_event_buffer_size < apache_event_buffer_size) {
				//printf("REC_EVENT_WINDOW function APACHE ELAPSED WINDOW SIZE : %d \n",(apache_event_buffer_size - apache_dummy_event_buffer_size));
				apache_psensor_recs->sensor_recs[apache_dummy_event_buffer_size].timestamp = input->timestamp;
				if(input->attr_name != NULL)
					apache_psensor_recs->sensor_recs[apache_dummy_event_buffer_size].attr_name = strdup(input->attr_name);
				apache_psensor_recs->sensor_recs[apache_dummy_event_buffer_size].rec_val_int = input->rec_val_int;
				apache_psensor_recs->sensor_recs[apache_dummy_event_buffer_size].rec_val_float = input->rec_val_float;
				apache_psensor_recs->sensor_recs[apache_dummy_event_buffer_size].rec_val = input->rec_val;
				apache_dummy_event_buffer_size = apache_dummy_event_buffer_size + 1;
				return 0;
			} else {
				printf("REC_EVENT_WINDOW function APACHE BUSY EVENT WINDOW Crossed ....\n");
				apache_marker_event = 1;  
				//output = apache_psensor_recs;
				output->sensor_recs_num = apache_event_buffer_size; 
				output->sensor_recs = (struct sensor_rec *) calloc (output->sensor_recs_num, sizeof(struct sensor_rec));

				int i = 0;
				for(i; i < apache_event_buffer_size; i++) {
					output->sensor_recs[i].timestamp = apache_psensor_recs->sensor_recs[i].timestamp;
					if(apache_psensor_recs->sensor_recs[i].attr_name != NULL)
						output->sensor_recs[i].attr_name = strdup(apache_psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val_int = apache_psensor_recs->sensor_recs[i].rec_val_int;
					output->sensor_recs[i].rec_val_float = apache_psensor_recs->sensor_recs[i].rec_val_float;
					output->sensor_recs[i].rec_val = apache_psensor_recs->sensor_recs[i].rec_val;
				}

				apache_dummy_event_buffer_size = 0; apache_event_buffer_size = 0;
				free(apache_psensor_recs);
				printf("OUT RECORD FROM APACHE BUSY EVENT WINDOW === \n");
				printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
				printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
				printf("OUT RECORD ==> rec_val_int : %d ...\n", output->sensor_recs[0].rec_val_int);
				printf("OUT RECORD ==> rec_val_float : %f ...\n", output->sensor_recs[0].rec_val_float);
				printf("OUT RECORD ==> rec_val : %lu ...\n", output->sensor_recs[0].rec_val);
				return 1;
			}
		}
	} else if(strstr(input->attr_name, "rubis_") != 0) { 
		if(rubis_marker_event) {
			rubis_event_buffer_size = 10;
			//printf("REC_EVENT_WINDOW function FAULT_EVENT_BUFFER_SIZE %d.... \n", rubis_event_buffer_size);
			rubis_dummy_event_buffer_size = 1;
			rubis_psensor_recs = (struct sensor_recs *) calloc(1, sizeof(struct sensor_recs));  
			rubis_psensor_recs->sensor_recs_num = rubis_event_buffer_size;
			rubis_psensor_recs->sensor_recs = (struct sensor_rec *) calloc (rubis_event_buffer_size, sizeof(struct sensor_rec));
			rubis_psensor_recs->sensor_recs[0].timestamp = input->timestamp;
			if(input->attr_name != NULL)
				rubis_psensor_recs->sensor_recs[0].attr_name = strdup(input->attr_name);
			rubis_psensor_recs->sensor_recs[0].rec_val_int = input->rec_val_int;
			rubis_psensor_recs->sensor_recs[0].rec_val = input->rec_val;
			rubis_marker_event = 0;
			return 0;
		} else {
			//printf("REC_EVENT_WINDOW function FAULT_EVENT_BUFFER_SIZE %d.... \n", rubis_event_buffer_size);
			//printf("REC_EVENT_WINDOW function FAULT_DUMMY_EVENT_BUFFER_SIZE %d.... \n", rubis_dummy_event_buffer_size);
			if(rubis_dummy_event_buffer_size < rubis_event_buffer_size) {
				//printf("REC_EVENT_WINDOW function FAULT ELAPSED WINDOW SIZE : %d \n",(rubis_event_buffer_size - rubis_dummy_event_buffer_size));
				rubis_psensor_recs->sensor_recs[rubis_dummy_event_buffer_size].timestamp = input->timestamp;
				if(input->attr_name != NULL)
					rubis_psensor_recs->sensor_recs[rubis_dummy_event_buffer_size].attr_name = strdup(input->attr_name);
				rubis_psensor_recs->sensor_recs[rubis_dummy_event_buffer_size].rec_val_int = input->rec_val_int;
				rubis_psensor_recs->sensor_recs[rubis_dummy_event_buffer_size].rec_val = input->rec_val;
				rubis_dummy_event_buffer_size = rubis_dummy_event_buffer_size + 1;
				return 0;
			} else {
				printf("REC_EVENT_WINDOW function FAULT EVENT WINDOW Crossed ....\n");
				rubis_marker_event = 1;  
				//output = rubis_psensor_recs;
				output->sensor_recs_num = rubis_event_buffer_size; 
				output->sensor_recs = (struct sensor_rec *) calloc (output->sensor_recs_num, sizeof(struct sensor_rec));

				int i = 0;
				for(i; i < rubis_event_buffer_size; i++) {
					output->sensor_recs[i].timestamp = rubis_psensor_recs->sensor_recs[i].timestamp;
					if(rubis_psensor_recs->sensor_recs[i].attr_name != NULL)
						output->sensor_recs[i].attr_name = strdup(rubis_psensor_recs->sensor_recs[i].attr_name);
					output->sensor_recs[i].rec_val_int = rubis_psensor_recs->sensor_recs[i].rec_val_int;
					output->sensor_recs[i].rec_val = rubis_psensor_recs->sensor_recs[i].rec_val;
				}

				rubis_dummy_event_buffer_size = 0; rubis_event_buffer_size = 0;
				free(rubis_psensor_recs);
				//printf("OUT RECORD FROM FAULT EVENT WINDOW === \n");
				//printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
				//printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
				//printf("OUT RECORD ==> rec_val_int: %d ...\n", output->sensor_recs[0].rec_val_int);
				//printf("OUT RECORD ==> rec_val : %lu ...\n", output->sensor_recs[0].rec_val);
				return 1;
			}
		}
	}
}

static int cmpeventoccurorder (const void *p1, const void *p2) {
	return (((struct sensor_rec *)p1)->timestamp - ((struct sensor_rec *)p2)->timestamp);
}

int rec_event_occurence_order(sensor_recs_ptr input, sensor_recs_ptr output) {
	printf("Inside the libfunctions.la OCCURENCE_ORDER function .... \n");

	output->sensor_recs_num = input->sensor_recs_num;
	output->sensor_recs = (struct sensor_rec *) calloc (input->sensor_recs_num, sizeof(struct sensor_rec));

	qsort(input->sensor_recs, input->sensor_recs_num, sizeof(struct sensor_rec), cmpeventoccurorder);

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		output->sensor_recs[i].timestamp = input->sensor_recs[i].timestamp;
		if(input->sensor_recs[i].attr_name != NULL)
			output->sensor_recs[i].attr_name = strdup(input->sensor_recs[i].attr_name);
		output->sensor_recs[i].rec_val = input->sensor_recs[i].rec_val;
	}

	//printf("OUT RECORD FROM EVENT_OCCURENCE_ORDER FUNCTION === \n");
	//printf("OUT RECORD ==> timestamp : %f ...\n", output->sensor_recs[0].timestamp);
	//printf("OUT RECORD ==> attr_name : %s ...\n", output->sensor_recs[0].attr_name);
	//printf("OUT RECORD ==> rec_val : %f ...\n", output->sensor_recs[0].rec_val);
	return 1;
}

/*
int rec_event_counting_sample(sensor_recs_ptr input, count_sample_recs_ptr output) {
	printf("Inside the libfunctions.la COUNT_SAMPLE function .... \n");
	if(input == NULL) return 0;	
	pcount_sample_recs = (struct count_sample_recs *) calloc(1, sizeof(struct count_sample_recs));  
	pcount_sample_recs->csample_recs_num = 4;
	pcount_sample_recs->count_sample_recs = (struct count_sample_rec *) calloc (4, sizeof(struct count_sample_rec));

	output->csample_recs_num = 4; // Changed value from 3 to 4
	output->count_sample_recs = (struct count_sample_rec *) calloc (4, sizeof(struct count_sample_rec));

	//pcount_sample_recs = output;

	char *metric_type = NULL; int sample_count = 0; 
	int cpu_sample_count = 0; int mem_sample_count = 0; int freq_sample_count = 0;
	int load_sample_count = 0; int apache_sample_count = 0;

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			if(strstr(input->sensor_recs[i].attr_name, "cpu")) {
				if(60 < input->sensor_recs[i].rec_val < 90) {
					cpu_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "mem")) {
				if(1000 < input->sensor_recs[i].rec_val < 2000) {
					mem_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "load")) {
				if(0.3 < input->sensor_recs[i].rec_val < 0.5) {
					load_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "freq")) {
				if(2000 < input->sensor_recs[i].rec_val) {
					freq_sample_count =+ 1;
				} 
			}
		}
	}

	int j;
	for(j = 0; j < 4; j++) {
		if(j == 0) {
			metric_type = "cpu";
			sample_count = cpu_sample_count;
		} else if(j == 1) {
			metric_type = "mem";
			sample_count = mem_sample_count;
		} else if(j == 2) {
			metric_type = "load";
			sample_count = load_sample_count;
		} else if(j == 3) {
			metric_type = "freq";
			sample_count = freq_sample_count;
		}
		if(metric_type != NULL) {
			output->count_sample_recs[j].metric_type = strdup(metric_type);
			output->count_sample_recs[j].sample_count = sample_count;
		}
	}

	printf("OUT RECORD FROM COUNT SAMPLE FUNCTION === \n");
	printf("OUT RECORD ==> metric_type : %s ...\n", output->count_sample_recs[0].metric_type);
	printf("OUT RECORD ==> sample_count : %d ...\n", output->count_sample_recs[0].sample_count);
	return 1;
}
*/
unsigned long apache_4_req_rate_val = 0; unsigned long apache_5_req_rate_val = 0; unsigned long apache_6_req_rate_val = 0;
unsigned long apache_4_bytes_rate_val = 0; unsigned long apache_5_bytes_rate_val = 0; unsigned long apache_6_bytes_rate_val = 0;

int apache_4_busy_workers_val = 0; int apache_5_busy_workers_val = 0; int apache_6_busy_workers_val = 0;
int apache_4_num_vcpus_val = 0; int apache_5_num_vcpus_val = 0; int apache_6_num_vcpus_val = 0;
int apache_lb_ch4_bytes_recv_val = 0; int apache_lb_ch5_bytes_recv_val = 0; int apache_lb_ch6_bytes_recv_val = 0;

int apache_req_rate_sample_count = 0; int apache_bytes_rate_sample_count = 0;
int apache_busy_workers_sample_count = 0; int apache_num_vcpus_sample_count = 0;
int apache_lb_ch_sample_count = 0;

int rec_event_apache_counting_sample(sensor_recs_ptr input, count_sample_recs_ptr output) {
	printf("Inside the libfunctions.la APACHE BUSY COUNT_SAMPLE function .... \n");
	if(input == NULL) return 0;	

	output->csample_recs_num = 5; // Changed value from 3 and 4
	output->count_sample_recs = (struct count_sample_rec *) calloc (5, sizeof(struct count_sample_rec));

	char metric_type[64]; int sample_count = 0; 
	int apache_busy_sample_count = 0; int apache_idle_sample_count = 0; 
	int apache_req_sample_count = 0; int apache_bytes_sample_count = 0;
	double req_sample = 0; double bytes_sample = 0; int idle_sample = 0; int busy_sample = 0;

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			printf("Sensor Record Name %s \n", input->sensor_recs[i].attr_name);
			printf("Sensor Record Val %d \n", input->sensor_recs[i].rec_val_int);
			printf("Sensor Record Val %f \n", input->sensor_recs[i].rec_val_float);
			printf("Sensor Record Val %lu \n", input->sensor_recs[i].rec_val);
			if(strstr(input->sensor_recs[i].attr_name, "apache_samoa4_req_rate") != NULL) {
				//if(!apache_4_req_rate_val) {
					apache_4_req_rate_val = input->sensor_recs[i].rec_val;
				//} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa5_req_rate") != NULL) {
				//if(!apache_5_req_rate_val) {
					apache_5_req_rate_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_5_req_rate_val - apache_4_req_rate_val) > 5) { //  Original 32 change it to 5
					apache_req_rate_sample_count +=1;
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa6_req_rate") != NULL) {
				//if(!apache_6_req_rate_val) {
					apache_6_req_rate_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_6_req_rate_val - apache_4_req_rate_val) > 5) { // Original 32 change it to 5
					apache_req_rate_sample_count +=1;
				}
			}
			if(strstr(input->sensor_recs[i].attr_name, "apache_samoa4_bytes_rate") != NULL) {
				//if(!apache_4_bytes_rate_val) {
					apache_4_bytes_rate_val = input->sensor_recs[i].rec_val;
				//} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa5_bytes_rate") != NULL) {
				//if(!apache_5_bytes_rate_val) {
					apache_5_bytes_rate_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_5_req_rate_val - apache_4_req_rate_val) > 10) { // Original 4500 change it to 10
					apache_bytes_rate_sample_count +=1;
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa6_req_rate") != NULL) {
				//if(!apache_6_req_rate_val) {
					apache_6_req_rate_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_6_req_rate_val - apache_4_req_rate_val) > 4500) { // Original 4500 change it to 10
					apache_bytes_rate_sample_count +=1;
				}
			}
			if(strstr(input->sensor_recs[i].attr_name, "apache_samoa4_busy_workers") != NULL) {
				//if(!apache_4_busy_workers_val) {
					apache_4_busy_workers_val = input->sensor_recs[i].rec_val;
				//} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa5_busy_workers") != NULL) {
				//if(!apache_5_busy_workers_val) {
					apache_5_busy_workers_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_5_busy_workers_val - apache_4_busy_workers_val) > 3) { // Original 5 change it to 3
					apache_busy_workers_sample_count +=1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa6_busy_workers") != NULL) {
				//if(!apache_6_busy_workers_val) {
					apache_6_busy_workers_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_6_busy_workers_val - apache_4_busy_workers_val) > 3) { // Original 5 change it to 3
					apache_busy_workers_sample_count +=1;
				}
			}
			if(strstr(input->sensor_recs[i].attr_name, "apache_samoa4_num_vcpus") != NULL) {
				//if(!apache_4_num_vcpus_val) {
					apache_4_num_vcpus_val = input->sensor_recs[i].rec_val;
				//} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa5_num_vcpus") != NULL) {
				//if(!apache_5_num_vcpus_val) {
					apache_5_num_vcpus_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_5_num_vcpus_val - apache_4_num_vcpus_val) > 1) {
					apache_num_vcpus_sample_count +=1;
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_samoa6_num_vcpus") != NULL) {
				//if(!apache_6_num_vcpus_val) {
					apache_6_num_vcpus_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_6_num_vcpus_val - apache_4_num_vcpus_val) > 1) {
					apache_num_vcpus_sample_count +=1;
				}
			}
			if(strstr(input->sensor_recs[i].attr_name, "apache_lb_ch4_bytes_recv") != NULL) {
				//if(!apache_lb_ch4_bytes_recv_val) {
					apache_lb_ch4_bytes_recv_val = input->sensor_recs[i].rec_val;
				//} 
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_lb_ch5_bytes_recv") != NULL) {
				//if(!apache_lb_ch5_bytes_recv_val) {
					apache_lb_ch5_bytes_recv_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_lb_ch5_bytes_recv_val - apache_lb_ch5_bytes_recv_val) > 1000) { // Original 10485760 change it to 1000
					apache_lb_ch_sample_count +=1;
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "apache_lb_ch6_bytes_recv") != NULL) {
				//if(!apache_lb_ch6_bytes_recv_val) {
					apache_lb_ch6_bytes_recv_val = input->sensor_recs[i].rec_val;
				//} 
				if((apache_lb_ch6_bytes_recv_val - apache_lb_ch5_bytes_recv_val) > 1000) { // Original 10485760 change it to 1000
					apache_lb_ch_sample_count +=1;
				}
			}
		}
	}

	int j;
	for(j = 0; j < 5; j++) {
		if(j == 0) {
			strcpy(metric_type, "req_rate");
			sample_count = apache_req_rate_sample_count;
		} else if(j == 1) {
			strcpy(metric_type, "bytes_rate");
			sample_count = apache_bytes_rate_sample_count;
		} else if(j == 2) {
			strcpy(metric_type, "busy_workers");
			sample_count = apache_busy_workers_sample_count;
		} else if(j == 3) {
			strcpy(metric_type, "num_vcpus");
			sample_count = apache_num_vcpus_sample_count;
		} else if(j == 4) {
			strcpy(metric_type, "bytes_recv");
			sample_count = apache_lb_ch_sample_count;
		}
		if(metric_type != NULL) {
			output->count_sample_recs[j].metric_type = strdup(metric_type);
			output->count_sample_recs[j].sample_count = sample_count;
		}
	}
	req_sample = 0; bytes_sample = 0; idle_sample = 0; busy_sample = 0;

	printf("OUT RECORD FROM APACHE BUSY COUNT SAMPLE FUNCTION === \n");
	printf("OUT RECORD 1 ==> metric_type : %s ...\n", output->count_sample_recs[0].metric_type);
	printf("OUT RECORD 1 ==> sample_count : %d ...\n", output->count_sample_recs[0].sample_count);
	printf("OUT RECORD 2 ==> metric_type : %s ...\n", output->count_sample_recs[1].metric_type);
	printf("OUT RECORD 2 ==> sample_count : %d ...\n", output->count_sample_recs[1].sample_count);
	printf("OUT RECORD 3 ==> metric_type : %s ...\n", output->count_sample_recs[2].metric_type);
	printf("OUT RECORD 3 ==> sample_count : %d ...\n", output->count_sample_recs[2].sample_count);
	printf("OUT RECORD 4 ==> metric_type : %s ...\n", output->count_sample_recs[3].metric_type);
	printf("OUT RECORD 4 ==> sample_count : %d ...\n", output->count_sample_recs[3].sample_count);
	printf("OUT RECORD 5 ==> metric_type : %s ...\n", output->count_sample_recs[4].metric_type);
	printf("OUT RECORD 5 ==> sample_count : %d ...\n", output->count_sample_recs[4].sample_count);
	
	return 1;
}

/*
int rec_event_fault_counting_sample(sensor_recs_ptr input, count_sample_recs_ptr output) {
	printf("Inside the libfunctions.la FAULT COUNT_SAMPLE function .... \n");
	if(input == NULL) return 0;	

	output->csample_recs_num = 3; // Changed value from 3 to 4
	output->count_sample_recs = (struct count_sample_rec *) calloc (3, sizeof(struct count_sample_rec));

	char metric_type[64]; int sample_count = 0; 
	int dom0_tx_sample_count = 0; double dom0_tx_sample = 0; 
	int apache_tx_sample_count = 0; int tomcat_tx_sample_count = 0; int mysql_tx_sample_count = 0;

	unsigned long apache_tx_sample = 0L; unsigned long tomcat_tx_sample = 0L; unsigned long mysql_tx_sample = 0L;
	unsigned long apache_tx_last_sample = 0L; unsigned long tomcat_tx_last_sample = 0L; unsigned long mysql_tx_last_sample = 0L;

	unsigned long apache_1_tx_sample = 0L; unsigned long tomcat_1_tx_sample = 0L; unsigned long mysql_1_tx_sample = 0L;
	unsigned long apache_1_tx_last_sample = 0L; unsigned long tomcat_1_tx_last_sample = 0L; unsigned long mysql_1_tx_last_sample = 0L;

	unsigned long apache_2_tx_sample = 0L; unsigned long tomcat_2_tx_sample = 0L; unsigned long mysql_2_tx_sample = 0L;
	unsigned long apache_2_tx_last_sample = 0L; unsigned long tomcat_2_tx_last_sample = 0L; unsigned long mysql_2_tx_last_sample = 0L;

	unsigned long apache_3_tx_sample = 0L; unsigned long tomcat_3_tx_sample = 0L; unsigned long mysql_3_tx_sample = 0L;
	unsigned long apache_3_tx_last_sample = 0L; unsigned long tomcat_3_tx_last_sample = 0L; unsigned long mysql_3_tx_last_sample = 0L;

	unsigned long apache_4_tx_sample = 0L; unsigned long tomcat_4_tx_sample = 0L; unsigned long mysql_4_tx_sample = 0L;
	unsigned long apache_4_tx_last_sample = 0L; unsigned long tomcat_4_tx_last_sample = 0L; unsigned long mysql_4_tx_last_sample = 0L;

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			printf("Sensor Record Name %s \n", input->sensor_recs[i].attr_name);
			printf("Sensor Record Val %lu \n", input->sensor_recs[i].rec_val);
			if(strstr(input->sensor_recs[i].attr_name, "rubis_web_iftxpackets") != NULL) {
				if(!apache_tx_sample_bit) {
					apache_tx_sample = input->sensor_recs[i].rec_val;
					apache_tx_last_sample = apache_tx_sample;
					apache_tx_sample_bit = 1;
				} else {
					apache_tx_sample = input->sensor_recs[i].rec_val;
					if((apache_tx_sample - apache_tx_last_sample) == 0) {
						apache_tx_sample_count += 1;
						//printf("Apache TX Sample Count %d \n", apache_tx_sample_count);
						apache_tx_last_sample = apache_tx_sample;
					}
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_serv_iftxpackets") != NULL) {
				if(!tomcat_tx_sample_bit) {
					tomcat_tx_last_sample = input->sensor_recs[i].rec_val;
					tomcat_tx_last_sample = tomcat_tx_sample;
					tomcat_tx_sample_bit = 1;
				} else {
					tomcat_tx_sample = input->sensor_recs[i].rec_val;
					if((tomcat_tx_sample - tomcat_tx_last_sample) == 0) {
						tomcat_tx_sample_count += 1;
						//printf("Tomcat TX Sample Count %d \n", tomcat_tx_sample_count);
						tomcat_tx_last_sample = tomcat_tx_sample;
					} 
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_db_iftxpackets") !=NULL) {
				//mysql_tx_sample = input->sensor_recs[i].rec_val;
				if(!mysql_tx_sample_bit) {
					mysql_tx_sample = input->sensor_recs[i].rec_val;
					mysql_tx_last_sample = mysql_tx_sample;
					mysql_tx_sample_bit = 1;
				} else {
					mysql_tx_sample = input->sensor_recs[i].rec_val;
					if((mysql_tx_sample - mysql_tx_last_sample) == 0) {
						mysql_tx_sample_count += 1;
						//printf("MySQL TX Sample Count %d \n", mysql_tx_sample_count);
						mysql_tx_last_sample = mysql_tx_sample;
					}
				} 
			}
		}
	}

	int j;
	for(j = 0; j < 3; j++) {
		if(j == 0) {
			strcpy(metric_type, "rubis_web_iftxpackets");
			printf("Apache TX Sample Count %d \n", apache_tx_sample_count);
			sample_count = apache_tx_sample_count;
		} else if(j == 1) {
			strcpy(metric_type, "rubis_serv_iftxpackets");
			printf("Tomcat TX Sample Count %d \n", tomcat_tx_sample_count);
			sample_count = tomcat_tx_sample_count;
		} else if(j == 2) {
			strcpy(metric_type, "rubis_db_iftxpackets");
			printf("MySQL TX Sample Count %d \n", mysql_tx_sample_count);
			sample_count = mysql_tx_sample_count;
		}
		if(metric_type != NULL) {
			output->count_sample_recs[j].metric_type = strdup(metric_type);
			output->count_sample_recs[j].sample_count = sample_count;
		}
	}
	dom0_tx_sample = 0; apache_tx_sample = 0; tomcat_tx_sample = 0; mysql_tx_sample = 0;
	apache_tx_last_sample = 0; tomcat_tx_last_sample = 0; mysql_tx_last_sample = 0;
	apache_tx_sample_bit = 0; tomcat_tx_sample_bit = 0; mysql_tx_sample_bit = 0;

	printf("OUT RECORD FROM COUNT SAMPLE FUNCTION === \n");
	printf("OUT RECORD 1 ==> metric_type : %s ...\n", output->count_sample_recs[0].metric_type);
	printf("OUT RECORD 1 ==> sample_count : %d ...\n", output->count_sample_recs[0].sample_count);
	printf("OUT RECORD 2 ==> metric_type : %s ...\n", output->count_sample_recs[1].metric_type);
	printf("OUT RECORD 2 ==> sample_count : %d ...\n", output->count_sample_recs[1].sample_count);
	printf("OUT RECORD 3 ==> metric_type : %s ...\n", output->count_sample_recs[2].metric_type);
	printf("OUT RECORD 3 ==> sample_count : %d ...\n", output->count_sample_recs[2].sample_count);

    //struct timeval now;
    //gettimeofday(&now, NULL);
    //timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	//start = time(NULL);
	
	return 1;
}*/

/*
unsigned long denom = 10000000000UL; //Sampling Freq *Num of Cores * Nanosec conversion == 5*2*(10*10*10*10*10*10*10*10*10);
unsigned long last_web_ctotaltime = 0UL; unsigned long last_serv_ctotaltime = 0UL; unsigned long last_db_ctotaltime = 0UL; 

unsigned long web_3_ctime_last_sample = 0UL; unsigned long web_4_ctime_last_sample = 0UL; unsigned long web_5_ctime_last_sample = 0UL;
unsigned long web_6_ctime_last_sample = 0UL;
unsigned long serv_3_ctime_last_sample = 0UL; unsigned long serv_4_ctime_last_sample = 0UL; unsigned long serv_5_ctime_last_sample = 0UL;
unsigned long serv_6_ctime_last_sample = 0UL;
unsigned long db_3_ctime_last_sample = 0UL; unsigned long db_4_ctime_last_sample = 0UL; unsigned long db_5_ctime_last_sample = 0UL;
unsigned long db_6_ctime_last_sample = 0UL;

unsigned long apache_3_tx_last_sample = 0UL; unsigned long tomcat_3_tx_last_sample = 0UL; unsigned long mysql_3_tx_last_sample = 0UL;
unsigned long apache_4_tx_last_sample = 0UL; unsigned long tomcat_4_tx_last_sample = 0UL; unsigned long mysql_4_tx_last_sample = 0UL;
unsigned long apache_5_tx_last_sample = 0UL; unsigned long tomcat_5_tx_last_sample = 0UL; unsigned long mysql_5_tx_last_sample = 0UL;
unsigned long apache_6_tx_last_sample = 0UL; unsigned long tomcat_6_tx_last_sample = 0UL; unsigned long mysql_6_tx_last_sample = 0UL;

int ch1_bytes_recv_last_sample = 0;  int ch3_bytes_recv_last_sample = 0; int ch4_bytes_recv_last_sample = 0; int ch5_bytes_recv_last_sample = 0; 
int ch6_bytes_recv_last_sample = 0; 

char bytestring[512];
FILE *byteslog;

int rec_event_fault_counting_sample(sensor_recs_ptr input, count_sample_recs_ptr output) {
	printf("Inside the libfunctions.la FAULT COUNT_SAMPLE function .... \n");
	if(input == NULL) return 0;	

	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	byteslog = fopen("bandwidthmeasure.log", "a");
	if(byteslog == NULL){
		printf("Cannot open file.\n");
		return 1;
	}
	sprintf(bytestring, "Time %lf Bytes %d \n", timestamp, (int)(sizeof(input->sensor_recs_num) + 
	(input->sensor_recs_num * sizeof(input->sensor_recs[0]))));
	fwrite(bytestring, sizeof(char), strlen(bytestring), byteslog);
	fclose(byteslog);
	//printf("Timestamp : %lf Bytes Received %d ", timestamp, sizeof(sensor_recs_ptr));
	printf("Timestamp : %lf Bytes Received : %d \n", timestamp, (int)(sizeof(input->sensor_recs_num) + 
	(input->sensor_recs_num * sizeof(input->sensor_recs[0]))));

	output->csample_recs_num = 29; // Changed value from 3 to 4 and 21 to 29
	output->count_sample_recs = (struct count_sample_rec *) calloc (29, sizeof(struct count_sample_rec));

	char metric_type[128]; int sample_count = 0; 

	int apache_3_tx_sample_count = 0; int tomcat_3_tx_sample_count = 0; int mysql_3_tx_sample_count = 0;
	int apache_4_tx_sample_count = 0; int tomcat_4_tx_sample_count = 0; int mysql_4_tx_sample_count = 0;
	int apache_5_tx_sample_count = 0; int tomcat_5_tx_sample_count = 0; int mysql_5_tx_sample_count = 0;
	int apache_6_tx_sample_count = 0; int tomcat_6_tx_sample_count = 0; int mysql_6_tx_sample_count = 0;

	int dom0_3_ctime_sample_count = 0; int dom0_4_ctime_sample_count = 0; int dom0_5_ctime_sample_count = 0; int dom0_6_ctime_sample_count = 0;
	int web_3_ctime_sample_count = 0; int web_4_ctime_sample_count = 0; int web_5_ctime_sample_count = 0; int web_6_ctime_sample_count = 0;
	int serv_3_ctime_sample_count = 0; int serv_4_ctime_sample_count = 0; int serv_5_ctime_sample_count = 0; int serv_6_ctime_sample_count = 0;
	int db_3_ctime_sample_count = 0; int db_4_ctime_sample_count = 0; int db_5_ctime_sample_count = 0; int db_6_ctime_sample_count = 0;

	int ch1_bytes_recv_sample_count = 0; int ch3_bytes_recv_sample_count = 0; int ch4_bytes_recv_sample_count = 0; int ch5_bytes_recv_sample_count = 0;
	int ch6_bytes_recv_sample_count = 0; 

	//unsigned long apache_tx_sample = 0L; unsigned long tomcat_tx_sample = 0L; unsigned long mysql_tx_sample = 0L;
	unsigned long apache_3_tx_sample = 0UL; unsigned long tomcat_3_tx_sample = 0UL; unsigned long mysql_3_tx_sample = 0UL;
	unsigned long apache_4_tx_sample = 0UL; unsigned long tomcat_4_tx_sample = 0UL; unsigned long mysql_4_tx_sample = 0UL;
	unsigned long apache_5_tx_sample = 0UL; unsigned long tomcat_5_tx_sample = 0UL; unsigned long mysql_5_tx_sample = 0UL;
	unsigned long apache_6_tx_sample = 0UL; unsigned long tomcat_6_tx_sample = 0UL; unsigned long mysql_6_tx_sample = 0UL;
	//unsigned long apache_tx_last_sample = 0L; unsigned long tomcat_tx_last_sample = 0L; unsigned long mysql_tx_last_sample = 0L;
	//unsigned long apache_3_tx_last_sample = 0L; unsigned long tomcat_3_tx_last_sample = 0L; unsigned long mysql_3_tx_last_sample = 0L;
	//unsigned long apache_4_tx_last_sample = 0L; unsigned long tomcat_4_tx_last_sample = 0L; unsigned long mysql_4_tx_last_sample = 0L;
	//unsigned long apache_5_tx_last_sample = 0L; unsigned long tomcat_5_tx_last_sample = 0L; unsigned long mysql_5_tx_last_sample = 0L;
	//unsigned long apache_6_tx_last_sample = 0L; unsigned long tomcat_6_tx_last_sample = 0L; unsigned long mysql_6_tx_last_sample = 0L;

	//int dom0_3_ctime_sample = 0; int dom0_3_ctime_last_sample = 0; int dom0_4_ctime_sample = 0; int dom0_4_ctime_last_sample = 0;
	//int dom0_5_ctime_sample = 0; int dom0_5_ctime_last_sample = 0; 	int dom0_6_ctime_sample = 0; int dom0_6_ctime_last_sample = 0;

	unsigned long dom0_3_ctime_sample = 0UL; int dom0_4_ctime_sample = 0UL; int dom0_5_ctime_sample = 0UL; int dom0_6_ctime_sample = 0UL; 
	unsigned long web_3_ctime_sample = 0UL; int web_4_ctime_sample = 0UL; int web_5_ctime_sample = 0UL; int web_6_ctime_sample = 0UL; 
	unsigned long serv_3_ctime_sample = 0UL; int serv_4_ctime_sample = 0UL; int serv_5_ctime_sample = 0UL; int serv_6_ctime_sample = 0UL; 
	unsigned long db_3_ctime_sample = 0UL; int db_4_ctime_sample = 0UL; int db_5_ctime_sample = 0UL; int db_6_ctime_sample = 0UL; 

	int ch1_bytes_recv_sample = 0;	int ch3_bytes_recv_sample = 0; 	int ch4_bytes_recv_sample = 0;	int ch5_bytes_recv_sample = 0; 
	int ch6_bytes_recv_sample = 0;	
	
	float web_3_cpu_util = 0.0; float web_4_cpu_util = 0.0; float web_5_cpu_util = 0.0; float web_6_cpu_util = 0.0;
	float serv_3_cpu_util = 0.0; float serv_4_cpu_util = 0.0; float serv_5_cpu_util = 0.0; float serv_6_cpu_util = 0.0;
	float db_3_cpu_util = 0.0; float db_4_cpu_util = 0.0; float db_5_cpu_util = 0.0; float db_6_cpu_util = 0.0;

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			//printf("Sensor Record Name %s \n", input->sensor_recs[i].attr_name);
			//printf("Sensor Record Val %lu \n", input->sensor_recs[i].rec_val);
			if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_web_iftxpackets") != NULL) {
				if(!apache_3_tx_sample_bit) {
					apache_3_tx_sample = input->sensor_recs[i].rec_val;
					apache_3_tx_last_sample = apache_3_tx_sample;
					apache_3_tx_sample_bit = 1;
				} else {
					apache_3_tx_sample = input->sensor_recs[i].rec_val;
					if((apache_3_tx_sample - apache_3_tx_last_sample) < 3) { // Moved from == 0
						apache_3_tx_sample_count += 1;
						//printf("Samoa3 Apache TX Sample Counting Now %d \n", apache_3_tx_sample_count);
						apache_3_tx_last_sample = apache_3_tx_sample;
					}
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_web_iftxpackets") != NULL) {
				if(!apache_4_tx_sample_bit) {
					apache_4_tx_sample = input->sensor_recs[i].rec_val;
					apache_4_tx_last_sample = apache_4_tx_sample;
					apache_4_tx_sample_bit = 1;
				} else {
					apache_4_tx_sample = input->sensor_recs[i].rec_val;
					if((apache_4_tx_sample - apache_4_tx_last_sample) < 3) { //Moved from == 0
						apache_4_tx_sample_count += 1;
						//printf("Samoa4 Apache TX Sample Counting Now %d \n", apache_4_tx_sample_count);
						apache_4_tx_last_sample = apache_4_tx_sample;
					}
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_web_iftxpackets") != NULL) {
				if(!apache_5_tx_sample_bit) {
					apache_5_tx_sample = input->sensor_recs[i].rec_val;
					apache_5_tx_last_sample = apache_5_tx_sample;
					apache_5_tx_sample_bit = 1;
				} else {
					apache_5_tx_sample = input->sensor_recs[i].rec_val;
					if((apache_5_tx_sample - apache_5_tx_last_sample) < 3) { //Moved from == 0
						apache_5_tx_sample_count += 1;
						//printf("Samoa5 Apache TX Sample Counting Now %d \n", apache_5_tx_sample_count);
						apache_5_tx_last_sample = apache_5_tx_sample;
					}
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_web_iftxpackets") != NULL) {
				if(!apache_6_tx_sample_bit) {
					apache_6_tx_sample = input->sensor_recs[i].rec_val;
					apache_6_tx_last_sample = apache_6_tx_sample;
					apache_6_tx_sample_bit = 1;
				} else {
					apache_6_tx_sample = input->sensor_recs[i].rec_val;
					if((apache_6_tx_sample - apache_6_tx_last_sample) < 3) { //Moved from == 0
						apache_6_tx_sample_count += 1;
						//printf("Samoa6 Apache TX Sample Counting Now %d \n", apache_6_tx_sample_count);
						apache_6_tx_last_sample = apache_6_tx_sample;
					}
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_serv_iftxpackets") != NULL) {
				if(!tomcat_3_tx_sample_bit) {
					tomcat_3_tx_sample = input->sensor_recs[i].rec_val;
					tomcat_3_tx_last_sample = tomcat_3_tx_sample;
					tomcat_3_tx_sample_bit = 1;
				} else {
					tomcat_3_tx_sample = input->sensor_recs[i].rec_val;
					if((tomcat_3_tx_sample - tomcat_3_tx_last_sample) < 3) { //Moved from == 0
						tomcat_3_tx_sample_count += 1;
						//printf("Samoa3 Tomcat TX Sample Counting Now %d \n", tomcat_3_tx_sample_count);
						tomcat_3_tx_last_sample = tomcat_3_tx_sample;
					} 
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_serv_iftxpackets") != NULL) {
				if(!tomcat_4_tx_sample_bit) {
					tomcat_4_tx_sample = input->sensor_recs[i].rec_val;
					tomcat_4_tx_last_sample = tomcat_4_tx_sample;
					tomcat_4_tx_sample_bit = 1;
				} else {
					tomcat_4_tx_sample = input->sensor_recs[i].rec_val;
					if((tomcat_4_tx_sample - tomcat_4_tx_last_sample) < 3) { // Moved from == 0
						tomcat_4_tx_sample_count += 1;
						//printf("Samoa4 Tomcat TX Sample Counting Now %d \n", tomcat_4_tx_sample_count);
						tomcat_4_tx_last_sample = tomcat_4_tx_sample;
					} 
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_serv_iftxpackets") != NULL) {
				if(!tomcat_5_tx_sample_bit) {
					tomcat_5_tx_sample = input->sensor_recs[i].rec_val;
					tomcat_5_tx_last_sample = tomcat_5_tx_sample;
					tomcat_5_tx_sample_bit = 1;
				} else {
					tomcat_5_tx_sample = input->sensor_recs[i].rec_val;
					if((tomcat_5_tx_sample - tomcat_5_tx_last_sample) < 3) {
						tomcat_5_tx_sample_count += 1;
						//printf("Samoa5 Tomcat TX Sample Counting Now %d \n", tomcat_5_tx_sample_count);
						tomcat_5_tx_last_sample = tomcat_5_tx_sample;
					} 
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_serv_iftxpackets") != NULL) {
				if(!tomcat_6_tx_sample_bit) {
					tomcat_6_tx_sample = input->sensor_recs[i].rec_val;
					tomcat_6_tx_last_sample = tomcat_6_tx_sample;
					tomcat_6_tx_sample_bit = 1;
				} else {
					tomcat_6_tx_sample = input->sensor_recs[i].rec_val;
					if((tomcat_6_tx_sample - tomcat_6_tx_last_sample) < 3) {
						tomcat_6_tx_sample_count += 1;
						//printf("Samoa6 Tomcat TX Sample Counting Now %d \n", tomcat_6_tx_sample_count);
						tomcat_6_tx_last_sample = tomcat_6_tx_sample;
					} 
				}
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_db_iftxpackets") !=NULL) {
				if(!mysql_3_tx_sample_bit) {
					mysql_3_tx_sample = input->sensor_recs[i].rec_val;
					mysql_3_tx_last_sample = mysql_3_tx_sample;
					mysql_3_tx_sample_bit = 1;
				} else {
					mysql_3_tx_sample = input->sensor_recs[i].rec_val;
					if((mysql_3_tx_sample - mysql_3_tx_last_sample) < 3) {
						mysql_3_tx_sample_count += 1;
						//printf("Samoa3 MySQL TX Sample Counting Now %d \n", mysql_3_tx_sample_count);
						mysql_3_tx_last_sample = mysql_3_tx_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_db_iftxpackets") !=NULL) {
				if(!mysql_4_tx_sample_bit) {
					mysql_4_tx_sample = input->sensor_recs[i].rec_val;
					mysql_4_tx_last_sample = mysql_4_tx_sample;
					mysql_4_tx_sample_bit = 1;
				} else {
					mysql_4_tx_sample = input->sensor_recs[i].rec_val;
					if((mysql_4_tx_sample - mysql_4_tx_last_sample) < 3) {
						mysql_4_tx_sample_count += 1;
						//printf("Samoa4 MySQL TX Sample Counting Now %d \n", mysql_4_tx_sample_count);
						mysql_4_tx_last_sample = mysql_4_tx_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_db_iftxpackets") !=NULL) {
				if(!mysql_5_tx_sample_bit) {
					mysql_5_tx_sample = input->sensor_recs[i].rec_val;
					mysql_5_tx_last_sample = mysql_5_tx_sample;
					mysql_5_tx_sample_bit = 1;
				} else {
					mysql_5_tx_sample = input->sensor_recs[i].rec_val;
					if((mysql_5_tx_sample - mysql_5_tx_last_sample) < 3) {
						mysql_5_tx_sample_count += 1;
						//printf("Samoa5 MySQL TX Sample Counting Now %d \n", mysql_5_tx_sample_count);
						mysql_5_tx_last_sample = mysql_5_tx_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_db_iftxpackets") !=NULL) {
				if(!mysql_6_tx_sample_bit) {
					mysql_6_tx_sample = input->sensor_recs[i].rec_val;
					mysql_6_tx_last_sample = mysql_6_tx_sample;
					mysql_6_tx_sample_bit = 1;
				} else {
					mysql_6_tx_sample = input->sensor_recs[i].rec_val;
					if((mysql_6_tx_sample - mysql_6_tx_last_sample) < 3) {
						mysql_6_tx_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", mysql_6_tx_sample_count);
						mysql_6_tx_last_sample = mysql_6_tx_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_web_ctotaltime") !=NULL) {
				if(!web_3_ctime_sample_bit) {
					web_3_ctime_sample = input->sensor_recs[i].rec_val;
					web_3_ctime_last_sample = web_3_ctime_sample;
					web_3_ctime_sample_bit = 1;
				} else {
					web_3_ctime_sample = input->sensor_recs[i].rec_val;
					web_3_cpu_util = (web_3_ctime_sample - web_3_ctime_last_sample)*100/denom;
					if(web_3_cpu_util < 2) {
						web_3_ctime_sample_count += 1;
						printf("Samoa3 Apache VM Util %f \n", web_3_cpu_util);
						web_3_ctime_last_sample = web_3_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_web_ctotaltime") !=NULL) {
				if(!web_4_ctime_sample_bit) {
					web_4_ctime_sample = input->sensor_recs[i].rec_val;
					web_4_ctime_last_sample = web_4_ctime_sample;
					web_4_ctime_sample_bit = 1;
				} else {
					web_4_ctime_sample = input->sensor_recs[i].rec_val;
					web_4_cpu_util = (web_4_ctime_sample - web_4_ctime_last_sample)*100/denom;
					if(web_4_cpu_util < 2) {
						web_4_ctime_sample_count += 1;
						printf("Samoa4 Apache VM Util %f \n", web_4_cpu_util);
						web_4_ctime_last_sample = web_4_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_web_ctotaltime") !=NULL) {
				if(!web_5_ctime_sample_bit) {
					web_5_ctime_sample = input->sensor_recs[i].rec_val;
					web_5_ctime_last_sample = web_5_ctime_sample;
					web_5_ctime_sample_bit = 1;
				} else {
					web_5_ctime_sample = input->sensor_recs[i].rec_val;
					web_5_cpu_util = (web_5_ctime_sample - web_5_ctime_last_sample)*100/denom;
					if(web_5_cpu_util < 2) {
						web_5_ctime_sample_count += 1;
						printf("Samoa5 Apache VM Util %f \n", web_5_cpu_util);
						web_5_ctime_last_sample = web_5_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_web_ctotaltime") !=NULL) {
				if(!web_6_ctime_sample_bit) {
					web_6_ctime_sample = input->sensor_recs[i].rec_val;
					web_6_ctime_last_sample = web_6_ctime_sample;
					web_6_ctime_sample_bit = 1;
				} else {
					web_6_ctime_sample = input->sensor_recs[i].rec_val;
					web_6_cpu_util = (web_6_ctime_sample - web_6_ctime_last_sample)*100/denom;
					//if((web_6_ctime_sample - web_6_ctime_last_sample) <3) {
					if(web_6_cpu_util < 2) {
						web_6_ctime_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", web_6_ctime_sample_count);
						printf("Samoa6 Apache VM Util %f \n", web_6_cpu_util);
						web_6_ctime_last_sample = web_6_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_serv_ctotaltime") !=NULL) {
				if(!serv_3_ctime_sample_bit) {
					serv_3_ctime_sample = input->sensor_recs[i].rec_val;
					serv_3_ctime_last_sample = serv_3_ctime_sample;
					serv_3_ctime_sample_bit = 1;
				} else {
					serv_3_ctime_sample = input->sensor_recs[i].rec_val;
					serv_3_cpu_util = (serv_3_ctime_sample - serv_3_ctime_last_sample)*100/denom;
					if(serv_3_cpu_util < 2) {
						serv_3_ctime_sample_count += 1;
						printf("Samoa3 Tomcat VM Util %f \n", serv_3_cpu_util);
						serv_3_ctime_last_sample = serv_3_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_serv_ctotaltime") !=NULL) {
				if(!serv_4_ctime_sample_bit) {
					serv_4_ctime_sample = input->sensor_recs[i].rec_val;
					serv_4_ctime_last_sample = serv_4_ctime_sample;
					serv_4_ctime_sample_bit = 1;
				} else {
					serv_4_ctime_sample = input->sensor_recs[i].rec_val;
					serv_4_cpu_util = (serv_4_ctime_sample - serv_4_ctime_last_sample)*100/denom;
					if(serv_4_cpu_util < 2) {
						serv_4_ctime_sample_count += 1;
						printf("Samoa4 Tomcat VM Util %f \n", serv_4_cpu_util);
						serv_4_ctime_last_sample = serv_4_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_serv_ctotaltime") !=NULL) {
				if(!serv_5_ctime_sample_bit) {
					serv_5_ctime_sample = input->sensor_recs[i].rec_val;
					serv_5_ctime_last_sample = serv_5_ctime_sample;
					serv_5_ctime_sample_bit = 1;
				} else {
					serv_5_ctime_sample = input->sensor_recs[i].rec_val;
					serv_5_cpu_util = (serv_5_ctime_sample - serv_5_ctime_last_sample)*100/denom;
					if(serv_5_cpu_util < 2) {
						serv_5_ctime_sample_count += 1;
						printf("Samoa5 Tomcat VM Util %f \n", serv_5_cpu_util);
						serv_5_ctime_last_sample = serv_5_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_serv_ctotaltime") !=NULL) {
				if(!serv_6_ctime_sample_bit) {
					serv_6_ctime_sample = input->sensor_recs[i].rec_val;
					serv_6_ctime_last_sample = serv_6_ctime_sample;
					serv_6_ctime_sample_bit = 1;
				} else {
					serv_6_ctime_sample = input->sensor_recs[i].rec_val;
					serv_6_cpu_util = (serv_6_ctime_sample - serv_6_ctime_last_sample)*100/denom;
					//if((serv_6_ctime_sample - serv_6_ctime_last_sample) <3) {
					if(serv_6_cpu_util < 2) {
						serv_6_ctime_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", serv_6_ctime_sample_count);
						printf("Samoa6 Tomcat VM Util %f \n", serv_6_cpu_util);
						serv_6_ctime_last_sample = serv_6_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa3_db_ctotaltime") !=NULL) {
				if(!db_3_ctime_sample_bit) {
					db_3_ctime_sample = input->sensor_recs[i].rec_val;
					db_3_ctime_last_sample = db_3_ctime_sample;
					db_3_ctime_sample_bit = 1;
				} else {
					db_3_ctime_sample = input->sensor_recs[i].rec_val;
					db_3_cpu_util = (db_3_ctime_sample - db_3_ctime_last_sample)*100/denom;
					if(db_3_cpu_util < 2) {
						db_3_ctime_sample_count += 1;
						printf("Samoa3 DB VM Util %f \n", db_3_cpu_util);
						db_3_ctime_last_sample = db_3_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa4_db_ctotaltime") !=NULL) {
				if(!db_4_ctime_sample_bit) {
					db_4_ctime_sample = input->sensor_recs[i].rec_val;
					db_4_ctime_last_sample = db_4_ctime_sample;
					db_4_ctime_sample_bit = 1;
				} else {
					db_4_ctime_sample = input->sensor_recs[i].rec_val;
					db_4_cpu_util = (db_4_ctime_sample - db_4_ctime_last_sample)*100/denom;
					if(db_4_cpu_util < 2) {
						db_4_ctime_sample_count += 1;
						printf("Samoa4 DB VM Util %f \n", db_4_cpu_util);
						db_4_ctime_last_sample = db_4_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa5_db_ctotaltime") !=NULL) {
				if(!db_5_ctime_sample_bit) {
					db_5_ctime_sample = input->sensor_recs[i].rec_val;
					db_5_ctime_last_sample = db_5_ctime_sample;
					db_5_ctime_sample_bit = 1;
				} else {
					db_5_ctime_sample = input->sensor_recs[i].rec_val;
					db_5_cpu_util = (db_5_ctime_sample - db_5_ctime_last_sample)*100/denom;
					if(db_5_cpu_util < 2) {
						db_5_ctime_sample_count += 1;
						printf("Samoa5 DB VM Util %f \n", db_5_cpu_util);
						db_5_ctime_last_sample = db_5_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_samoa6_db_ctotaltime") !=NULL) {
				if(!db_6_ctime_sample_bit) {
					db_6_ctime_sample = input->sensor_recs[i].rec_val;
					db_6_ctime_last_sample = db_6_ctime_sample;
					db_6_ctime_sample_bit = 1;
				} else {
					db_6_ctime_sample = input->sensor_recs[i].rec_val;
					db_6_cpu_util = (db_6_ctime_sample - db_6_ctime_last_sample)*100/denom;
					//if((db_6_ctime_sample - db_6_ctime_last_sample) <3) {
					if(db_6_cpu_util < 2) {
						db_6_ctime_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", db_6_ctime_sample_count);
						printf("Samoa6 DB VM Util %f \n", db_6_cpu_util);
						db_6_ctime_last_sample = db_6_ctime_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_lb_ch1_bytes_recv") !=NULL) {
				if(!ch1_bytes_recv_sample_bit) {
					ch1_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					ch1_bytes_recv_last_sample = ch1_bytes_recv_sample;
					ch1_bytes_recv_sample_bit = 1;
				} else {
					ch1_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					if((ch1_bytes_recv_sample - ch1_bytes_recv_last_sample) <3) {
						ch1_bytes_recv_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", ch1_bytes_recv_sample_count);
						ch1_bytes_recv_last_sample = ch1_bytes_recv_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_lb_ch3_bytes_recv") !=NULL) {
				if(!ch3_bytes_recv_sample_bit) {
					ch3_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					ch3_bytes_recv_last_sample = ch3_bytes_recv_sample;
					ch3_bytes_recv_sample_bit = 1;
				} else {
					ch3_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					if((ch3_bytes_recv_sample - ch3_bytes_recv_last_sample) <3) {
						ch3_bytes_recv_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", ch3_bytes_recv_sample_count);
						ch3_bytes_recv_last_sample = ch3_bytes_recv_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_lb_ch4_bytes_recv") !=NULL) {
				if(!ch4_bytes_recv_sample_bit) {
					ch4_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					ch4_bytes_recv_last_sample = ch4_bytes_recv_sample;
					ch4_bytes_recv_sample_bit = 1;
				} else {
					ch4_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					if((ch4_bytes_recv_sample - ch4_bytes_recv_last_sample) <3) {
						ch4_bytes_recv_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", ch4_bytes_recv_sample_count);
						ch4_bytes_recv_last_sample = ch4_bytes_recv_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_lb_ch5_bytes_recv") !=NULL) {
				if(!ch5_bytes_recv_sample_bit) {
					ch5_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					ch5_bytes_recv_last_sample = ch5_bytes_recv_sample;
					ch5_bytes_recv_sample_bit = 1;
				} else {
					ch5_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					if((ch5_bytes_recv_sample - ch5_bytes_recv_last_sample) <3) {
						ch5_bytes_recv_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", ch5_bytes_recv_sample_count);
						ch5_bytes_recv_last_sample = ch5_bytes_recv_sample;
					}
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "rubis_lb_ch6_bytes_recv") !=NULL) {
				if(!ch6_bytes_recv_sample_bit) {
					ch6_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					ch6_bytes_recv_last_sample = ch6_bytes_recv_sample;
					ch6_bytes_recv_sample_bit = 1;
				} else {
					ch6_bytes_recv_sample = input->sensor_recs[i].rec_val_int;
					if((ch6_bytes_recv_sample - ch6_bytes_recv_last_sample) <3) {
						ch6_bytes_recv_sample_count += 1;
						//printf("Samoa6 MySQL TX Sample Counting Now %d \n", ch6_bytes_recv_sample_count);
						ch6_bytes_recv_last_sample = ch6_bytes_recv_sample;
					}
				} 
			}
		}
	}

	int j;
	for(j = 0; j < 29; j++) {
		if(j == 0) {
			strcpy(metric_type, "rubis_samoa3_web_iftxpackets");
			//printf("Samoa3 Apache TX Sample Count %d \n", apache_3_tx_sample_count);
			sample_count = apache_3_tx_sample_count;
		} else if(j == 1) {
			strcpy(metric_type, "rubis_samoa4_web_iftxpackets");
			//printf("Samoa4 Apache TX Sample Count %d \n", apache_4_tx_sample_count);
			sample_count = apache_4_tx_sample_count;
		} else if(j == 2) {
			strcpy(metric_type, "rubis_samoa5_web_iftxpackets");
			//printf("Samoa5 Apache TX Sample Count %d \n", apache_5_tx_sample_count);
			sample_count = apache_5_tx_sample_count;
		} else if(j == 3) {
			strcpy(metric_type, "rubis_samoa6_web_iftxpackets");
			//printf("Samoa6 Apache TX Sample Count %d \n", apache_6_tx_sample_count);
			sample_count = apache_6_tx_sample_count;
		} else if(j == 4) {
			strcpy(metric_type, "rubis_samoa3_serv_iftxpackets");
			//printf("Samoa3 Tomcat TX Sample Count %d \n", tomcat_3_tx_sample_count);
			sample_count = tomcat_3_tx_sample_count;
		} else if(j == 5) {
			strcpy(metric_type, "rubis_samoa4_serv_iftxpackets");
			//printf("Samoa4 Tomcat TX Sample Count %d \n", tomcat_4_tx_sample_count);
			sample_count = tomcat_4_tx_sample_count;
		} else if(j == 6) {
			strcpy(metric_type, "rubis_samoa5_serv_iftxpackets");
			//printf("Samoa5 Tomcat TX Sample Count %d \n", tomcat_5_tx_sample_count);
			sample_count = tomcat_5_tx_sample_count;
		} else if(j == 7) {
			strcpy(metric_type, "rubis_samoa6_serv_iftxpackets");
			//printf("Samoa6 Tomcat TX Sample Count %d \n", tomcat_6_tx_sample_count);
			sample_count = tomcat_6_tx_sample_count;
		} else if(j == 8) {
			strcpy(metric_type, "rubis_samoa3_db_iftxpackets");
			//printf("Samoa3 MySQL TX Sample Count %d \n", mysql_3_tx_sample_count);
			sample_count = mysql_3_tx_sample_count;
		} else if(j == 9) {
			strcpy(metric_type, "rubis_samoa4_db_iftxpackets");
			//printf("Samoa4 MySQL TX Sample Count %d \n", mysql_4_tx_sample_count);
			sample_count = mysql_4_tx_sample_count;
		} else if(j == 10) {
			strcpy(metric_type, "rubis_samoa5_db_iftxpackets");
			//printf("Samoa5 MySQL TX Sample Count %d \n", mysql_5_tx_sample_count);
			sample_count = mysql_5_tx_sample_count;
		} else if(j == 11) {
			strcpy(metric_type, "rubis_samoa6_db_iftxpackets");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = mysql_6_tx_sample_count;
		} else if(j == 12) {
			strcpy(metric_type, "rubis_samoa3_web_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = web_3_ctime_sample_count;
		} else if(j == 13) {
			strcpy(metric_type, "rubis_samoa4_web_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = web_4_ctime_sample_count;
		} else if(j == 14) {
			strcpy(metric_type, "rubis_samoa5_web_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = web_5_ctime_sample_count;
		} else if(j == 15) {
			strcpy(metric_type, "rubis_samoa6_web_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = web_6_ctime_sample_count;
		} else if(j == 16) {
			strcpy(metric_type, "rubis_samoa3_serv_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = serv_3_ctime_sample_count;
		} else if(j == 17) {
			strcpy(metric_type, "rubis_samoa4_serv_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = serv_4_ctime_sample_count;
		} else if(j == 18) {
			strcpy(metric_type, "rubis_samoa5_serv_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = serv_5_ctime_sample_count;
		} else if(j == 19) {
			strcpy(metric_type, "rubis_samoa6_serv_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = serv_6_ctime_sample_count;
		} else if(j == 20) {
			strcpy(metric_type, "rubis_samoa3_db_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = db_3_ctime_sample_count;
		} else if(j == 21) {
			strcpy(metric_type, "rubis_samoa4_db_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = db_4_ctime_sample_count;
		} else if(j == 22) {
			strcpy(metric_type, "rubis_samoa5_db_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = db_5_ctime_sample_count;
		} else if(j == 23) {
			strcpy(metric_type, "rubis_samoa6_db_ctotaltime");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = db_6_ctime_sample_count;
		} else if(j == 24) {
			strcpy(metric_type, "rubis_lb_ch1_bytes_recv");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = ch1_bytes_recv_sample_count;
		} else if(j == 25) {
			strcpy(metric_type, "rubis_lb_ch3_bytes_recv");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = ch3_bytes_recv_sample_count;
		} else if(j == 26) {
			strcpy(metric_type, "rubis_lb_ch4_bytes_recv");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = ch4_bytes_recv_sample_count;
		} else if(j == 27) {
			strcpy(metric_type, "rubis_lb_ch5_bytes_recv");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = ch5_bytes_recv_sample_count;
		} else if(j == 28) {
			strcpy(metric_type, "rubis_lb_ch6_bytes_recv");
			//printf("Samoa6 MySQL TX Sample Count %d \n", mysql_6_tx_sample_count);
			sample_count = ch6_bytes_recv_sample_count;
		}
		if(metric_type != NULL) {
			output->count_sample_recs[j].metric_type = strdup(metric_type);
			output->count_sample_recs[j].sample_count = sample_count;
		}
	}

	//dom0_tx_sample = 0; apache_tx_sample = 0; tomcat_tx_sample = 0; mysql_tx_sample = 0;
	//apache_tx_last_sample = 0; tomcat_tx_last_sample = 0; mysql_tx_last_sample = 0;
	//apache_tx_sample_bit = 0; tomcat_tx_sample_bit = 0; mysql_tx_sample_bit = 0;

	apache_3_tx_sample = 0; tomcat_3_tx_sample = 0; mysql_3_tx_sample = 0;
	apache_4_tx_sample = 0; tomcat_4_tx_sample = 0; mysql_4_tx_sample = 0;
	apache_5_tx_sample = 0; tomcat_5_tx_sample = 0; mysql_5_tx_sample = 0;
	apache_6_tx_sample = 0; tomcat_6_tx_sample = 0; mysql_6_tx_sample = 0;

	//apache_3_tx_last_sample = 0; tomcat_3_tx_last_sample = 0; mysql_3_tx_last_sample = 0;
	//apache_4_tx_last_sample = 0; tomcat_4_tx_last_sample = 0; mysql_4_tx_last_sample = 0;
	//apache_5_tx_last_sample = 0; tomcat_5_tx_last_sample = 0; mysql_5_tx_last_sample = 0;
	//apache_6_tx_last_sample = 0; tomcat_6_tx_last_sample = 0; mysql_6_tx_last_sample = 0;

	apache_3_tx_sample_bit = 0; tomcat_3_tx_sample_bit = 0; mysql_3_tx_sample_bit = 0;
	apache_4_tx_sample_bit = 0; tomcat_4_tx_sample_bit = 0; mysql_4_tx_sample_bit = 0;
	apache_5_tx_sample_bit = 0; tomcat_5_tx_sample_bit = 0; mysql_5_tx_sample_bit = 0;
	apache_6_tx_sample_bit = 0; tomcat_6_tx_sample_bit = 0; mysql_6_tx_sample_bit = 0;

	dom0_3_ctime_sample = 0; dom0_4_ctime_sample = 0; dom0_5_ctime_sample = 0; dom0_6_ctime_sample = 0;
	//dom0_3_ctime_last_sample = 0; dom0_4_ctime_last_sample = 0; dom0_5_ctime_last_sample = 0; dom0_6_ctime_last_sample = 0;

	ch1_bytes_recv_sample = 0;	ch3_bytes_recv_sample = 0; ch4_bytes_recv_sample = 0;	ch5_bytes_recv_sample = 0; 
	ch6_bytes_recv_sample = 0;

	//ch1_bytes_recv_last_sample = 0; ch3_bytes_recv_last_sample = 0;ch4_bytes_recv_last_sample = 0; ch5_bytes_recv_last_sample = 0;
	//ch6_bytes_recv_last_sample = 0;

	printf("OUT RECORD FROM COUNT SAMPLE FUNCTION === \n");
	printf("OUT RECORD 1 ==> metric_type : %s ...\n", output->count_sample_recs[0].metric_type);
	printf("OUT RECORD 1 ==> sample_count : %d ...\n", output->count_sample_recs[0].sample_count);
	printf("OUT RECORD 2 ==> metric_type : %s ...\n", output->count_sample_recs[1].metric_type);
	printf("OUT RECORD 2 ==> sample_count : %d ...\n", output->count_sample_recs[1].sample_count);
	printf("OUT RECORD 3 ==> metric_type : %s ...\n", output->count_sample_recs[2].metric_type);
	printf("OUT RECORD 3 ==> sample_count : %d ...\n", output->count_sample_recs[2].sample_count);
	printf("OUT RECORD 4 ==> metric_type : %s ...\n", output->count_sample_recs[3].metric_type);
	printf("OUT RECORD 4 ==> sample_count : %d ...\n", output->count_sample_recs[3].sample_count);
	printf("OUT RECORD 5 ==> metric_type : %s ...\n", output->count_sample_recs[4].metric_type);
	printf("OUT RECORD 5 ==> sample_count : %d ...\n", output->count_sample_recs[4].sample_count);
	printf("OUT RECORD 6 ==> metric_type : %s ...\n", output->count_sample_recs[5].metric_type);
	printf("OUT RECORD 6 ==> sample_count : %d ...\n", output->count_sample_recs[5].sample_count);
	printf("OUT RECORD 7 ==> metric_type : %s ...\n", output->count_sample_recs[6].metric_type);
	printf("OUT RECORD 7 ==> sample_count : %d ...\n", output->count_sample_recs[6].sample_count);
	printf("OUT RECORD 8 ==> metric_type : %s ...\n", output->count_sample_recs[7].metric_type);
	printf("OUT RECORD 8 ==> sample_count : %d ...\n", output->count_sample_recs[7].sample_count);
	printf("OUT RECORD 9 ==> metric_type : %s ...\n", output->count_sample_recs[8].metric_type);
	printf("OUT RECORD 9 ==> sample_count : %d ...\n", output->count_sample_recs[8].sample_count);
	printf("OUT RECORD 17 ==> metric_type : %s ...\n", output->count_sample_recs[16].metric_type);
	printf("OUT RECORD 17 ==> sample_count : %d ...\n", output->count_sample_recs[16].sample_count);
	printf("OUT RECORD 18 ==> metric_type : %s ...\n", output->count_sample_recs[17].metric_type);
	printf("OUT RECORD 18 ==> sample_count : %d ...\n", output->count_sample_recs[17].sample_count);
	printf("OUT RECORD 19 ==> metric_type : %s ...\n", output->count_sample_recs[19].metric_type);
	printf("OUT RECORD 19 ==> sample_count : %d ...\n", output->count_sample_recs[19].sample_count);
	printf("OUT RECORD 20 ==> metric_type : %s ...\n", output->count_sample_recs[20].metric_type);
	printf("OUT RECORD 20 ==> sample_count : %d ...\n", output->count_sample_recs[20].sample_count);

	return 1;
}*/

/*
int nodes_group_counting_sample(sensor_recs_ptr input, count_sample_recs_ptr output) {
	printf("Inside the libfunctions.la NODES GROUP COUNT_SAMPLE function .... \n");
	printf(" %d \n", 2);
	if(input == NULL) return 0;	
	pcount_sample_recs = (struct count_sample_recs *) calloc(1, sizeof(struct count_sample_recs));  
	pcount_sample_recs->csample_recs_num = 1;
	pcount_sample_recs->count_sample_recs = (struct count_sample_rec *) calloc (1, sizeof(struct count_sample_rec));

	pcount_sample_recs = output;

	char *metric_type = NULL; int sample_count = 0; 
	int cpu_sample_count = 0; int mem_sample_count = 0; int load_sample_count = 0; int freq_sample_count = 0;

	int i;
	for(i = 0; i < input->sensor_recs_num; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			if(strstr(input->sensor_recs[i].attr_name, "cpu")) {
				if(60 < input->sensor_recs[i].rec_val < 90) {
					cpu_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "mem")) {
				if(1000 < input->sensor_recs[i].rec_val < 2000) {
					mem_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "load")) {
				if(0.3 < input->sensor_recs[i].rec_val < 0.5) {
					load_sample_count =+ 1;
				} 
			} else if(strstr(input->sensor_recs[i].attr_name, "freq")) {
				if(2000 < input->sensor_recs[i].rec_val) {
					freq_sample_count =+ 1;
				} 
			}
		}
	}

	int j;
	for(j = 0; j < 1; j++) {
		if(j == 0) {
			metric_type = "cpu";
			sample_count = cpu_sample_count;
		} else if(j == 1) {
			metric_type = "mem";
			sample_count = mem_sample_count;
		} else if(j == 2) {
			metric_type = "load";
			sample_count = load_sample_count;
		} else if(j == 3) {
			metric_type = "freq";
			sample_count = freq_sample_count;
		}
		if(metric_type != NULL){
			output->count_sample_recs[j].metric_type = strdup(metric_type);
			output->count_sample_recs[j].sample_count = sample_count;
		}
	}

	printf("OUT RECORD FROM COUNT SAMPLE FUNCTION === \n");
	printf("OUT RECORD ==> metric_type : %s ...\n", output->count_sample_recs[0].metric_type);
	printf("OUT RECORD ==> sample_count : %d ...\n", output->count_sample_recs[0].sample_count);
	return 1;
	}

	int counting_sample_decision(count_sample_recs_ptr input) {
	printf(" %d \n", 1);
	if(input == NULL) return 0;	

	int i;
	for(i = 0; i < input->csample_recs_num; i++) {
		if(((count_sample_recs_ptr)input)->count_sample_recs != NULL && ((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type != NULL) {
			if(strstr(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "cpu") != 0) {
				if(strcmp(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "cpu_idle") == 0 && 
					(((count_sample_recs_ptr)input)->count_sample_recs[i].sample_count > 1)) {
						printf("CPU THRESHOLD TRIGGER ACTIVATED ..\n");
						return 0;
					}
			} else if(strcmp(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "mem") == 0) {
				if(strcmp(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "mem_free") == 0 && 
				(((count_sample_recs_ptr)input)->count_sample_recs[i].sample_count > 10)) {
					printf("MEMORY THRESHOLD TRIGGER ACTIVATED ..\n");
					return 1;
				}
			} else {
				return -1;
			}
		}
	}
}

int load_counting_sample_decision(count_sample_recs_ptr input) {
	printf(" %d \n", 1);
	if(input == NULL) return 0;	
	int i;
	for(i = 0; i < input->csample_recs_num; i++) {
		if(((count_sample_recs_ptr)input)->count_sample_recs != NULL && ((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type != NULL) {
			if(strstr(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "apache") != 0) {//Change to "load"
				if(strcmp(((count_sample_recs_ptr)input)->count_sample_recs[i].metric_type, "cpu_idle") == 0 && 
					(((count_sample_recs_ptr)input)->count_sample_recs[i].sample_count > 1)) {
						printf("LOAD THRESHOLD TRIGGER ACTIVATED ..\n");
						return 0;
					}
			} else {
				return -1;
			}
		}
	}
}
*/

/*
int apache_busy_counting_sample_window(count_sample_recs_ptr input, sample_window_recs_ptr output) {
	if(input == NULL) return -1;	
	int i; int counter = 0; int sample_type = 0;
	if(apache_busy_marker_event) {
		apache_busy_event_buffer_size = 10;
		apache_busy_dummy_event_buffer_size = 1;
		apache_busy_swindow_recs = (struct sample_window_recs *) calloc(1, sizeof(struct sample_window_recs));  
		apache_busy_swindow_recs->swindow_recs_type = "apache_busy";
		apache_busy_swindow_recs->swindow_recs_num = apache_busy_event_buffer_size;
		apache_busy_swindow_recs->swindow_recs = (int *) calloc (apache_busy_event_buffer_size, sizeof(int));
		for(i = 0; i < input->csample_recs_num; i++) {
			if(strstr(input->count_sample_recs[i].metric_type, "apache_samoa4") == 0 || 
				strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
				sample_type = 1;
				if(input->count_sample_recs[i].sample_count > 0) {
					counter += 1;
				}
			}
		}
		printf("APACHE BUSY EVENT WINDOW -- COUNTER : %d \n", counter);
		if(sample_type == 1) { 
			if(counter > 1) { // Should be ideally 5 as we measure 5 metrics  Changed from 2 to 1
				apache_busy_swindow_recs->swindow_recs[0] = 1;
			} else {
				apache_busy_swindow_recs->swindow_recs[0] = 0;
			}
			printf("APACHE BUSY EVENT WINDOW -- SWINDOW 0 Index VAL : %d \n", apache_busy_swindow_recs->swindow_recs[0]);
			counter = 0;
			apache_busy_marker_event = 0;
		}
		return 0;
	} else {
		if(apache_busy_dummy_event_buffer_size < apache_busy_event_buffer_size) {
			for(i = 0; i < input->csample_recs_num; i++) {
				if(strstr(input->count_sample_recs[i].metric_type, "apache_samoa4") == 0 || 
					strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
					sample_type = 1;
					if(input->count_sample_recs[i].sample_count > 0) {
						counter += 1;
					}
				}
			}
			printf("APACHE BUSY EVENT WINDOW -- SAMPLE TYPE : %d \n", sample_type);
			printf("APACHE BUSY EVENT WINDOW -- Index %d COUNTER : %d \n", apache_busy_dummy_event_buffer_size, counter);
			if(sample_type == 1) {
				if(counter >= 1) { //Ideally should be 5 as we are measuring 5 metrics  //Changed value from 2 to 1
					apache_busy_swindow_recs->swindow_recs[apache_busy_dummy_event_buffer_size] = 1;
				} else {
					apache_busy_swindow_recs->swindow_recs[apache_busy_dummy_event_buffer_size] = 0;
				}
			}
			printf("APACHE BUSY EVENT WINDOW -- SWINDOW Index %d VAL : %d \n", apache_busy_dummy_event_buffer_size, 
			apache_busy_swindow_recs->swindow_recs[apache_busy_dummy_event_buffer_size]);
			apache_busy_dummy_event_buffer_size = apache_busy_dummy_event_buffer_size + 1;
			counter = 0;
			sample_type = 0;
			return 0;
		} else {
			printf("SAMPLE_EVENT_WINDOW APACHE BUSY EVENT WINDOW Crossed ....\n");
			apache_busy_marker_event = 1;  
			output->swindow_recs_type = strdup(apache_busy_swindow_recs->swindow_recs_type);
			output->swindow_recs_num = apache_busy_event_buffer_size; 
			output->swindow_recs = (int *) calloc (output->swindow_recs_num, sizeof(int));

			int i;
			for(i = 0; i < apache_busy_event_buffer_size; i++) {
				output->swindow_recs[i] = apache_busy_swindow_recs->swindow_recs[i];
			}

			apache_busy_dummy_event_buffer_size = 0; apache_busy_event_buffer_size = 0;
			free(apache_busy_swindow_recs);
			return 1;
		}
	}
}*/

/*
int apache_busy_local_counting_sample_window(count_sample_recs_ptr input, sample_window_recs_ptr output) {
	if(input == NULL) return -1;	
	int i;
	for(i = 0; i < input->csample_recs_num; i++) {
		int sample_count = input->count_sample_recs[i].sample_count;
		if(strcmp(input->count_sample_recs[i].metric_type, "apache_busy") == 0) { 
			if(apache_busy_marker_event) {
				apache_busy_event_buffer_size = 10;
				apache_busy_dummy_event_buffer_size = 1;
				apache_busy_swindow_recs = (struct sample_window_recs *) calloc(1, sizeof(struct sample_window_recs));  
				apache_busy_swindow_recs->swindow_recs_type = "apache_busy";
				apache_busy_swindow_recs->swindow_recs_num = apache_busy_event_buffer_size;
				apache_busy_swindow_recs->swindow_recs = (int *) calloc (apache_busy_event_buffer_size, sizeof(int));
				apache_busy_swindow_recs->swindow_recs[0] = sample_count;
				apache_busy_marker_event = 0;
				return 0;
			} else {
				if(apache_busy_dummy_event_buffer_size < apache_busy_event_buffer_size) {
					apache_busy_swindow_recs->swindow_recs[apache_dummy_event_buffer_size] = sample_count;
					apache_busy_dummy_event_buffer_size = apache_busy_dummy_event_buffer_size + 1;
					return 0;
				} else {
					printf("SAMPLE_EVENT_WINDOW APACHE BUSY EVENT WINDOW Crossed ....\n");
					apache_busy_marker_event = 1;  
					output->swindow_recs_type = strdup(apache_busy_swindow_recs->swindow_recs_type);
					output->swindow_recs_num = apache_busy_event_buffer_size; 
					output->swindow_recs = (int *) calloc (output->swindow_recs_num, sizeof(int));

					int i;
					for(i = 0; i < apache_busy_event_buffer_size; i++) {
						output->swindow_recs[i] = apache_busy_swindow_recs->swindow_recs[i];
					}

					apache_busy_dummy_event_buffer_size = 0; apache_busy_event_buffer_size = 0;
					free(apache_busy_swindow_recs);
					return 1;
				}
			}
		}
	}
}*/

/*
int rubis_fault_counting_sample_window(count_sample_recs_ptr input, sample_window_recs_ptr output) {
	if(input == NULL) return -1;	
	int i;
	for(i = 0; i < input->csample_recs_num; i++) {
		int sample_count = input->count_sample_recs[i].sample_count;
		if(strcmp(input->count_sample_recs[i].metric_type, "rubis_web_iftxpackets") == 0) { 
			if(rubis_fault_marker_event) {
				rubis_fault_event_buffer_size = 10;
				rubis_fault_dummy_event_buffer_size = 1;
				rubis_fault_swindow_recs = (struct sample_window_recs *) calloc(1, sizeof(struct sample_window_recs));  
				rubis_fault_swindow_recs->swindow_recs_type = "rubis_fault";
				rubis_fault_swindow_recs->swindow_recs_num = rubis_fault_event_buffer_size;
				rubis_fault_swindow_recs->swindow_recs = (int *) calloc (rubis_fault_event_buffer_size, sizeof(int));
				rubis_fault_swindow_recs->swindow_recs[0] = sample_count;
				rubis_fault_marker_event = 0;
				return 0;
			} else {
				if(rubis_fault_dummy_event_buffer_size < rubis_fault_event_buffer_size) {
					rubis_fault_swindow_recs->swindow_recs[apache_dummy_event_buffer_size] = sample_count;
					rubis_fault_dummy_event_buffer_size = rubis_fault_dummy_event_buffer_size + 1;
					return 0;
				} else {
					printf("SAMPLE_EVENT_WINDOW APACHE BUSY EVENT WINDOW Crossed ....\n");
					rubis_fault_marker_event = 1;  
					output->swindow_recs_type = strdup(rubis_fault_swindow_recs->swindow_recs_type);
					output->swindow_recs_num = rubis_fault_event_buffer_size; 
					output->swindow_recs = (int *) calloc (output->swindow_recs_num, sizeof(int));

					int i;
					for(i = 0; i < rubis_fault_event_buffer_size; i++) {
						output->swindow_recs[i] = rubis_fault_swindow_recs->swindow_recs[i];
					}

					rubis_fault_dummy_event_buffer_size = 0; rubis_fault_event_buffer_size = 0;
					free(rubis_fault_swindow_recs);
					return 1;
				}
			}
		}
	}
}*/

int rubis_fault_counting_sample_window(count_sample_recs_ptr input, sample_window_recs_ptr output) {
	printf("RUBIS FAULT GLOBAL COUNTING SAMPLE WINDOW ...\n");
	if(input == NULL) return -1;	
	int i; int counter = 0; int sample_type = 0;
	if(rubis_fault_marker_event) {
		rubis_fault_event_buffer_size = 15;		// Change from last 25 to last 15 events
		rubis_fault_dummy_event_buffer_size = 1;
		rubis_fault_swindow_recs = (struct sample_window_recs *) calloc(1, sizeof(struct sample_window_recs));  
		rubis_fault_swindow_recs->swindow_recs_type = "rubis_fault";
		rubis_fault_swindow_recs->swindow_recs_num = rubis_fault_event_buffer_size;
		rubis_fault_swindow_recs->swindow_recs = (int *) calloc (rubis_fault_event_buffer_size, sizeof(int));
		for(i = 0; i < input->csample_recs_num; i++) {
			if(strstr(input->count_sample_recs[i].metric_type, "rubis_samoa4") == 0 || 
				strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
				sample_type = 1;
				if(input->count_sample_recs[i].sample_count > 0) {
					counter += 1;
				}
			}
		}
		printf("RUBIS FAULT EVENT WINDOW -- COUNTER : %d \n", counter);
		if(sample_type == 1) { 
			if(counter > 2) { // Should be ideally 5 as we measure 5 metrics
				rubis_fault_swindow_recs->swindow_recs[0] = 1;
			} else {
				rubis_fault_swindow_recs->swindow_recs[0] = 0;
			}
			printf("RUBIS FAULT EVENT WINDOW -- SWINDOW 0 Index VAL : %d \n", rubis_fault_swindow_recs->swindow_recs[0]);
			counter = 0;
			rubis_fault_marker_event = 0;
		}
		return 0;
	} else {
		if(rubis_fault_dummy_event_buffer_size < rubis_fault_event_buffer_size) {
			printf("SAMPLE_EVENT_WINDOW RUBIS FAULT EVENT WINDOW -- ADDING SAMPLE BUFFER SIZE : %d \n", 
			rubis_fault_dummy_event_buffer_size);
			for(i = 0; i < input->csample_recs_num; i++) {
				if(strstr(input->count_sample_recs[i].metric_type, "rubis_samoa4") == 0 || 
					strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
					sample_type = 1;
					if(input->count_sample_recs[i].sample_count > 0) {
						counter += 1;
					}
				}
			}
			printf("RUBIS FAULT EVENT WINDOW -- Index %d COUNTER : %d \n", rubis_fault_dummy_event_buffer_size, counter);
			if(sample_type == 1) {
				if(counter > 2) { //Ideally should be 5 as we are measuring 5 metrics
					rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size] = 1;
				} else {
					rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size] = 0;
				}
			}
			printf("RUBIS FAULT EVENT WINDOW -- SWINDOW Index %d VAL : %d \n", rubis_fault_dummy_event_buffer_size, 
			rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size]);
			rubis_fault_dummy_event_buffer_size = rubis_fault_dummy_event_buffer_size + 1;
			counter = 0;
			sample_type = 0;
			return 0;
		} else {
			printf("SAMPLE_EVENT_WINDOW RUBIS FAULT EVENT WINDOW Crossed ....\n");
			rubis_fault_marker_event = 1;  
			output->swindow_recs_type = strdup(rubis_fault_swindow_recs->swindow_recs_type);
			output->swindow_recs_num = rubis_fault_event_buffer_size; 
			output->swindow_recs = (int *) calloc (output->swindow_recs_num, sizeof(int));

			int i;
			for(i = 0; i < rubis_fault_event_buffer_size; i++) {
				output->swindow_recs[i] = rubis_fault_swindow_recs->swindow_recs[i];
			}

			rubis_fault_dummy_event_buffer_size = 0; rubis_fault_event_buffer_size = 0;
			free(rubis_fault_swindow_recs);
			return 1;
		}
	}
}

unsigned long denom = 3000000000UL; //Sampling Freq *Num of Cores * Nanosec conversion == 3*1*(10*10*10*10*10*10*10*10*10);
unsigned long last_web_ctotaltime = 0UL; unsigned long last_serv_ctotaltime = 0UL; unsigned long last_db_ctotaltime = 0UL;
int LOCAL_BIN_NUMBER = 6; unsigned long last_ctotaltime = 0UL;
int LOCAL_WINDOW_SIZE = 20; int LOCAL_RANGE = 5;
static int buffer_current = 0;
static float buffer[20];

char lentropystring[512];
FILE *lentropylog;

char lbytesstr[512];
FILE *lbyteslog;
int lbytesavg = 0;

int rec_local_entropy_stats(sensor_recs_ptr input, recs_entropy_ptr output) {
	printf("Inside the libfunctions.la LOCAL_ENTROPY_STATS function .... \n");
	int i; int j; int total_recs = 0; float entropy_val = 0.0; float prob = 0.0; char rec_type[64] = "rubis1-serv";
       
    float web_cpu_util = 0.0; float serv_cpu_util = 0.0; float db_cpu_util = 0.0;
	float web_cpu_total = 0.0; float serv_cpu_total = 0.0; float db_cpu_total = 0.0;
	float entropy_serv = 0.0; //unsigned long last_ctotaltime = 0UL;

	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	lbyteslog = fopen("localentropybandwidthmeasure.log", "a");
	if(lbyteslog == NULL) {
		printf("Cannot open file.\n");
		return 1;
	}
	lbytesavg = lbytesavg + (int)(sizeof(input->sensor_recs[0].timestamp) * 2 + sizeof(input->sensor_recs[0].attr_name) * 2 +
	sizeof(input->sensor_recs[0].rec_val_int) * 2 + sizeof(input->sensor_recs[0].rec_val_float) * 2 + 
	sizeof(input->sensor_recs[0].rec_val) * 2);
	sprintf(lbytesstr, "\t %d \n", lbytesavg); 
	fwrite(lbytesstr, sizeof(char), strlen(lbytesstr), lbyteslog);
	fclose(lbyteslog);

	total_recs = input->sensor_recs_num;
    for (i = 0; i < total_recs; i++) {
		if(input->sensor_recs[i].attr_name != NULL) {
			if(strstr(input->sensor_recs[i].attr_name, "serv_ctotaltime") != NULL) {
				serv_cpu_util = ((input->sensor_recs[i].rec_val - last_ctotaltime)*100)/denom;
				serv_cpu_total = serv_cpu_total + serv_cpu_util;
				last_ctotaltime = input->sensor_recs[i].rec_val;
			 }
		}
	}  

	printf("SERV CPU UTIL === %f \n", serv_cpu_util);
    if(buffer_current < LOCAL_WINDOW_SIZE - 1) { /*buffer is still filling*/
         buffer[buffer_current] = serv_cpu_util; 
		 buffer_current++;
         return 0;
    } else {  /*buffer is full, push the oldest one*/ 
        buffer[buffer_current%LOCAL_WINDOW_SIZE] = serv_cpu_util;
		/*
		 * Latest local entropy calculation code, BEGIN
		 */
		int bins[LOCAL_BIN_NUMBER];
		float mean = 0.0;   
		float entropy = 0.0;   
		float norm = 0.0;
		
		// calculate mean value of samples in buffer
		for(i = 0; i < LOCAL_WINDOW_SIZE; i ++) {
			mean += buffer[i];        
		}
		mean /= LOCAL_WINDOW_SIZE;

		// setup counters
		for(i = 0; i < LOCAL_BIN_NUMBER; i ++) {
			bins[i] = 0;
		}     
	 
		for(i = 0; i < LOCAL_WINDOW_SIZE; i ++) {
			norm = buffer[i] / mean;
			int bin_index = (int) floor(norm * (LOCAL_BIN_NUMBER-1) / LOCAL_RANGE);   
			if(bin_index >= LOCAL_BIN_NUMBER - 1) {
				bin_index = LOCAL_BIN_NUMBER - 1;
			}
			if(bin_index > -1)
				bins[bin_index] ++; 
		}   

		for(i = 0; i < LOCAL_BIN_NUMBER; i ++) {
			float p = (float)bins[i] / LOCAL_WINDOW_SIZE;
			if(p != 0) { 
				entropy_serv = -1 * p * log(p) + entropy_serv; 
			}
		} 

		printf("ENTROPY_STATS SERV ENTROPY VALUE === %f \n", entropy_serv);
		/*struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		lentropylog = fopen("servlocalcpuutilentropyvalues.log", "a");
		if(lentropylog == NULL) {
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(lentropystring, "\t %f \t %f \n", serv_cpu_util, entropy_serv); 
		fwrite(lentropystring, sizeof(char), strlen(lentropystring), lentropylog);
		fclose(lentropylog);*/
	
		output->recs_type = strdup(rec_type);
		output->web_recs_num = 0;
		output->web_recs_entropy = 0.0;
		output->serv_recs_num = LOCAL_WINDOW_SIZE;
		output->serv_recs_entropy = entropy_serv;
		output->db_recs_num = 0;
		output->db_recs_entropy = 0.0;
		buffer_current++;
		return 1;
	}
}

int initialized = 0;/*flag to indicate if the buffer is initialized*/
int apache_arrived = 0;
int mysql_arrived = 0;
int tomcat_arrived = 0;
int apache_current = 0;
int mysql_current = 0;
int tomcat_current = 0;
int CHILD_BIN_NUMBER = 5;
int WINDOW_SIZE = 10;
int BIN_NUMBER = 125;
int RANGE = 4.75;
static entropy_trace trace_buffer;

char ebytestring[512];
FILE *ebyteslog;
int bytesavg;

char gbytestring[512];
FILE *gbyteslog;

FILE *entropybufflog;
char entropystring[512];

int rec_global_entropy_window(recs_entropy_ptr input, global_entropy_ptr output) {
	/*
	*  update buffer 
	*/
	int count = 0;
	printf("Entropy GLOBAL WINDOW COUNT : %d \n", count++);
	if(input == NULL) return 0;	

	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	ebyteslog = fopen("globalentropybandwidthmeasure.log", "a");
	if(ebyteslog == NULL){
		printf("Cannot open file.\n");
		return 1;
	}
	bytesavg = bytesavg + (int)(sizeof(input->web_recs_num) * 3 + sizeof(input->web_recs_entropy) * 3 + sizeof(input->recs_type)); 
	//sprintf(ebytestring, "\t %d \t %f \t %f \t %f \n", bytesavg, input->web_recs_entropy, input->serv_recs_entropy, input->db_recs_entropy); 
	sprintf(ebytestring, "\t %d \n", bytesavg); 
	fwrite(ebytestring, sizeof(char), strlen(ebytestring), ebyteslog);
	fclose(ebyteslog);

	if(initialized == 0) {
		trace_buffer.entropy_apache = (float *) calloc(WINDOW_SIZE, sizeof(float));
		trace_buffer.entropy_mysql = (float *) calloc(WINDOW_SIZE, sizeof(float));
		trace_buffer.entropy_tomcat = (float *) calloc(WINDOW_SIZE, sizeof(float));
		initialized = 1; 
	} 
    if(apache_current < WINDOW_SIZE - 1 || mysql_current < WINDOW_SIZE - 1 || tomcat_current < WINDOW_SIZE - 1) { /*trace_buffer is not filled yet*/
		if(strcmp(input->recs_type, "rubis1-web") == 0 && apache_current < WINDOW_SIZE - 1) {
			trace_buffer.entropy_apache[apache_current] = input->web_recs_entropy;
			apache_current++;
		}
        if(strcmp(input->recs_type, "rubis1-db") == 0 && mysql_current < WINDOW_SIZE - 1) {
             trace_buffer.entropy_mysql[mysql_current] = input->db_recs_entropy;
             mysql_current++;
		}
        if(strcmp(input->recs_type, "rubis1-serv") == 0 && tomcat_current < WINDOW_SIZE - 1) {
			trace_buffer.entropy_tomcat[tomcat_current] = input->serv_recs_entropy;
			tomcat_current++;
        }
		return 0;
	} else { /*trace_buffer is filled, waiting for three updates on local entropies*/
		if(strcmp(input->recs_type, "rubis1-web") == 0 && apache_arrived == 0) {
			trace_buffer.entropy_apache[apache_current%WINDOW_SIZE] = input->web_recs_entropy;
			apache_current++;
			apache_arrived = 1;
		}
		if(strcmp(input->recs_type, "rubis1-db") == 0 && mysql_arrived == 0) {
			trace_buffer.entropy_mysql[mysql_current%WINDOW_SIZE] = input->db_recs_entropy;
			mysql_current++;
			mysql_arrived = 1;
		}
		if(strcmp(input->recs_type, "rubis1-serv") == 0 && tomcat_arrived == 0) {
			trace_buffer.entropy_tomcat[tomcat_current%WINDOW_SIZE] = input->serv_recs_entropy;
			tomcat_current++;
			tomcat_arrived = 1;
		}
        if(apache_arrived == 1 && mysql_arrived == 1 && tomcat_arrived == 1) { /*trace_buffer is ready to compute*/
		/*
		 *  global entropy calculation entropy_trace buffer
		 */       
		/*int z;
		for(z = 0; z < WINDOW_SIZE; z++) {
			entropybufflog = fopen("localentropiesbuffer.log", "a");
			if(entropybufflog == NULL){
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(entropystring, "\t %f \t %f \t %f \n", (float)trace_buffer.entropy_apache[z], (float)trace_buffer.entropy_tomcat[z],
			(float)trace_buffer.entropy_mysql[z]); 
			fwrite(entropystring, sizeof(char), strlen(entropystring), entropybufflog);
			fclose(entropybufflog);
			printf("%f\t%f\t%f\n", trace_buffer.entropy_apache[z], trace_buffer.entropy_mysql[z], trace_buffer.entropy_tomcat[z]);
        }*/
		/*entropybufflog = fopen("localentropiesbuffer.log", "a");
		if(entropybufflog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(entropystring, "%s \n", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"); 
		fwrite(entropystring, sizeof(char), strlen(entropystring), entropybufflog);
		fclose(entropybufflog);*/
		/* code for calculate global_entropy begins*/	        
		int i;
		int bins[125];
		float apache_mean = 0.0;   
		float mysql_mean = 0.0;
		float tomcat_mean = 0.0;
		float entropy = 0.0;   
		float apache_norm = 0.0;
		float mysql_norm = 0.0;
		float tomcat_norm = 0.0;
    
		// calculate mean value of samples in buffer
		for(i = 0; i < WINDOW_SIZE; i ++) {
			apache_mean += trace_buffer.entropy_apache[i];        
			mysql_mean += trace_buffer.entropy_mysql[i];
			tomcat_mean += trace_buffer.entropy_tomcat[i];
		}
		apache_mean /= WINDOW_SIZE;
		mysql_mean /= WINDOW_SIZE;
		tomcat_mean /= WINDOW_SIZE;
		
		// setup counters
		for(i = 0; i < BIN_NUMBER; i ++) {
			bins[i] = 0;
		}     
		printf("apache_mean: %f, mysql_mean: %f, tomcat_mean: %f\n", apache_mean, mysql_mean, tomcat_mean);
		for(i = 0; i < WINDOW_SIZE; i ++) {
			apache_norm = trace_buffer.entropy_apache[i] / apache_mean;
			mysql_norm = trace_buffer.entropy_mysql[i] / mysql_mean;
			tomcat_norm = trace_buffer.entropy_tomcat[i] / tomcat_mean;
			int apache_index = (int)floor(apache_norm*4/4.75);
			if(apache_index >= 4-1) {
				apache_index = 4-1;
			}
			int mysql_index = (int)floor(mysql_norm*4/4.75);
			if(mysql_index >= 4-1) {
				mysql_index = 4-1;
			}
			int tomcat_index = (int)floor(tomcat_norm*4/4.75);
			if(tomcat_index >= 4-1) {
				tomcat_index = 4-1;
			}

			int bin_index = apache_index*4*4 + mysql_index*4 + tomcat_index;   
			//printf("%f|%d %f|%d %f|%d \n", apache_norm, apache_index, mysql_norm, mysql_index, tomcat_norm, tomcat_index);
			bins[bin_index] ++; 
		}   
		for(i = 0; i < BIN_NUMBER; i ++) {
			float p = (float)bins[i] / WINDOW_SIZE;
			if(p != 0) { 
				entropy = -1 * p * log(p) + entropy; 
			}
		}   
	 
		/* code for calculate global_entropy ends*/
		printf("\n Entropy: %f \n", entropy);
		output->web_recs_entropy = input->web_recs_entropy;
		output->serv_recs_entropy = input->serv_recs_entropy;
		output->db_recs_entropy = input->db_recs_entropy;
		output->global_recs_num = WINDOW_SIZE;
		output->global_entropy = entropy;
		/*struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		gbyteslog = fopen("localglobalentropymeasure.log", "a");
		if(gbyteslog == NULL) {
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(gbytestring, "\t %f \t %f \t %f \t %lf \n", input->web_recs_entropy, input->serv_recs_entropy, 
		input->db_recs_entropy, output->global_entropy); 
		fwrite(gbytestring, sizeof(char), strlen(gbytestring), gbyteslog);
		fclose(gbyteslog);*/
		apache_arrived = 0;
		mysql_arrived = 0;
		tomcat_arrived = 0; 
		return 1;
    } else {
      return 0; 
    } 
  }
} 

/*
int rec_global_entropy_window(recs_entropy_ptr input, global_entropy_ptr output) {
	printf("Entropy GLOBAL WINDOW ...\n");
	//int each_size = global_window_size/3;
	if(input == NULL) return 0;	
	printf("INPUT RECS TYPE %s \n", input->recs_type);
	printf("WEB ENTROPY %f ...\n", input->web_recs_entropy);
	printf("SERV ENTROPY %f ...\n", input->serv_recs_entropy);
	printf("DB ENTROPY %f ...\n", input->db_recs_entropy);

	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	ebyteslog = fopen("globalentropybandwidthmeasure.log", "a");
	if(ebyteslog == NULL){
		printf("Cannot open file.\n");
		return 1;
	}
	bytesavg = bytesavg + (int)(sizeof(input->web_recs_num) * 3 + sizeof(input->web_recs_entropy) * 3 + sizeof(input->recs_type)); 
	sprintf(ebytestring, "%lf \t %d \n", timestamp, bytesavg); 
	fwrite(ebytestring, sizeof(char), strlen(ebytestring), ebyteslog);
	fclose(ebyteslog);

	if(local_entropy_arrived == 0) {
		global_entropy_psensor_recs = (struct global_entropy *) calloc(1, sizeof(struct global_entropy));  
		global_entropy_psensor_recs->global_recs_num = local_entropy_arrived;
		global_entropy_psensor_recs->web_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
		global_entropy_psensor_recs->serv_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
		global_entropy_psensor_recs->db_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
	}    
	if(web_entropy_arrived != each_size || serv_entropy_arrived != each_size || db_entropy_arrived != each_size) {
		 if(strcmp(input->recs_type, "rubis1-web") == 0 && web_entropy_arrived < each_size) {
		   global_entropy_psensor_recs->web_local_entropy[web_entropy_arrived].web_recs_entropy = input->web_recs_entropy;
		   global_entropy_psensor_recs->web_local_entropy[web_entropy_arrived].recs_type = input->recs_type;
		   global_entropy_psensor_recs->web_local_entropy[web_entropy_arrived].web_recs_num = input->web_recs_num;
		   web_entropy_arrived ++;
		 } 
		 if(strcmp(input->recs_type, "rubis1-serv") == 0 && serv_entropy_arrived < each_size) {
		   global_entropy_psensor_recs->serv_local_entropy[serv_entropy_arrived].serv_recs_entropy = input->serv_recs_entropy;
		   global_entropy_psensor_recs->serv_local_entropy[serv_entropy_arrived].recs_type = input->recs_type;
		   global_entropy_psensor_recs->serv_local_entropy[serv_entropy_arrived].serv_recs_num = input->serv_recs_num;
		   serv_entropy_arrived ++;
		 }  
		 if(strcmp(input->recs_type, "rubis1-db") == 0 && db_entropy_arrived < each_size) {
		   global_entropy_psensor_recs->db_local_entropy[db_entropy_arrived].db_recs_entropy = input->db_recs_entropy;
		   global_entropy_psensor_recs->db_local_entropy[db_entropy_arrived].recs_type = input->recs_type;
		   global_entropy_psensor_recs->db_local_entropy[db_entropy_arrived].db_recs_num = input->db_recs_num;
		   db_entropy_arrived ++;
		 }
		 local_entropy_arrived ++;
	}
	printf("GLOBAL ENTROPY WEB ENTROPY ARRRIVED  %d ...\n", web_entropy_arrived);
	printf("GLOBAL ENTROPY SERV ENTROPY ARRRIVED  %d ...\n", serv_entropy_arrived);
	printf("GLOBAL ENTROPY DB ENTROPY ARRRIVED  %d ...\n", db_entropy_arrived);
	printf("GLOBAL ENTROPY LOCAL ENTROPY ARRRIVED  %d ...\n", local_entropy_arrived);
	if(local_entropy_arrived == global_window_size) {
		 printf("ENTROPY GLOBAL WINDOW CROSSED ...\n");
		 int i;
		 int bins[BIN_NUMBER];
		 double web_mean, serv_mean, db_mean;
		 double web_norm, serv_norm, db_norm;
		 double entropy_global = 0.0;
		 for(i = 0; i < BIN_NUMBER; i++) {
			   bins[i] = 0;
		 }
		 for(i = 0; i < each_size; i++) {
			web_mean = web_mean + global_entropy_psensor_recs->web_local_entropy[i].web_recs_entropy; 
			serv_mean = serv_mean + global_entropy_psensor_recs->serv_local_entropy[i].serv_recs_entropy;
			db_mean = db_mean + global_entropy_psensor_recs->db_local_entropy[i].db_recs_entropy;
		 }
		 web_mean /= each_size; 
		 serv_mean /= each_size;
		 db_mean /= each_size;
			printf("WEB MEAN ENTROPY %lf ...\n", web_mean);
			printf("SERV MEAN ENTROPY %lf ...\n", serv_mean);
			printf("DB MEAN ENTROPY %lf ...\n", db_mean);
		 for(i = 0; i < each_size; i++) {
			   web_norm = global_entropy_psensor_recs->web_local_entropy[i].web_recs_entropy/web_mean;
			   int web_index = (int) floor(web_norm * CHILD_BIN_NUMBER /4.75);
			   if(web_index > CHILD_BIN_NUMBER) { 
					 web_index = CHILD_BIN_NUMBER;
			   }         
			   serv_norm = global_entropy_psensor_recs->serv_local_entropy[i].serv_recs_entropy/serv_mean;
			   int serv_index = (int) floor(serv_norm * CHILD_BIN_NUMBER /4.75);
			   if(serv_index > CHILD_BIN_NUMBER) { 
					 serv_index = CHILD_BIN_NUMBER;
			   }
			   db_norm = global_entropy_psensor_recs->db_local_entropy[i].db_recs_entropy/web_mean;
			   int db_index = (int) floor(db_norm * CHILD_BIN_NUMBER /4.75);
			   if(db_index > CHILD_BIN_NUMBER) { 
					 db_index = CHILD_BIN_NUMBER;
			   }
			   int bin_index = web_index*((CHILD_BIN_NUMBER+1)*(CHILD_BIN_NUMBER+1))+db_index*(CHILD_BIN_NUMBER+1)+serv_index;
			   if(bin_index > -1)
				   bins[bin_index]++;
		  }   
		printf("BIN NUMBER %d \n", BIN_NUMBER);
		  for(i = 0; i < BIN_NUMBER; i++) {
			   double p = bins[i]/each_size;
			   if(p != 0) { 
				  entropy_global = -1 * p * log(p) + entropy_global; 
			   }
		  } 
		printf("GLOBAL ENTROPY VAL %lf \n", entropy_global);
		  output->web_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
		  output->serv_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
		  output->db_local_entropy = (struct recs_entropy *) calloc (each_size, sizeof(struct recs_entropy));
		  for(i = 0; i < each_size; i++) {
			output->web_local_entropy[i].web_recs_entropy = global_entropy_psensor_recs->web_local_entropy[i].web_recs_entropy;
			output->web_local_entropy[i].web_recs_num = global_entropy_psensor_recs->web_local_entropy[i].web_recs_num;
			output->serv_local_entropy[i].serv_recs_entropy = global_entropy_psensor_recs->serv_local_entropy[i].serv_recs_entropy;
			output->serv_local_entropy[i].serv_recs_num = global_entropy_psensor_recs->serv_local_entropy[i].serv_recs_num; 
			output->db_local_entropy[i].db_recs_entropy = global_entropy_psensor_recs->db_local_entropy[i].db_recs_entropy;
			output->db_local_entropy[i].db_recs_num = global_entropy_psensor_recs->db_local_entropy[i].db_recs_num;

			struct timeval now;
			gettimeofday(&now, NULL);
			double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
			gbyteslog = fopen("localglobalentropyvalues.log", "a");
			if(gbyteslog == NULL) {
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(gbytestring, "%lf \t %f \t %f \t %f \t %f \n", timestamp, output->web_local_entropy[i], output->serv_local_entropy[i], 
			output->db_local_entropy[i], output->global_entropy); 

			fwrite(gbytestring, sizeof(char), strlen(gbytestring), gbyteslog);
			fclose(gbyteslog);
		  }      

		local_entropy_arrived = 0;
		web_entropy_arrived = 0;
		serv_entropy_arrived = 0;
		db_entropy_arrived = 0;

		free(global_entropy_psensor_recs);
		output->web_recs_num = each_size;
		output->serv_recs_num = each_size;
		output->db_recs_num = each_size;
		output->global_recs_num = global_window_size;
		output->global_entropy = entropy_global;
		return 1;
	} else {
		return 0;
	}   
}*/


/*
int rec_global_entropy_window(recs_entropy_ptr input, global_entropy_ptr output) {
	//printf("INSIDE Entropy GLOBAL WINDOW ...\n");
	if(input == NULL) return -1;	
	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	ebyteslog = fopen("globalentropybandwidthmeasure.log", "a");
	if(ebyteslog == NULL){
		printf("Cannot open file.\n");
		return 1;
	}

	bytesavg = bytesavg + (int)(sizeof(input->web_recs_num) * 3 + sizeof(input->web_recs_entropy) * 3 + sizeof(input->recs_type)); 
	sprintf(ebytestring, "%lf \t %d \n", timestamp, bytesavg); 
	fwrite(ebytestring, sizeof(char), strlen(ebytestring), ebyteslog);
	fclose(ebyteslog);

	if(global_entropy_marker_event) {
		global_entropy_event_buffer_size = 3;
		//printf("GLOBAL_ENTROPY_WINDOW FUNCTION GENTROPY_EVENT_BUFFER_SIZE %d.... \n", global_entropy_event_buffer_size);
		global_entropy_dummy_event_buffer_size = 1;
		global_entropy_psensor_recs = (struct global_entropy *) calloc(1, sizeof(struct global_entropy));  
		global_entropy_psensor_recs->recs_num = global_entropy_event_buffer_size;
		global_entropy_psensor_recs->local_entropy = (struct recs_entropy *) calloc (global_entropy_event_buffer_size, sizeof(struct recs_entropy));
		if(input->recs_type != NULL)
			global_entropy_psensor_recs->local_entropy[0].recs_type = strdup(input->recs_type);
		global_entropy_psensor_recs->local_entropy[0].web_recs_num = input->web_recs_num;
		global_entropy_psensor_recs->local_entropy[0].web_recs_entropy = input->web_recs_entropy;
		global_entropy_psensor_recs->local_entropy[0].serv_recs_num = input->serv_recs_num;
		global_entropy_psensor_recs->local_entropy[0].serv_recs_entropy = input->serv_recs_entropy;
		global_entropy_psensor_recs->local_entropy[0].db_recs_num = input->db_recs_num;
		global_entropy_psensor_recs->local_entropy[0].db_recs_entropy = input->db_recs_entropy;
		global_entropy_marker_event = 0;
		return 0;
	} else {
		//printf("GLOBAL_ENTROPY_WINDOW function GENTROPY_EVENT_BUFFER_SIZE %d.... \n", global_entropy_event_buffer_size);
		//printf("GLOBAL_ENTROPY_WINDOW function GENTROPY_DUMMY_EVENT_BUFFER_SIZE %d.... \n", global_entropy_dummy_event_buffer_size);
		if(global_entropy_dummy_event_buffer_size < global_entropy_event_buffer_size) {
			//printf("GLOBAL_ENTROPY_WINDOW function GENTROPY ELAPSED WINDOW SIZE : %d \n",
			//(global_entropy_event_buffer_size - global_entropy_dummy_event_buffer_size));
			if(input->recs_type != NULL)
				global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].recs_type = strdup(input->recs_type);
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].web_recs_num = input->web_recs_num;
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].web_recs_entropy = input->web_recs_entropy;
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].serv_recs_num = input->serv_recs_num;
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].serv_recs_entropy = input->serv_recs_entropy;
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].db_recs_num = input->db_recs_num;
			global_entropy_psensor_recs->local_entropy[global_entropy_dummy_event_buffer_size].db_recs_entropy = input->db_recs_entropy;
			global_entropy_dummy_event_buffer_size = global_entropy_dummy_event_buffer_size + 1;
			return 0;
		} else {
			printf("GLOBAL ENTROPY WINDOW function GENTROPY EVENT WINDOW Crossed ....\n");
			global_entropy_marker_event = 1;  
			//output = global_entropy_plocal_entropy;
			output->recs_num = global_entropy_event_buffer_size; 
			output->local_entropy = (struct recs_entropy *) calloc (output->recs_num, sizeof(struct recs_entropy));

			output->web_recs_entropy = 0.0;
			output->serv_recs_entropy = 0.0;
			output->db_recs_entropy = 0.0;
			output->global_total_entropy = 0.0;

			int i = 0;
			for(i; i < global_entropy_event_buffer_size; i++) {
				if(global_entropy_psensor_recs->local_entropy[i].recs_type != NULL)
					output->local_entropy[i].recs_type = strdup(global_entropy_psensor_recs->local_entropy[i].recs_type);
				output->local_entropy[i].web_recs_num = global_entropy_psensor_recs->local_entropy[i].web_recs_num;
				output->local_entropy[i].web_recs_entropy = global_entropy_psensor_recs->local_entropy[i].web_recs_entropy;
				output->local_entropy[i].serv_recs_num = global_entropy_psensor_recs->local_entropy[i].serv_recs_num;
				output->local_entropy[i].serv_recs_entropy = global_entropy_psensor_recs->local_entropy[i].serv_recs_entropy;
				output->local_entropy[i].db_recs_num = global_entropy_psensor_recs->local_entropy[i].db_recs_num;
				output->local_entropy[i].db_recs_entropy = global_entropy_psensor_recs->local_entropy[i].db_recs_entropy;
				output->web_recs_entropy = output->web_recs_entropy + global_entropy_psensor_recs->local_entropy[i].web_recs_entropy; 
				output->serv_recs_entropy = output->serv_recs_entropy + global_entropy_psensor_recs->local_entropy[i].serv_recs_entropy; 
				output->db_recs_entropy = output->db_recs_entropy + global_entropy_psensor_recs->local_entropy[i].db_recs_entropy; 
			}

			output->global_total_entropy = output->web_recs_entropy + output->serv_recs_entropy + output->db_recs_entropy;

			global_entropy_dummy_event_buffer_size = 0; global_entropy_event_buffer_size = 0;
			free(global_entropy_psensor_recs);

			struct timeval now;
			gettimeofday(&now, NULL);
			double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
			gbyteslog = fopen("localglobalentropyvalues.log", "a");
			if(gbyteslog == NULL){
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(gbytestring, "%lf \t %f \t %f \t %f \t %f \n", timestamp, output->web_recs_entropy, output->serv_recs_entropy, 
			output->db_recs_entropy, output->global_total_entropy); 
			fwrite(gbytestring, sizeof(char), strlen(gbytestring), gbyteslog);
			fclose(gbyteslog);
			printf("OUT RECORD ==> web_recs_entropy : %f ...\n", output->web_recs_entropy);
			printf("OUT RECORD ==> serv_recs_entropy : %f ...\n", output->serv_recs_entropy);
			printf("OUT RECORD ==> db_recs_entropy : %f ...\n", output->db_recs_entropy);
			printf("OUT RECORD ==> global_total_entropy : %f ...\n", output->global_total_entropy);
			return 1;
		}
	}
}*/

/*
int rubis_fault_local_counting_sample_window(count_sample_recs_ptr input, sample_window_recs_ptr output) {
	printf("RUBIS FAULT LOCAL COUNTING SAMPLE WINDOW ...\n");
	if(input == NULL) return -1;	
	int i; int counter = 0; int sample_type = 0;
	if(rubis_fault_marker_event) {
		rubis_fault_event_buffer_size = 15;		// Change from last 25 to last 15 events
		rubis_fault_dummy_event_buffer_size = 1;
		rubis_fault_swindow_recs = (struct sample_window_recs *) calloc(1, sizeof(struct sample_window_recs));  
		rubis_fault_swindow_recs->swindow_recs_type = "rubis_fault";
		rubis_fault_swindow_recs->swindow_recs_num = rubis_fault_event_buffer_size;
		rubis_fault_swindow_recs->swindow_recs = (int *) calloc (rubis_fault_event_buffer_size, sizeof(int));
		for(i = 0; i < input->csample_recs_num; i++) {
			if(strstr(input->count_sample_recs[i].metric_type, "rubis_samoa4") == 0 || 
				strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
				sample_type = 1;
				if(input->count_sample_recs[i].sample_count > 0) {
					counter += 1;
				}
			}
		}
		printf("RUBIS FAULT EVENT WINDOW -- COUNTER : %d \n", counter);
		if(sample_type == 1) { 
			if(counter > 2) { // Should be ideally 5 as we measure 5 metrics
				rubis_fault_swindow_recs->swindow_recs[0] = 1;
			} else {
				rubis_fault_swindow_recs->swindow_recs[0] = 0;
			}
			printf("RUBIS FAULT EVENT WINDOW -- SWINDOW 0 Index VAL : %d \n", rubis_fault_swindow_recs->swindow_recs[0]);
			counter = 0;
			rubis_fault_marker_event = 0;
		}
		return 0;
	} else {
		if(rubis_fault_dummy_event_buffer_size < rubis_fault_event_buffer_size) {
			printf("SAMPLE_EVENT_WINDOW RUBIS FAULT EVENT WINDOW -- ADDING SAMPLE BUFFER SIZE : %d \n", 
			rubis_fault_dummy_event_buffer_size);
			for(i = 0; i < input->csample_recs_num; i++) {
				if(strstr(input->count_sample_recs[i].metric_type, "rubis_samoa4") == 0 || 
					strstr(input->count_sample_recs[i].metric_type, "ch4") == 0) { 
					sample_type = 1;
					if(input->count_sample_recs[i].sample_count > 0) {
						counter += 1;
					}
				}
			}
			printf("RUBIS FAULT EVENT WINDOW -- Index %d COUNTER : %d \n", rubis_fault_dummy_event_buffer_size, counter);
			if(sample_type == 1) {
				if(counter > 2) { //Ideally should be 5 as we are measuring 5 metrics
					rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size] = 1;
				} else {
					rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size] = 0;
				}
			}
			printf("RUBIS FAULT EVENT WINDOW -- SWINDOW Index %d VAL : %d \n", rubis_fault_dummy_event_buffer_size, 
			rubis_fault_swindow_recs->swindow_recs[rubis_fault_dummy_event_buffer_size]);
			rubis_fault_dummy_event_buffer_size = rubis_fault_dummy_event_buffer_size + 1;
			counter = 0;
			sample_type = 0;
			return 0;
		} else {
			printf("SAMPLE_EVENT_WINDOW RUBIS FAULT EVENT WINDOW Crossed ....\n");
			rubis_fault_marker_event = 1;  
			output->swindow_recs_type = strdup(rubis_fault_swindow_recs->swindow_recs_type);
			output->swindow_recs_num = rubis_fault_event_buffer_size; 
			output->swindow_recs = (int *) calloc (output->swindow_recs_num, sizeof(int));

			int i;
			for(i = 0; i < rubis_fault_event_buffer_size; i++) {
				output->swindow_recs[i] = rubis_fault_swindow_recs->swindow_recs[i];
			}

			rubis_fault_dummy_event_buffer_size = 0; rubis_fault_event_buffer_size = 0;
			free(rubis_fault_swindow_recs);
			return 1;
		}
	}
}*/

char bstring[512];
FILE *blog;

int apache_busy_sample_decision(sample_window_recs_ptr input) {
	if(input == NULL) return -1;	
	printf("APACHE BUSY SAMPLE DECISION ..\n");
	if(input->swindow_recs_type != 0) {
		if(strcmp(input->swindow_recs_type, "apache_busy") == 0) {
			int i;
			for(i = 0; i < input->swindow_recs_num; i++) {
				apache_load_sat_sample_count += input->swindow_recs[i];
			}
		}
		printf("RUBIS APACHE BUSY SAMPLE DECISION - SAMPLE COUNT : %d \n", apache_load_sat_sample_count);
		if(apache_load_sat_sample_count >  5) {
			struct timeval now;
			gettimeofday(&now, NULL);
			double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
			blog = fopen("busydetectiontime.log", "a");
			if(blog == NULL){
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(bstring, "Apache Busy Detection Time : %lf \n", timestamp);
			fwrite(bstring, sizeof(char), strlen(bstring), blog);
			fclose(blog);
			printf("RUBIS APACHE BUSY DETECTED ..\n");
			printf("RUBIS APACHE BUSY DETECTED TIMESTAMP : %f \n", timestamp);
			apache_load_sat_sample_count = 0;
			return 0;
		} 
		printf("APACHE BUSY THRESHOLD TRIGGER NOOOT ACTIVATED ..\n");
		apache_load_sat_sample_count = 0;
		return -1;
	}
}
/*
int apache_busy_local_sample_decision(sample_window_recs_ptr input) {
	if(input == NULL) return -1;	
	printf("APACHE BUSY SAMPLE DECISION ..\n");

	if(input->swindow_recs_type != 0) {
		if(strcmp(input->swindow_recs_type, "apache_busy") == 0) {
			printf("APACHE BUSY SAMPLE DECISION -- INSIDE APACHE BUSY \n");
			int i;
			for(i = 0; i < input->swindow_recs_num; i++) {
				printf("APACHE BUSY SAMPLE WINDOW CELL %d --- SCOUNT VAL: %d \n", i, input->swindow_recs[i]);
				apache_load_sat_sample_count += input->swindow_recs[i];
				printf("APACHE BUSY SAMPLE DECISION --- SAMPLE COUNT : %d \n", apache_load_sat_sample_count);
			}
		}
		if(apache_load_sat_sample_count >  0) {
			printf("APACHE THRESHOLD TRIGGER ACTIVATED ..\n");
			apache_load_sat_sample_count = 0;
			return 0;
		} 
		printf("APACHE THRESHOLD TRIGGER NOOOT ACTIVATED ..\n");
		apache_load_sat_sample_count = 0;
		return -1;
	}
}*/

/*
int rubis_fault_sample_decision(sample_window_recs_ptr input) {
	if(input == NULL) return -1;	
	printf("RUBIS FAULT SAMPLE DECISION ..\n");

	if(input->swindow_recs_type != 0) {
		if(strcmp(input->swindow_recs_type, "rubis_fault") == 0) {
			printf("RUBIS FAULT SAMPLE DECISION -- INSIDE RUBIS FAULT \n");
			int i;
			for(i = 0; i < input->swindow_recs_num; i++) {
				printf("RUBIS FAULT SAMPLE WINDOW CELL %d - SCOUNT VAL: %d \n", i, input->swindow_recs[i]);
				rubis_fault_sample_count += input->swindow_recs[i];
				printf("RUBIS FAULT SAMPLE DECISION - SAMPLE COUNT : %d \n", rubis_fault_sample_count);
			}
		}
		if(rubis_fault_sample_count >  0) {
			printf("RUBIS FAULT THRESHOLD TRIGGER ACTIVATED ..\n");
			rubis_fault_sample_count = 0;
			return 0;
		} 
		printf("RUBIS FAULT THRESHOLD TRIGGER NOOOT ACTIVATED ..\n");
		rubis_fault_sample_count = 0;
		return -1;
	}
}*/

int rubis_fault_sample_decision(sample_window_recs_ptr input) {
	if(input == NULL) return -1;	
	printf("INSIDE RUBIS FAULT SAMPLE DECISION ..\n");

	/*
	struct timeval now;
	gettimeofday(&now, NULL);
	double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
	byteslog = fopen("localbandwidthmeasure.log", "a");
	if(byteslog == NULL){
		printf("Cannot open file.\n");
		return 1;
	}
	sprintf(bytestring, "Time %lf Bytes %d \n", timestamp, (int)(sizeof(input->swindow_recs_type)));
	fwrite(bytestring, sizeof(char), strlen(bytestring), byteslog);
	fclose(byteslog);
	//printf("Timestamp : %lf Bytes Received %d ", timestamp, sizeof(sensor_recs_ptr));
	printf("Timestamp : %lf Bytes Received : %d \n", timestamp, (int)((input->swindow_recs_num * sizeof(input->swindow_recs[0]))));
	*/

	if(input->swindow_recs_type != 0) {
		if(strcmp(input->swindow_recs_type, "rubis_fault") == 0) {
			int i;
			for(i = 0; i < input->swindow_recs_num; i++) {
				rubis_fault_sample_count += input->swindow_recs[i];
			}
		}
		printf("RUBIS FAULT SAMPLE DECISION - SAMPLE COUNT : %d \n", rubis_fault_sample_count);
		if(rubis_fault_sample_count > 1) { // Moved from 0 15 5 // Value based on event window above
			struct timeval now;
			gettimeofday(&now, NULL);
			double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
			blog = fopen("faultdetectiontime.log", "a");
			if(blog == NULL){
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(bstring, "RUBiS Fault Detection Time : %lf \n", timestamp);
			fwrite(bstring, sizeof(char), strlen(bstring), blog);
			fclose(blog);
			printf("RUBIS FAULT DECTECTED ..\n");
			printf("RUBIS FAULT DETECTED TIMESTAMP : %f \n", timestamp);
			rubis_fault_sample_count = 0;
			return 0;
		} 
		printf("RUBIS FAULT THRESHOLD TRIGGER NOOOT ACTIVATED ..\n");
		rubis_fault_sample_count = 0;
		return -1;
	}
}

int rec_entropy_threshold_decision(global_entropy_ptr input) {
	if(input == NULL) return -1;
	printf("INSIDE GENTROPY THRESHOLD DECISION ..\n");
	printf("GENTROPY THRESHOLD DECISION - GENTROPY VAL : %f \n", input->global_entropy);
	if(input->global_entropy > 0) { //
		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		blog = fopen("entropydetectiontime.log", "a");
		if(blog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(bstring, "Entropy Fault Detection Time : %lf \n", timestamp);
		fwrite(bstring, sizeof(char), strlen(bstring), blog);
		fclose(blog);
		printf("ENTROPY FAULT DECTECTED ..\n");
		rubis_fault_sample_count = 0;
		return 0;
	} 
	printf("ENTROPY THRESHOLD NOOOT CROSSED ..\n");
	return -1;
}

/*
int rec_entropy_threshold_decision(global_entropy_ptr input) {
	if(input == NULL) return -1;
	float global_total_entropy = 0.0; float web_total_entropy = 0.0; float serv_total_entropy = 0.0; float db_total_entropy = 0.0;

	printf("INSIDE GENTROPY THRESHOLD DECISION ..\n");
	int i;
	for(i = 0; i < input->recs_num; i++) {
		if(strcmp(input->local_entropy[i].recs_type, "rubis1") == 0) {
			web_total_entropy = web_total_entropy + input->local_entropy[i].web_recs_entropy;
			serv_total_entropy = serv_total_entropy + input->local_entropy[i].serv_recs_entropy;
			db_total_entropy = db_total_entropy + input->local_entropy[i].db_recs_entropy;
		}
	}
	global_total_entropy = web_total_entropy + serv_total_entropy + db_total_entropy;
	printf("GENTROPY THRESHOLD DECISION - ENTROPY ALL : %f \n", global_entropy_count);
	if(global_entropy_count > 1) { // Moved from 0 15 5 // Value based on event window above
		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
		blog = fopen("entropydetectiontime.log", "a");
		if(blog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}
		sprintf(bstring, "Entropy Fault Detection Time : %lf \n", timestamp);
		fwrite(bstring, sizeof(char), strlen(bstring), blog);
		fclose(blog);
		printf("ENTROPY FAULT DECTECTED ..\n");
		printf("ENTROPY FAULT DETECTED TIMESTAMP : %f \n", timestamp);
		rubis_fault_sample_count = 0;
		return 0;
	} 
	printf("ENTROPY FAULT THRESHOLD NOOOT DETECTED ..\n");
	global_entropy_count = 0;
	return -1;
}*/

/*
int rubis_fault_local_sample_decision(sample_window_recs_ptr input) {
	if(input == NULL) return -1;	
	printf("INSIDE RUBIS FAULT SAMPLE DECISION ..\n");

	if(input->swindow_recs_type != 0) {
		if(strcmp(input->swindow_recs_type, "rubis_fault") == 0) {
			int i;
			for(i = 0; i < input->swindow_recs_num; i++) {
				rubis_fault_sample_count += input->swindow_recs[i];
			}
		}
		printf("RUBIS FAULT SAMPLE DECISION - SAMPLE COUNT : %d \n", rubis_fault_sample_count);
		if(rubis_fault_sample_count > 1) { // Moved from 0 15 5 // Value based on event window above
			struct timeval now;
			gettimeofday(&now, NULL);
			double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;
			blog = fopen("detectiontime.log", "a");
			if(blog == NULL){
				printf("Cannot open file.\n");
				return 1;
			}
			sprintf(bstring, "Fault Detection Time : %lf \n", timestamp);
			fwrite(bstring, sizeof(char), strlen(bstring), blog);
			fclose(blog);
			printf("RUBIS FAULT THRESHOLD FAULT DETECTED TIMESTAMP : %f \n", timestamp);
			rubis_fault_sample_count = 0;
			return 0;
		} 
		printf("RUBIS FAULT THRESHOLD TRIGGER NOOOT ACTIVATED ..\n");
		rubis_fault_sample_count = 0;
		return -1;
	}
}*/

int apache_counting_sample_decision(count_sample_recs_ptr input) {
	if(input == NULL) return -1;	
	int i;
	for(i = 0; i < input->csample_recs_num; i++) {
		char *metric_type = strdup((char *) input->count_sample_recs[i].metric_type);
		int sample_count = (int) input->count_sample_recs[i].sample_count;

		if(strcmp(metric_type, "apache_busy") == 0) {
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
			printf("Got Apache metric_type : %s \n", (char *) input->count_sample_recs[i].metric_type);
			printf("Got Apache sample_count : %d \n", (int) input->count_sample_recs[i].sample_count);
			if(sample_count >= 0) {
				printf("APACHE THRESHOLD TRIGGER ACTIVATED ..\n");
				return 0;
			}
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
		}
	}
	return -1;
}
