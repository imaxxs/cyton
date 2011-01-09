#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct sensor_rec {
	double sampling_window;
	int sampling_rate;
	double timestamp;
	char *attr_name;
	float rec_val;
} sensor_rec, *sensor_rec_ptr;

typedef struct sensor_recs {
	int sensor_recs_num;
	struct sensor_rec *sensor_recs;
} sensor_recs, *sensor_recs_ptr;

static int cmpeventorder (const void *p1, const void *p2) {
	printf(" First param %f \n ", ((struct sensor_rec *)p1)->timestamp);
	printf(" Second param %f \n ", ((struct sensor_rec *)p2)->timestamp);
	return (((struct sensor_rec *)p1)->timestamp - ((struct sensor_rec *)p2)->timestamp);
}

static int cmpeventtype (const void *p1, const void *p2) {
	return strcmp(((struct sensor_rec *)p1)->attr_name, ((struct sensor_rec *)p2)->attr_name);
}

main () {
	struct sensor_recs *input = (struct sensor_recs *) calloc (1, sizeof(struct sensor_recs));
	input->sensor_recs_num = 4;
	input->sensor_recs = (struct sensor_rec *) calloc (1, sizeof(struct sensor_rec));

	input->sensor_recs[0].sampling_window = 10;
	input->sensor_recs[0].sampling_rate = 1;
	input->sensor_recs[0].timestamp = 15;
	input->sensor_recs[0].attr_name = "cpu_";
	input->sensor_recs[0].rec_val = 98.99;

	input->sensor_recs[1].sampling_window = 5;
	input->sensor_recs[1].sampling_rate = 1;
	input->sensor_recs[1].timestamp = 30;
	input->sensor_recs[1].attr_name = "mem_";
	input->sensor_recs[1].rec_val = 97.99;

	input->sensor_recs[2].sampling_window = 10;
	input->sensor_recs[2].sampling_rate = 1;
	input->sensor_recs[2].timestamp = 5;
	input->sensor_recs[2].attr_name = "cpu_";
	input->sensor_recs[2].rec_val = 98.99;

	input->sensor_recs[3].sampling_window = 5;
	input->sensor_recs[3].sampling_rate = 1;
	input->sensor_recs[3].timestamp = 10;
	input->sensor_recs[3].attr_name = "mem_";
	input->sensor_recs[3].rec_val = 97.99;

	qsort(input->sensor_recs, input->sensor_recs_num, sizeof(struct sensor_rec), cmpeventtype);
	int i;
	for(i = 0; i < 4; i++) {
		printf(" Time stamp value %s \n ", input->sensor_recs[i].attr_name);
	}
}
