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
	struct timeval now;
	int ret = gettimeofday(&now, NULL);
	if(ret) printf("Failure to execute command gettimeofday ... \n");
	if(atoi(argv[1]) == 0) {
		printf("Run start timestamp in usec : %f \n ", ((double)now.tv_sec*1.0e6 + (double)now.tv_usec));
	} else if (atoi(argv[1]) == 1) {
		printf("Run end timestamp in usec : %f \n ", ((double)now.tv_sec*1.0e6 + (double)now.tv_usec));
	}
}
