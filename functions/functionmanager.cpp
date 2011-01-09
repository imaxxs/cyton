#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "evpath.h"
#include "revpath.h"
#include "gen_thread.h"

typedef struct resrc_rec {
	float key_srng;
	float key_erng;
	int num_recs; // should be 120 isnt it
	float *rec_val;
} resrc_rec, *resrc_rec_ptr;

typedef struct stats_rec {
	float key_srng;
	float key_erng;
	//int num_recs;
	int rec_cnt;
} stats_rec, *stats_rec_ptr;

typedef struct mng_rec {
	int num_rec;
	struct stats_rec *stats_rec;	
} mng_rec, *mng_rec_ptr;

/*
typedef struct resrc_rec {
	int num_recs;
	float *rec_val;
} resrc_rec, *resrc_rec_ptr;

typedef struct stats_rec {
	int num_recs;
	int *rec_cnt;
} stats_rec, *stats_rec_ptr;
*/

/*
void cpu_usage_data(struct resrc_rec *cpu_usage) {
	float cpu_idle; 
	FILE *mfp = get_mon_sample_log();
	if(mfp == NULL) { 
	  printf("%s\n", "Cannot get handle to mon.log file"); 
	  exit(0);
	}

	cpu_usage->num_recs = 0;
	cpu_usage->rec_val = (float *) calloc(120, sizeof(float *));
	while (fgets(line, 256, mfp) != NULL && !feof(mfp)) {
		sscanf(line, "%s", perline);
		sscanf(perline, "%f,%f", &cpu_idle, &free_mem);
		cpu_usage->rec_val[cpu_usage>num_records] = cpu_idle;
		cpu_usage->num_records += 1;
	}
}

void cpu_mem_usage_data(struct resrc_rec *cpu_mem_usage) {
	float cpu_idle; //, free_mem;
	FILE *mfp = get_mon_sample_log();
	if(mfp == NULL) { 
	  printf("%s\n", "Cannot get handle to mon.log file"); 
	  exit(0);
	}

	cpu_mem_usage[0]->num_recs = 0;
	cpu_mem_usage[1]->num_recs = 0;

	cpu_usage->rec_val = (float *) calloc(120, sizeof(float *));
	while (fgets(line, 256, mfp) != NULL && !feof(mfp)) {
		sscanf(line, "%s", perline);
		sscanf(perline, "%f,%f", &cpu_idle, &free_mem);
		cpu_mem_usage[0]->rec_val[cpu_mem_usage[0]->num_records] = cpu_idle;
		cpu_mem_usage[0]->num_records += 1;
		cpu_mem_usage[1]->rec_val[cpu_mem_usage[1]->num_records] = cpu_idle;
		cpu_mem_usage[1]->num_records += 1;
	}
}*/

//foo(void* input, void* output, void *data)
/*
int range(struct mng_rec *mng_rec, struct resrc_rec *cpu_mem_usage) {
	int cpu_rec_start = mng_rec[0].stats_record[0].key_srng;
	int cpu_rec_end = mng_rec[0].stats_record[0].key_erng;

    sum = sum + ((simple_rec_ptr)input)->integer_field;
	int mem_rec_start = mng_rec[0].stats_record[1].key_srng;
	int mem_rec_end = mng_rec[0].stats_record[1].key_erng;

	if(cpu_mem_usage[0].num_recs != 0) {
		int i;
		for(i = 0; i < cpu_mem_usage[0].num_recs; i++) {
			if(cpu_rec_start < cpu_mem_usage[0].rec_val[i] < cpu_rec_end) {
				mng_rec[0].stats_record[0].rec_cnt += 1;				
			}
		}
	}

	if(cpu_mem_usage[1].num_recs != 0) {
		int j;
		for(j = 0; j < cpu_mem_usage[0].num_recs; j++) {
			if(mem_rec_start < cpu_mem_usage[0].rec_val[j] < mem_rec_end) {
				mng_rec[0].stats_record[1].rec_cnt += 1;				
			}
		}
	}

	if(mng_rec[0].stats_record[0].rec_cnt > 10) {
		return 1;
	} else {
		return 0;
	}
}
*/

int range(void* input, void* output, void *data) {
	float cpu_rec_start = ((resrc_rec)input).key_srng;
	float cpu_rec_end = ((resrc_rec)input)->key_erng;
	int cpu_num_recs = ((resrc_rec)input)->num_recs;
	float *cpu_rec_val = ((resrc_rec)input)->rec_val;

	if(cpu_num_recs != 0) {
		int i;
		for(i = 0; i < cpu_num_recs; i++) {
			if(cpu_rec_start < cpu_rec_val[i] < cpu_rec_end) {
				((stats_rec)output)->rec_cnt += 1;				
			}
		}
	}

	if(((stats_rec)output)->rec_cnt > 10) {
		return 1;
	} else {
		return 0;
	}
}

