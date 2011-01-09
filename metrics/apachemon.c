#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <ctype.h>
#include <curl/curl.h>

char * skip_whitespace (const char *p)
{
    while (isspace((unsigned char)*p)) p++;
    return (char *)p;
}
 
char * skip_token (const char *p)
{
    while (isspace((unsigned char)*p)) p++;
    while (*p && !isspace((unsigned char)*p)) p++;
    return (char *)p;
}         

double mapache_req_rate = 0; double mapache_bytes_rate = 0; double mapache_bytes_req = 0; 
int mapache_busy_workers = 0; int mapache_idle_workers = 0; int mapache_up_time = 0;
double mapache_cpu_load = 0; int mapache_total_access = 0; int mapache_total_kbytes = 0;
int mapache_available = 0;

size_t handle_data( void *ptr, size_t size, size_t nmemb, void *stream) {
	//printf("Size of data returned %ld \n", size*nmemb);
	char lastchar = *((char *) ptr + size*nmemb - 1);
	*((char *) ptr + size*nmemb - 1) = '\0';
	char apache_status[8192];
	strncpy(apache_status, (char *)ptr, 8192);

	//printf("%s\n", apache_status);
	if((size*nmemb) != 0) {
		mapache_available = 1;
	}
    const char *apache_req_rate = strstr(apache_status, "ReqPerSec:");
	if(apache_req_rate) {
		apache_req_rate = skip_token(apache_req_rate);
		//printf("%lf\n", strtod(((char *)apache_req_rate), ((char **)NULL)));
		mapache_req_rate = strtod(((char *)apache_req_rate), ((char **)NULL));
	}
    const char *apache_bytes_rate = strstr(apache_status, "BytesPerSec:");
	if(apache_bytes_rate) {
		apache_bytes_rate = skip_token(apache_bytes_rate);
		//printf("%lf\n", strtod(((char *)apache_bytes_rate), ((char **)NULL)));
		mapache_bytes_rate = strtod(((char *)apache_bytes_rate), ((char **)NULL));
	}
    const char *apache_bytes_req = strstr(apache_status, "BytesPerReq:");
	if(apache_bytes_req) {
		apache_bytes_req = skip_token(apache_bytes_req);
		//printf("%lf\n", strtod(((char *)apache_bytes_req), ((char **)NULL)));
		mapache_bytes_req = strtod(((char *)apache_bytes_req), ((char **)NULL));
	}
    const char *apache_busy_workers = strstr(apache_status, "BusyWorkers:");
	if(apache_busy_workers) {
		apache_busy_workers = skip_token(apache_busy_workers);
		//printf("%d\n", atoi(((char *)apache_busy_workers)));
		mapache_busy_workers = atoi(((char *)apache_busy_workers));
	}
    const char *apache_idle_workers = strstr(apache_status, "IdleWorkers:");
	if(apache_idle_workers) {
		apache_idle_workers = skip_token(apache_idle_workers);
		//printf("%d\n", atoi(((char *)apache_idle_workers)));
		mapache_idle_workers = atoi(((char *)apache_idle_workers));
	}
    const char *apache_cpu_load = strstr(apache_status, "CPULoad:");
	if(apache_cpu_load) {
		apache_cpu_load = skip_token(apache_cpu_load);
		//printf("%lf\n", strtod(((char *)apache_cpu_load), ((char **)NULL)));
		mapache_cpu_load = strtod(((char *)apache_cpu_load), ((char **)NULL));
	}
	const char *apache_up_time = strstr(apache_status, "Uptime:");
	if(apache_up_time) {
		apache_up_time = skip_token(apache_up_time);
		//printf("%d\n", atoi(((char *)apache_up_time)));
		mapache_up_time = atoi(((char *)apache_up_time));
	}
    const char *apache_total_accesses = strstr(apache_status, "Total Accesses:");
	if(apache_total_accesses) {
		apache_total_accesses = skip_token(apache_total_accesses);
		apache_total_accesses = skip_token(apache_total_accesses);
		//printf("%d\n", atoi(((char *)apache_total_accesses)));
		mapache_total_access = atoi(((char *)apache_total_accesses));
	}
    const char *apache_total_kbytes = strstr(apache_status, "Total kBytes:");
	if(apache_total_kbytes) {
		apache_total_kbytes = skip_token(apache_total_kbytes);
		apache_total_kbytes = skip_token(apache_total_kbytes);
		//printf("%d\n", atoi(((char *)apache_total_kbytes)));
		mapache_total_kbytes = atoi(((char *)apache_total_kbytes));
	}

	return size*nmemb;
}

int libcurl_init(char *url) {
	CURLcode res;
	CURL *curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 0);
		curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,handle_data);
		res = curl_easy_perform(curl);
		//curl_easy_cleanup(curl); 
		if(res != 0) {
			printf("Error occured .. \n");
			mapache_available = 0;
		}
	}
	return res;
}

double get_apache_req_rate() {
	return mapache_req_rate;
}

double get_apache_bytes_rate() {
	return mapache_bytes_rate;
}

double get_apache_bytes_per_req() {
	return mapache_bytes_req;
}

double get_apache_cpu_load() {
	return mapache_cpu_load;
}

int get_apache_busy_workers() {
	return mapache_busy_workers;
}

int get_apache_idle_workers() {
	return mapache_idle_workers;
}

int get_apache_total_access() {
	return mapache_total_access;
}

int get_apache_up_time() {
	return mapache_up_time;
}

int get_apache_total_kbytes() {
	return mapache_total_kbytes;
}

int get_apache_availability() {
	return mapache_available;
}
/*
int main(int argc, char **argv) {
    char bstring[512];
    FILE *alog;

	do{
		libcurl_init("http://10.0.0.111/server-status?auto");
		int arr = get_apache_req_rate();
		int abr = get_apache_bytes_rate();
		int atk = get_apache_total_kbytes();
		int aiw = get_apache_idle_workers();
		int abw = get_apache_busy_workers();
		int aav = get_apache_availability();

		alog = fopen("apache.log", "a");
		if(alog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}

		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;

		sprintf(bstring, "%f \t %d \t %d \t %d \t %d \t %d \t %d \n", timestamp, arr, abr, atk, aiw, abw, aav);
		fwrite(bstring, sizeof(char), strlen(bstring), alog);
		fclose(alog);
		sleep(1);
	} while(1);*/

	/*
	libcurl_init("http://10.0.0.131/server-status?auto");
	printf("Number of Bytes Served Per Sec %f\n", get_apache_bytes_rate());
	printf("Number of KBytes Served %d \n", get_apache_total_kbytes());
	printf("Get Idle Workers %d \n", get_apache_idle_workers());
	printf("Get Busy Workers %d \n", get_apache_busy_workers());
	printf("Get Apache Uptime %d \n", get_apache_up_time());
	printf("Get Apache Availability %d \n", get_apache_availability());
	*/
	/*
	return 0;
}*/
