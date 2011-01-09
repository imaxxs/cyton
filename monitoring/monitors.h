#ifndef MONITORS_H
#define MONITORS_H

#include "../util/common.h"

typedef struct resrc_rec {
	float key_srng;
	float key_erng;
	int num_recs;
	float *rec_val;
} resrc_rec, *resrc_rec_ptr;

typedef struct base_resrc_rec {
	float cpu_key_srng;
	float cpu_key_erng;
	int cpu_num_recs;
	float *cpu_rec_val;
	float mem_key_srng;
	float mem_key_erng;
	int mem_num_recs;
	float *mem_rec_val;
} base_resrc_rec, *base_resrc_rec_ptr;

typedef struct range_rec {
	char *attr_name;
	//float key_srng;
	//float key_erng;
	int num_recs;
	float *rec_val;
} range_rec, *range_rec_ptr;

typedef struct range_recs_list {
	int num_range_recs;
	struct range_rec *range_recs;
} range_recs_list, *range_recs_list_ptr;
	
typedef struct stats_rec {
	char *attr_name;
	float key_srng;
	float key_erng;
	int rec_int_cnt;
	float rec_float_cnt;
} stats_rec, *stats_rec_ptr;

typedef struct stats_recs_list {
	int num_stats_recs;
	struct stats_rec *stats_recs;
} stats_recs_list, *stats_recs_list_ptr;

typedef struct sensor_rec {
	double timestamp;
	char *attr_name;
	int rec_val_int;
	float rec_val_float;
	unsigned long rec_val;
} sensor_rec, *sensor_rec_ptr;

typedef struct recs {
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

extern FMStructDescRec *get_format_list(char *format); 
extern void msg_coord_for_monitors_streams(char *node_contact_list, char *node_coord_contact_list, struct stone_context *stone_ctx, 
struct func_context *func_ctx, struct mon_context *mon_ctx, struct context *ctx); 
extern void clean_split_context(int num_recs, struct split_context *split_ctx);
extern void clean_stone_context(int num_recs, struct stone_context *stone_ctx);
extern void clean_func_context(int num_recs, struct func_context *func_ctx); 
extern void clean_mon_context(int num_recs, struct mon_context *mon_ctx); 

#endif
