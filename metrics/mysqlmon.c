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
#include <mysql/mysql.h>

struct mysql_db {
	char *instance;
	char *host;
	char *user;
	char *passwd;
	char *database;
	char *socket;
	char *port;
	MYSQL *con;
	int state;
};

typedef struct mysql_db mysql_db_t;

int mmysql_req_maxp_time = 0; int mmysql_req_ptime = 0; int mmysql_req_count = 0; int mmysql_req_err_count = 0; 
int mmysql_req_bytes_sent = 0; int mmysql_req_bytes_recv = 0;
int mmysql_busy_threads = 0; int mmysql_current_threads = 0; int mmysql_max_threads = 0;

CURL *curl;

void mysql_parse_connector_requests(xmlNode *node, char *connector_name, int i, struct mysql_connect_list *mysql_connects) {
	xmlAttr *attr;
	char *attr_name = NULL;
	int reqMaxPTime = 0; int reqPTime = 0; int reqCount = 0; int reqErrorCount = 0; int bytesRecv = 0; int bytesSent = 0; 
	for(attr = node->properties; attr; attr = attr->next) {
		if(strcasecmp((char *)attr->name, "maxTime") == 0 ) {
			reqMaxPTime = attr->children == NULL ? 0 : atoi((char *)attr->children->content);
			//printf("mysql: mysql_connector %s, reqMaxTime: %d \n", (char *)type, reqMaxPTime);
			//mmysql_req_maxp_time = reqMaxPTime;
			mysql_connects->connects[i].req_maxp_time = reqMaxPTime;
			printf("mysql: mysql_connector : %s, index: %d, reqMaxTime: %d \n", (char *)connector_name, i, reqMaxPTime);
		} else if(strcasecmp((char *)attr->name, "processingTime") == 0) {
			reqPTime = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, reqPTime: %d \n", (char *)type, reqPTime);
			//mmysql_req_ptime = reqPTime;
			mysql_connects->connects[i].req_ptime = reqPTime;
			printf("mysql: mysql_connector %s, index: %d, reqPTime: %d \n", (char *)connector_name, i, reqPTime);
		} else if(strcasecmp((char *)attr->name, "requestCount") == 0) {
			reqCount = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, reqCount: %d \n", (char *)type, reqCount);
			//mmysql_req_count = reqCount;
			mysql_connects->connects[i].req_count = reqCount;
			printf("mysql: mysql_connector %s, index: %d, reqCount: %d \n", (char *)connector_name, i, reqCount);
		} else if(strcasecmp((char *)attr->name, "errorCount") == 0) {
			reqErrorCount = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, reqErrorCount: %d \n", (char *)type, reqErrorCount);
			//mmysql_req_err_count = reqErrorCount;
			mysql_connects->connects[i].req_err_count = reqErrorCount;
			printf("mysql: mysql_connector %s, index: %d, reqErrorCount: %d \n", (char *)connector_name, i, reqErrorCount);
		} else if(strcasecmp((char *)attr->name, "bytesReceived") == 0) {
			bytesRecv = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, bytesRecv: %d \n", (char *)type, bytesRecv);
			//mmysql_bytes_recv = bytesRecv;
			mysql_connects->connects[i].req_bytes_recv = bytesRecv;
			printf("mysql: mysql_connector %s, index: %d, bytesRecv: %d \n", (char *)connector_name, i, bytesRecv);
		} else if(strcasecmp((char *)attr->name, "bytesSent") == 0) {
			bytesRecv = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, bytesSent: %d \n", (char *)type, bytesSent);
			//mmysql_bytes_sent = bytesSent;
			mysql_connects->connects[i].req_bytes_sent = bytesSent;
			printf("mysql: mysql_connector %s, index: %d, bytesSent: %d \n", (char *)connector_name, i, bytesSent);
		}
	}
}

