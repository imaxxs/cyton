#include "formatmanager.h"

static FMField sensor_field_list[] = 
{
	{"timestamp", "double", sizeof(double), FMOffset(sensor_rec_ptr, timestamp)},
	{"attr_name", "string", sizeof(char*), FMOffset(sensor_rec_ptr, attr_name)},
	{"rec_val_int", "integer", sizeof(int), FMOffset(sensor_rec_ptr, rec_val_int)},
	{"rec_val_float", "float", sizeof(float), FMOffset(sensor_rec_ptr, rec_val_float)},
	{"rec_val", "unsigned integer", sizeof(unsigned long), FMOffset(sensor_rec_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec sensor_format_list[] = 
{
    {"sensor", sensor_field_list, sizeof(sensor_rec), NULL},
    {NULL, NULL}
};

static FMField sensor_recs_field_list[] = 
{
	{"sensor_recs_num", "integer", sizeof(int), FMOffset(sensor_recs_ptr, sensor_recs_num)},
	{"sensor_recs", "sensor_recs[sensor_recs_num]", sizeof(struct sensor_rec), FMOffset(sensor_recs_ptr, sensor_recs)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec sensor_recs_format_list[] = 
{
    {"sensor_recs_list", sensor_recs_field_list, sizeof(sensor_recs), NULL},
    {"sensor_recs", sensor_field_list, sizeof(sensor_rec), NULL},
    {NULL, NULL}
};

static FMField count_sample_field_list[] = 
{
	{"metric_type", "string", sizeof(char *), FMOffset(count_sample_rec_ptr, metric_type)},
	{"sample_count", "integer", sizeof(int), FMOffset(count_sample_rec_ptr, sample_count)},
	{NULL, NULL, 0, 0}
};

static FMField count_sample_recs_field_list[] = 
{
	{"csample_recs_num", "integer", sizeof(int), FMOffset(count_sample_recs_ptr, csample_recs_num)},
	{"csample_recs", "count_sample_recs[csample_recs_num]", sizeof(struct count_sample_rec), FMOffset(count_sample_recs_ptr, count_sample_recs)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec count_sample_recs_format_list[] = 
{
    {"csample_recs_list", count_sample_recs_field_list, sizeof(struct count_sample_recs), NULL},
    {"count_sample_recs", count_sample_field_list, sizeof(struct count_sample_rec), NULL},
    {NULL, NULL}
};

static FMField sample_window_recs_field_list[] = 
{
	{"swindow_recs_type", "string", sizeof(char *), FMOffset(sample_window_recs_ptr, swindow_recs_type)},
	{"swindow_recs_num", "integer", sizeof(int), FMOffset(sample_window_recs_ptr, swindow_recs_num)},
	{"swin_recs", "integer[swindow_recs_num]", sizeof(int), FMOffset(sample_window_recs_ptr, swindow_recs)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec sample_window_recs_format_list[] = 
{
    {"sample_window_recs_list", sample_window_recs_field_list, sizeof(struct sample_window_recs), NULL},
    {NULL, NULL}
};

static FMField recs_entropy_field_list[] = 
{
	{"recs_type", "string", sizeof(char *), FMOffset(recs_entropy_ptr, recs_type)},
	{"web_recs_num", "integer", sizeof(int), FMOffset(recs_entropy_ptr, web_recs_num)},
	{"web_recs_entropy", "float", sizeof(float), FMOffset(recs_entropy_ptr, web_recs_entropy)},
	{"serv_recs_num", "integer", sizeof(int), FMOffset(recs_entropy_ptr, serv_recs_num)},
	{"serv_recs_entropy", "float", sizeof(float), FMOffset(recs_entropy_ptr, serv_recs_entropy)},
	{"db_recs_num", "integer", sizeof(int), FMOffset(recs_entropy_ptr, db_recs_num)},
	{"db_recs_entropy", "float", sizeof(float), FMOffset(recs_entropy_ptr, db_recs_entropy)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_entropy_format_list[] = 
{
    {"recs_entropy_format_list", recs_entropy_field_list, sizeof(struct recs_entropy), NULL},
    {NULL, NULL}
};

/*
static FMField global_entropy_field_list[] = 
{
	{"recs_num", "integer", sizeof(int), FMOffset(global_entropy_ptr, recs_num)},
	{"local_entropy", "recs_entropy[recs_num]", sizeof(struct recs_entropy), FMOffset(global_entropy_ptr, local_entropy)},
	{"web_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, web_recs_entropy)},
	{"serv_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, serv_recs_entropy)},
	{"db_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, db_recs_entropy)},
	{"global_total_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, global_total_entropy)},
	{NULL, NULL, 0, 0}
};*/

static FMField global_entropy_field_list[] = 
{
	{"global_recs_num", "integer", sizeof(int), FMOffset(global_entropy_ptr, global_recs_num)},
	{"global_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, global_entropy)},
	{"web_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, web_recs_entropy)},
	{"serv_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, serv_recs_entropy)},
	{"db_recs_entropy", "float", sizeof(float), FMOffset(global_entropy_ptr, db_recs_entropy)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec global_entropy_format_list[] = 
{
    {"global_entropy_list", global_entropy_field_list, sizeof(struct global_entropy), NULL},
    {NULL, NULL}
};

/*
static FMField recs_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(recs_ptr, attr_name)},
	{"num_recs", "integer", sizeof(int), FMOffset(recs_ptr, num_recs)},
	{"rec_val", "float[num_recs]", sizeof(float), FMOffset(recs_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_format_list[] = 
{
    {"recs", recs_field_list, sizeof(recs), NULL},
    {NULL, NULL}
};

static FMField recs_range_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(recs_range_ptr, attr_name)},
	{"key_srange", "float", sizeof(float), FMOffset(recs_range_ptr, key_srng)},
	{"key_erange", "float", sizeof(float), FMOffset(recs_range_ptr, key_erng)},
	{"num_recs", "integer", sizeof(int), FMOffset(recs_range_ptr, num_recs)},
	{"rec_val", "float[num_recs]", sizeof(float), FMOffset(recs_range_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_range_format_list[] = 
{
    {"recs_range", recs_range_field_list, sizeof(recs_range), NULL},
    {NULL, NULL}
};

static FMField recs_range_level_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(recs_range_level_ptr, attr_name)},
	{"key_srange", "float", sizeof(float), FMOffset(recs_range_level_ptr, key_srng)},
	{"key_erange", "float", sizeof(float), FMOffset(recs_range_level_ptr, key_erng)},
	{"range_level", "integer", sizeof(int), FMOffset(recs_range_level_ptr, range_level)},
	{"num_recs", "integer", sizeof(int), FMOffset(recs_range_level_ptr, num_recs)},
	{"rec_val", "float[num_recs]", sizeof(float), FMOffset(recs_range_level_ptr, rec_val)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_range_level_format_list[] = 
{
    {"recs_range_level", recs_range_level_field_list, sizeof(recs_range_level), NULL},
    {NULL, NULL}
};

static FMField recs_range_level_list_field_list[] = 
{
	{"num_range_recs", "integer", sizeof(int), FMOffset(recs_range_level_list_ptr, num_range_recs)},
	{"recs_range_level", "recs_range_level[num_range_recs]", sizeof(struct recs_range_level), FMOffset(recs_range_level_list_ptr, recs_range)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_range_level_list_format_list[] = 
{
    {"recs_range_level_list", recs_range_level_list_field_list, sizeof(recs_range_level_list), NULL},
    {"recs_range_level", recs_range_level_field_list, sizeof(recs_range_level), NULL},
    {NULL, NULL}
};

static FMField rec_stats_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(rec_stats_ptr, attr_name)},
	{"key_srange", "float", sizeof(float), FMOffset(rec_stats_ptr, key_srng)},
	{"key_erange", "float", sizeof(float), FMOffset(rec_stats_ptr, key_erng)},
	{"rec_int_cnt", "integer", sizeof(int), FMOffset(rec_stats_ptr, rec_int_cnt)},
	{"rec_float_cnt", "float", sizeof(float), FMOffset(rec_stats_ptr, rec_float_cnt)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec rec_stats_format_list[] = 
{
    {"rec_stats", rec_stats_field_list, sizeof(rec_stats), NULL},
    {NULL, NULL}
};

static FMField recs_stats_range_list_field_list[] = 
{
	{"num_stats_recs", "integer", sizeof(int), FMOffset(recs_stats_range_list_ptr, num_stats_recs)},
	{"recs_stats", "rec_stats[num_stats_recs]", sizeof(struct rec_stats), FMOffset(recs_stats_range_list_ptr, recs_stats)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_stats_range_list_format_list[] = 
{
    {"recs_stats_range_list", recs_stats_range_list_field_list, sizeof(recs_stats_range_list), NULL},
    {"rec_stats", rec_stats_field_list, sizeof(rec_stats), NULL},
    {NULL, NULL}
};

static FMField rec_stats_threshold_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(rec_stats_threshold_ptr, attr_name)},
	{"key_srange", "float", sizeof(float), FMOffset(rec_stats_threshold_ptr, key_srng)},
	{"key_erange", "float", sizeof(float), FMOffset(rec_stats_threshold_ptr, key_erng)},
	{"threshold_val", "integer", sizeof(int), FMOffset(rec_stats_threshold_ptr, threshold_val)},
	{"rec_int_cnt", "integer", sizeof(int), FMOffset(rec_stats_threshold_ptr, rec_int_cnt)},
	{"rec_float_cnt", "float", sizeof(float), FMOffset(rec_stats_threshold_ptr, rec_float_cnt)},
	{NULL, NULL, 0, 0}
};

static FMField recs_stats_threshold_range_list_field_list[] = 
{
	{"num_range_recs", "integer", sizeof(int), FMOffset(recs_stats_threshold_range_list_ptr, num_stats_recs)},
	{"rec_stats_threshold", "rec_stats_threshold[num_range_recs]", sizeof(struct rec_stats_threshold), FMOffset(recs_stats_threshold_range_list_ptr, 
	recs_stats_threshold)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_stats_threshold_range_list_format_list[] = 
{
    {"recs_stats_threshold_range_list", recs_stats_threshold_range_list_field_list, sizeof(recs_stats_threshold_range_list), NULL},
    {"rec_stats_threshold", rec_stats_threshold_field_list, sizeof(rec_stats_threshold), NULL},
    {NULL, NULL}
};

static FMField recs_entropy_stats_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(recs_entropy_stats_ptr, attr_name)},
	{"total_recs", "integer", sizeof(int), FMOffset(recs_entropy_stats_ptr, total_recs)},
	{"recs_entropy", "float", sizeof(float), FMOffset(recs_entropy_stats_ptr, recs_entropy)},
	{NULL, NULL, 0, 0}
};

static FMField recs_entropy_stats_threshold_field_list[] = 
{
	{"attr_name", "string", sizeof(char*), FMOffset(recs_entropy_stats_threshold_ptr, attr_name)},
	{"total_recs", "integer", sizeof(int), FMOffset(recs_entropy_stats_threshold_ptr, total_recs)},
	{"recs_entropy", "float", sizeof(float), FMOffset(recs_entropy_stats_threshold_ptr, recs_entropy)},
	{"recs_entropy_threshold", "float", sizeof(float), FMOffset(recs_entropy_stats_threshold_ptr, recs_entropy_threshold)},
	{NULL, NULL, 0, 0}
};

static FMStructDescRec recs_entropy_stats_format_list[] = 
{
    {"recs_entropy_stats", recs_entropy_stats_field_list, sizeof(recs_entropy_stats), NULL},
    {NULL, NULL}
};

static FMStructDescRec recs_entropy_stats_threshold_format_list[] = 
{
    {"recs_entropy_threshold_stats", recs_entropy_stats_threshold_field_list, sizeof(recs_entropy_stats_threshold), NULL},
    {NULL, NULL}
};
*/

FMStructDescRec *get_format_list(char *format) {
	if(strcmp("sensor", format) == 0) {
		return sensor_format_list;
	} else if(strcmp("sensor_recs", format) == 0) {
		return sensor_recs_format_list;
	} else if(strcmp("count_sample_recs", format) == 0) {
		return count_sample_recs_format_list;
	} else if(strcmp("sample_window_recs", format) == 0) {
		return sample_window_recs_format_list;
	} else if(strcmp("recs_entropy", format) == 0) {
		return recs_entropy_format_list;
	} else if(strcmp("global_entropy", format) == 0) {
		return global_entropy_format_list;
	}
}
