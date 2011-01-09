#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <ctype.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

struct tomcat_connect {
	char *connector_name;
	int req_maxp_time;
	int req_ptime;
	int req_count;
	int req_err_count;
	int req_bytes_sent;
	int req_bytes_recv;
	int threads_busy;
	int threads_current;
	int threads_max;
};

struct tomcat_connect_list {
	int num_connect;
	struct tomcat_connect *connects;
};

struct tomcat_connect_list *tomcat_connects;

int mtomcat_req_maxp_time = 0; int mtomcat_req_ptime = 0; int mtomcat_req_count = 0; int mtomcat_req_err_count = 0; 
int mtomcat_req_bytes_sent = 0; int mtomcat_req_bytes_recv = 0;
int mtomcat_busy_threads = 0; int mtomcat_current_threads = 0; int mtomcat_max_threads = 0;
int mtomcat_available = 0;

CURL *curl;

void tomcat_parse_connector_requests(xmlNode *node, char *connector_name, int i, struct tomcat_connect_list *tomcat_connects) {
	xmlAttr *attr;
	char *attr_name = NULL;
	int reqMaxPTime = 0; int reqPTime = 0; int reqCount = 0; int reqErrorCount = 0; int bytesRecv = 0; int bytesSent = 0; 
	for(attr = node->properties; attr; attr = attr->next) {
		if(strcasecmp((char *)attr->name, "maxTime") == 0 ) {
			reqMaxPTime = attr->children == NULL ? 0 : atoi((char *)attr->children->content);
			//printf("tomcat: tomcat_connector %s, reqMaxTime: %d \n", (char *)type, reqMaxPTime);
			//mtomcat_req_maxp_time = reqMaxPTime;
			tomcat_connects->connects[i].req_maxp_time = reqMaxPTime;
			//printf("tomcat: tomcat_connector : %s, index: %d, reqMaxTime: %d \n", (char *)connector_name, i, reqMaxPTime);
		} else if(strcasecmp((char *)attr->name, "processingTime") == 0) {
			reqPTime = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, reqPTime: %d \n", (char *)type, reqPTime);
			//mtomcat_req_ptime = reqPTime;
			tomcat_connects->connects[i].req_ptime = reqPTime;
			//printf("tomcat: tomcat_connector %s, index: %d, reqPTime: %d \n", (char *)connector_name, i, reqPTime);
		} else if(strcasecmp((char *)attr->name, "requestCount") == 0) {
			reqCount = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, reqCount: %d \n", (char *)type, reqCount);
			//mtomcat_req_count = reqCount;
			tomcat_connects->connects[i].req_count = reqCount;
			//printf("tomcat: tomcat_connector %s, index: %d, reqCount: %d \n", (char *)connector_name, i, reqCount);
		} else if(strcasecmp((char *)attr->name, "errorCount") == 0) {
			reqErrorCount = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, reqErrorCount: %d \n", (char *)type, reqErrorCount);
			//mtomcat_req_err_count = reqErrorCount;
			tomcat_connects->connects[i].req_err_count = reqErrorCount;
			//printf("tomcat: tomcat_connector %s, index: %d, reqErrorCount: %d \n", (char *)connector_name, i, reqErrorCount);
		} else if(strcasecmp((char *)attr->name, "bytesReceived") == 0) {
			bytesRecv = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, bytesRecv: %d \n", (char *)type, bytesRecv);
			//mtomcat_bytes_recv = bytesRecv;
			tomcat_connects->connects[i].req_bytes_recv = bytesRecv;
			//printf("tomcat: tomcat_connector %s, index: %d, bytesRecv: %d \n", (char *)connector_name, i, bytesRecv);
		} else if(strcasecmp((char *)attr->name, "bytesSent") == 0) {
			bytesRecv = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, bytesSent: %d \n", (char *)type, bytesSent);
			//mtomcat_bytes_sent = bytesSent;
			tomcat_connects->connects[i].req_bytes_sent = bytesSent;
			//printf("tomcat: tomcat_connector %s, index: %d, bytesSent: %d \n", (char *)connector_name, i, bytesSent);
		}
	}
}