void mysql_parse_connector_threads(xmlNode *node, char *connector_name, int i, struct mysql_connect_list *mysql_connects) {
	xmlAttr *attr;
	char *attr_name = NULL;
	int max = 0; int current = 0; int busy = 0;
	for(attr = node->properties; attr; attr = attr->next) {
		if(strcasecmp((char *)attr->name, "maxThreads") == 0 ) {
			max = attr->children == NULL ? 0 : atoi((char *)attr->children->content);
			//printf("mysql: mysql_connector %s, maxThreads: %d \n", (char *)type, max);
			//mmysql_max_threads = max;
			mysql_connects->connects[i].threads_max = max;
		} else if(strcasecmp((char *)attr->name, "currentThreadsBusy") == 0) {
			busy = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, currentThreadsBusy: %d \n", (char *)type, busy);
			//mmysql_busy_threads = busy;
			mysql_connects->connects[i].threads_busy = busy;
		} else if(strcasecmp((char *)attr->name, "currentThreadCount") == 0) {
			current = attr->children == NULL ? 0 : atoi((char*)attr->children->content);
			//printf("mysql: mysql_connector %s, currentThreadCount: %d \n", (char *)type, current);
			//mmysql_current_threads = current;
			mysql_connects->connects[i].threads_current = current;
		}
	}
}

void mysql_parse_connector(xmlNode *node, int i, struct mysql_connect_list *mysql_connects) {
	xmlNode *cur_node = NULL;
	xmlAttr *attr;
	char *connector_name = NULL;

    for(attr = node->properties; attr; attr = attr->next)
		if(strcasecmp((char *)attr->name, "name") == 0)
			connector_name = attr->children == NULL ? NULL : (char *)attr->children->content;			
	
	if(connector_name) {
		//printf("mysql: mysql_connector, name: %s \n ", (char *)connector_name);
		if(!mysql_connects->connects[i].connector_name)
			mysql_connects->connects[i].connector_name = strdup(connector_name);
			printf("mysql: mysql_connector, index: %d, name: %s \n ", i, (char *)connector_name);
		for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				printf("mysql: mysql_connector children, name: %s \n", (char *)cur_node->name);
				if(strcasecmp((char *)cur_node->name, "threadInfo") == 0)
					mysql_parse_connector_threads(cur_node, connector_name, i, mysql_connects);
				else if(strcasecmp((char *)cur_node->name, "requestInfo") == 0)
					mysql_parse_connector_requests(cur_node, connector_name, i, mysql_connects);
			}
		}
	}
}

void mysql_parse_xml(xmlNode *a_node, int i, struct mysql_connect_list *mysql_connects) {
	xmlNode *cur_node = NULL;
	int q = -1;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		printf("mysql: node, name: %s \n", (char *)cur_node->name);
		if(strcasecmp((char *)cur_node->name, "connector") == 0) {
			//printf("mysql: node, name: %s \n", (char *)cur_node->name);
			q = q + 1;
			mysql_parse_connector(cur_node, q, mysql_connects);
		}
		i = i + 1;
		mysql_parse_xml(a_node->children, i, mysql_connects);
	}
}

size_t handle_data( void *ptr, size_t size, size_t nmemb, void *stream) {
	//printf("Size of data returned %ld \n", size*nmemb);
	//char lastchar = *((char *) ptr + size*nmemb - 1);
	//*((char *) ptr + size*nmemb - 1) = '\0';
	char mysql_status[8196];
	strncpy(mysql_status, (char *)ptr, 8196);

	//printf(" XML %s\n", mysql_status);

	int index = 0;
	xmlDocPtr doc;
	xmlNode *root_element = NULL;
	//doc = xmlParseMemory(mysql_status, 8196);
	if(strstr(mysql_status, "<?xml version=\"1.0\" encoding=\"utf-8\"?>") != NULL) {
		//printf(" XML %s\n", mysql_status);
		doc = xmlReadMemory(mysql_status, 8196, NULL, NULL, XML_PARSE_NOERROR);
		root_element = xmlDocGetRootElement(doc);

		mysql_connects = (struct mysql_connect_list *) calloc(1, sizeof(struct mysql_connect_list));
		mysql_connects->num_connect = 2;
		mysql_connects->connects = (struct mysql_connect *) calloc(2, sizeof(struct mysql_connect));

		mysql_parse_xml(root_element, index, mysql_connects);
		xmlFreeDoc(doc);
	}
	return size*nmemb;
}

int libcurl_init(char *url, char *user, char *password) {
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
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,handle_data);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		res = curl_easy_perform(curl);
		//curl_easy_cleanup(curl); 
		if(res != 0) {
			printf("Error occured .. \n");
		}
	}
	return res;
}

