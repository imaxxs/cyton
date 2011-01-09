#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


float timediff(const struct timeval *thistime, const struct timeval *lasttime) {
	float diff; 
	diff = ((double) thistime->tv_sec * 1.0e6 + 
			(double) thistime->tv_usec - 
			(double) lasttime->tv_sec * 1.0e6 - 
			(double) lasttime->tv_usec) / 1.0e6; 
	return diff;
}

int main(int argc, char **argv) {
	float timediff_threshold = ((double) atoi(argv[1]) * 1.0e6);
	struct timeval last; struct timeval now;
	int ret = gettimeofday(&last, NULL);
	if(ret) printf("Failure to execute command gettimeofday ... \n");
	printf("Run start timestamp in usec : %f \n ", ((double)last.tv_sec*1.0e6 + (double)last.tv_usec));
	do {
		gettimeofday(&now, NULL);
		
	} while(timediff(now, last) > timediff_threshold);
	printf("Run end timestamp in usec : %f \n ", ((double)last.tv_sec*1.0e6 + (double)last.tv_usec));
}