void tomcat_parse_connector_threads(xmlNode *node, char *connector_name, int i, struct tomcat_connect_list *tomcat_connects) {
	xmlAttr *attr;
	char *attr_name = NULL;
	int max = 0; int current = 0; int busy = 0;
	for(attr = node->properties; attr; attr = attr->next) {
		if(strcasecmp((char *)attr->name, "maxThreads") == 0 ) {
			max = attr->children == NULL ? 0 : atoi((char *)attr->children->content);
			//printf("tomcat: tomcat_connector %s, maxThreads: %d \n", (char *)type, max);
			//mtomcat_max_threads = max;
			tomcat_connects->connects[i].threads_max = max;
		} else if(strcasecmp((char *)attr->name, "currentThreadsBusy") == 0) {
			busy = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, currentThreadsBusy: %d \n", (char *)type, busy);
			//mtomcat_busy_threads = busy;
			tomcat_connects->connects[i].threads_busy = busy;
		} else if(strcasecmp((char *)attr->name, "currentThreadCount") == 0) {
			current = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("tomcat: tomcat_connector %s, currentThreadCount: %d \n", (char *)type, current);
			//mtomcat_current_threads = current;
			tomcat_connects->connects[i].threads_current = current;
		}
	}
}

void tomcat_parse_connector(xmlNode *node, int i, struct tomcat_connect_list *tomcat_connects) {
	xmlNode *cur_node = NULL;
	xmlAttr *attr;
	char *connector_name = NULL;

    for(attr = node->properties; attr; attr = attr->next)
		if(strcasecmp((char *)attr->name, "name") == 0)
			connector_name = attr->children == NULL ? NULL : (char *)attr->children->content;			
	
	if(connector_name) {
		//printf("tomcat: tomcat_connector, name: %s \n ", (char *)connector_name);
		if(!tomcat_connects->connects[i].connector_name)
			tomcat_connects->connects[i].connector_name = strdup(connector_name);
			//printf("tomcat: tomcat_connector, index: %d, name: %s \n ", i, (char *)connector_name);
		for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				//printf("tomcat: tomcat_connector children, name: %s \n", (char *)cur_node->name);
				if(strcasecmp((char *)cur_node->name, "threadInfo") == 0)
					tomcat_parse_connector_threads(cur_node, connector_name, i, tomcat_connects);
				else if(strcasecmp((char *)cur_node->name, "requestInfo") == 0)
					tomcat_parse_connector_requests(cur_node, connector_name, i, tomcat_connects);
			}
		}
	}
}

void tomcat_parse_xml(xmlNode *a_node, int i, struct tomcat_connect_list *tomcat_connects) {
	xmlNode *cur_node = NULL;
	int q = -1;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		//printf("tomcat: node, name: %s \n", (char *)cur_node->name);
		if(strcasecmp((char *)cur_node->name, "connector") == 0) {
			//printf("tomcat: node, name: %s \n", (char *)cur_node->name);
			q = q + 1;
			tomcat_parse_connector(cur_node, q, tomcat_connects);
		}
		i = i + 1;
		tomcat_parse_xml(a_node->children, i, tomcat_connects);
	}
}

size_t handle_tomcat_data( void *ptr, size_t size, size_t nmemb, void *stream) {
	printf("Size of data returned %ld \n", size*nmemb);
	//char lastchar = *((char *) ptr + size*nmemb - 1);
	//*((char *) ptr + size*nmemb - 1) = '\0';
	char tomcat_status[8196];
	strncpy(tomcat_status, (char *)ptr, 8196);

	//printf(" XML %s\n", tomcat_status);
	if((size*nmemb) != 0) {
		mtomcat_available = 1;
	}

	int index = 0;
	xmlDocPtr doc;
	xmlNode *root_element = NULL;
	//doc = xmlParseMemory(tomcat_status, 8196);
	if(strstr(tomcat_status, "<?xml version=\"1.0\" encoding=\"utf-8\"?>") != NULL) {
		//printf(" XML %s\n", tomcat_status);
		doc = xmlReadMemory(tomcat_status, 8196, NULL, NULL, XML_PARSE_NOERROR);
		root_element = xmlDocGetRootElement(doc);

		tomcat_connects = (struct tomcat_connect_list *) calloc(1, sizeof(struct tomcat_connect_list));
		tomcat_connects->num_connect = 2;
		tomcat_connects->connects = (struct tomcat_connect *) calloc(2, sizeof(struct tomcat_connect));

		tomcat_parse_xml(root_element, index, tomcat_connects);
		xmlFreeDoc(doc);
	}
	return size*nmemb;
}