void libcurl_close(CURL *curl){
	curl_easy_cleanup(curl); 
}

int get_mysql_req_maxp_time(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_maxp_time = mysql_connects->connects[i].req_maxp_time;
				break;
			}
		}
	}
	return mmysql_req_maxp_time;
}

int get_mysql_req_ptime(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_ptime = mysql_connects->connects[i].req_ptime;
				break;
			}
		}
	}
	return mmysql_req_ptime;
}

int get_mysql_req_count(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_count = mysql_connects->connects[i].req_count;
				break;
			}
		}
	}
	return mmysql_req_count;
}

int get_mysql_req_err_count(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_err_count = mysql_connects->connects[i].req_err_count;
				break;
			}
		}
	}
	return mmysql_req_err_count;
}

int get_mysql_req_bytes_sent(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_bytes_sent = mysql_connects->connects[i].req_bytes_sent;
				break;
			}
		}
	}
	return mmysql_req_bytes_sent;
}

int get_mysql_req_bytes_recv(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_req_bytes_recv = mysql_connects->connects[i].req_bytes_recv;
				break;
			}
		}
	}
	return mmysql_req_bytes_recv;
}

int get_mysql_busy_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_busy_threads = mysql_connects->connects[i].threads_busy;
				break;
			}
		}
	}
	return mmysql_busy_threads;
}

int get_mysql_current_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_current_threads = mysql_connects->connects[i].threads_current;
				break;
			}
		}
	}
	return mmysql_current_threads;
}

int get_mysql_max_threads(char *connector_name) {
	int i;
	for(i = 0; i < 2; i++) {
		if(mysql_connects->connects[i].connector_name != NULL) {
			if(strcmp(mysql_connects->connects[i].connector_name, connector_name) == 0) {
				mmysql_max_threads = mysql_connects->connects[i].threads_max;
				break;
			}
		}
	}
	return mmysql_max_threads;
}

void mysql_read() {

	mysql_db_t *db;
	MYSQL *con;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *query;
	int field_num;

	mysql_init()
}

int main(int argc, char **argv) {
    char bstring[512];
    FILE *alog;

	do{
		libcurl_init("http://imax:imax@10.0.0.112:8080/manager/status?XML=true", "imax", "imax");
		//int trmpt = 0; int trpt = 0; int trc = 0; int trec = 0; int trbs = 0; int trbr = 0;
		//int tmt = 0; int tct = 0; int tbt = 0;

		int trmpt = get_mysql_req_maxp_time("jk-8009");
		int trpt = get_mysql_req_ptime("jk-8009");
		int trc = get_mysql_req_count("jk-8009");
		int trec = get_mysql_req_err_count("jk-8009");
		int trbs = get_mysql_req_bytes_sent("jk-8009");
		int trbr = get_mysql_req_bytes_recv("jk-8009");

		int tmt = get_mysql_max_threads("jk-8009");
		int tct = get_mysql_current_threads("jk-8009");
		int tbt = get_mysql_busy_threads("jk-8009");
		
		/*
		alog = fopen("mysql.log", "a");
		if(alog == NULL){
			printf("Cannot open file.\n");
			return 1;
		}

		struct timeval now;
		gettimeofday(&now, NULL);
		double timestamp = now.tv_sec * 1.0e6 + now.tv_usec;

		sprintf(bstring, "%f \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n", timestamp, trmpt, trpt, trc, trec, trbs, trbr, tmt, tct, tbt);
		fwrite(bstring, sizeof(char), strlen(bstring), alog);
		fclose(alog);
		*/
		printf("Requests Max Processing Time %d \n", trmpt);
		printf("Requests Processing Time %d \n", trpt);
		printf("Number of Requests %d \n", trc);
		printf("Requests Error Count %d \n", trec);
		printf("Requests Bytes Sent %d \n", trbs);
		printf("Requests Bytes Recv %d \n", trbr);

		printf("Max Threads Count %d \n", tmt);
		printf("Current Threads Count %d \n", tct);
		printf("Busy Threads Count %d \n", tbt);

		sleep(1);
	} while(1);

	return 0;
}