int libtomcat_start(char *url, char *user, char *password) {
	CURLcode res;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_USERPWD, "imax:imax");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		//curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
		curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,handle_tomcat_data);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		res = curl_easy_perform(curl);
		//curl_easy_cleanup(curl); 
		if(res != 0) {
			printf("Error occured .. \n");
			mtomcat_available = 0;
		}
	}
	return res;
}

void libcurl_close(CURL *curl){
	curl_easy_cleanup(curl); 
}

int get_tomcat_req_maxp_time(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_maxp_time = tomcat_connects->connects[i].req_maxp_time;
					break;
				}
			}
		}
	}
	return mtomcat_req_maxp_time;
}

int get_tomcat_req_ptime(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_ptime = tomcat_connects->connects[i].req_ptime;
					break;
				}
			}
		}
	}
	return mtomcat_req_ptime;
}

int get_tomcat_req_count(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_count = tomcat_connects->connects[i].req_count;
					break;
				}
			}
		}
	}
	return mtomcat_req_count;
}

int get_tomcat_req_err_count(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_err_count = tomcat_connects->connects[i].req_err_count;
					break;
				}
			}
		}
	}
	return mtomcat_req_err_count;
}

int get_tomcat_req_bytes_sent(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_bytes_sent = tomcat_connects->connects[i].req_bytes_sent;
					break;
				}
			}
		}
	}
	return mtomcat_req_bytes_sent;
}

int get_tomcat_req_bytes_recv(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_req_bytes_recv = tomcat_connects->connects[i].req_bytes_recv;
					break;
				}
			}
		}
	}
	return mtomcat_req_bytes_recv;
}

int get_tomcat_busy_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_busy_threads = tomcat_connects->connects[i].threads_busy;
					break;
				}
			}
		}
	}
	return mtomcat_busy_threads;
}

int get_tomcat_current_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_current_threads = tomcat_connects->connects[i].threads_current;
					break;
				}
			}
		}
	}
	return mtomcat_current_threads;
}

int get_tomcat_max_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(tomcat_connects != NULL) {
			if(tomcat_connects->connects[i].connector_name != NULL) {
				if(strcmp(tomcat_connects->connects[i].connector_name, connector_name) == 0) {
					mtomcat_max_threads = tomcat_connects->connects[i].threads_max;
					break;
				}
			}
		}
	}
	return mtomcat_max_threads;
}

int get_tomcat_availability() {
	return mtomcat_available;
}

/*
int main(int argc, char **argv) {
    char bstring[512];
    FILE *alog;

	do{
		libtomcat_start("http://imax:imax@10.0.0.112:8080/manager/status?XML=true", "imax", "imax");
		//int trmpt = 0; int trpt = 0; int trc = 0; int trec = 0; int trbs = 0; int trbr = 0;
		//int tmt = 0; int tct = 0; int tbt = 0;

		int trmpt = get_tomcat_req_maxp_time("jk-8009");
		int trpt = get_tomcat_req_ptime("jk-8009");
		int trc = get_tomcat_req_count("jk-8009");
		int trec = get_tomcat_req_err_count("jk-8009");
		int trbs = get_tomcat_req_bytes_sent("jk-8009");
		int trbr = get_tomcat_req_bytes_recv("jk-8009");

		int tmt = get_tomcat_max_threads("jk-8009");
		int tct = get_tomcat_current_threads("jk-8009");
		int tbt = get_tomcat_busy_threads("jk-8009");
		int tav = get_tomcat_availability();	
		
		alog = fopen("tomcat.log", "a");
		if(alog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}

		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;

		sprintf(bstring, "%f \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n", timestamp, trmpt, trpt, trc, trec, trbs, trbr, 
		tmt, tct, tbt, tav);
		fwrite(bstring, sizeof(char), strlen(bstring), alog);
		fclose(alog);*/

		/*
		printf("Requests Max Processing Time %d \n", trmpt);
		printf("Requests Processing Time %d \n", trpt);
		printf("Number of Requests %d \n", trc);
		printf("Requests Error Count %d \n", trec);
		printf("Requests Bytes Sent %d \n", trbs);
		printf("Requests Bytes Recv %d \n", trbr);

		printf("Max Threads Count %d \n", tmt);
		printf("Current Threads Count %d \n", tct);
		printf("Busy Threads Count %d \n", tbt);
		printf("Tomcat availability %d \n", tav);*/
		/*
		sleep(1);
	} while(1);

	return 0;
}*/
