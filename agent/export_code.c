#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "fm.h"
#include "cod.h"
#include "evpath.h"
#include "revpath.h"
#include "gen_thread.h"

extern int libvirt_init();
extern int libvirt_open();

CManager cm;
//static char extern_string;
//static cod_extern_entry externs;

void export_dynamic_code(CManager cm, char *export_api, cod_extern_entry externs) {
	/*int i; int j; char *export_me;
	strcpy(extern_string, export_api);
	for(i = 0; i < 1; i++) {
		printf("Export Name : %s ", export_name);
		strcat(export_me, export_name);
		strcat(export_me, ",");
		strcat(export_me, "(void *) 0");
		printf("Export Me : %s ", export_me);
		externs[j].extern_name = export_me;
	}
	for(j = 0; j < 1; j++) {
		externs[j].extern_value = (void *) (long) export_name;
	}*/
	EVadd_standard_routines(cm, export_api, externs);
}

int main(int argc, char *argv[]) {
    attr_list node_contact_list = NULL;
    gen_pthread_init();
    cm = CManager_create();

    attr_list listen_list = NULL;
	char *node_cm_contact_list;
    CMlisten_specific(cm, listen_list);
    node_contact_list = CMget_contact_list(cm);
    if (node_contact_list) {
		node_cm_contact_list = attr_list_to_string(node_contact_list);
		free_attr_list(node_contact_list);
    }

	printf("Local node CM contact list \"%s\"\n", node_cm_contact_list);

	/*
    static char extern_string[] = "\
		void libvirt_init();\n\
		void libvirt_open();\n\
		void libvirt_close();\n\
		int libvirt_num_domains();\n\
		int libvirt_num_active_domains();\n\
		int libvirt_num_inactive_domains();\n\
		void *libvirt_get_domain_info(char *domain_name);\n\
		void *libvirt_get_domain_disk_info(char *domain_name);\n\
		void *libvirt_get_domain_interface_info(char *domain_name);\n\
		int libvirt_domain_exists(char *domain_name);\n\
		int libvirt_get_domain_id(char *domain_name);\n\
		unsigned char libvirt_get_domain_state(char *domain_name);\n\
		unsigned long libvirt_get_domain_mem_max(char *domain_name);\n\
		unsigned long libvirt_get_domain_mem_used(char *domain_name);\n\
		unsigned short libvirt_get_domain_num_vcpus(char *domain_name);\n\
		unsigned long libvirt_get_domain_cpu_total_time(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_rreq(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_wreq(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_rbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_vblock_wbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txbytes(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxpackets(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txpackets(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxerrors(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txerrors(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_rxdrops(char *domain_name);\n\
		unsigned long libvirt_get_domain_if_txdrops(char *domain_name);\n\
		int libvirt_set_vcpus(char *domain_name, int num_vcpus);\n\
		int libvirt_set_memory(char *domain_name, unsigned long mem_kbs);\n\
		int libvirt_create_domain(char *xmlDesc);\n\
		int libvirt_shutdown_domain(char *domain_name);\n\
		int get_apache_busy_workers();\n\
		int get_apache_idle_workers();\n\
		double get_apache_req_rate();\n\
		double get_apache_bytes_rate();\n\
		int get_apache_availability();\n\
		int get_tomcat_req_maxp_time(char *connector_name);\n\
		int get_tomcat_req_ptime(char *connector_name);\n\
		int get_tomcat_req_count(char *connector_name);\n\
		int get_tomcat_req_err_count(char *connector_name);\n\
		int get_tomcat_req_bytes_sent(char *connector_name);\n\
		int get_tomcat_req_bytes_recv(char *connector_name);\n\
		int get_tomcat_max_threads(char *connector_name);\n\
		int get_tomcat_current_threads(char *connector_name);\n\
		int get_tomcat_busy_threads(char *connector_name);\n\
		int get_tomcat_availability();\n\
		int system(char *command);\n";
	*/
	/*
    static cod_extern_entry externs[] = {
	{"libvirt_init", (void *) 0},						// 0
	{"libvirt_open", (void *) 0},						// 1
	{"libvirt_close", (void *) 0},						// 2
	{"libvirt_num_domains", (void *) 0},				// 3
	{"libvirt_num_active_domains", (void *) 0},			// 4
	{"libvirt_num_inactive_domains", (void *) 0},		// 5
	{"libvirt_get_domain_info", (void *) 0},			// 6
	{"libvirt_get_domain_disk_info", (void *) 0},		// 7	
	{"libvirt_get_domain_interface_info", (void *) 0},  // 8
	{"libvirt_domain_exists", (void *) 0}, 				// 9
	{"libvirt_get_domain_id", (void *) 0},				// 10
	{"libvirt_get_domain_state", (void *) 0}, 			// 11
	{"libvirt_get_domain_mem_max", (void *) 0}, 		// 12
	{"libvirt_get_domain_mem_used", (void *) 0}, 		// 13
	{"libvirt_get_domain_num_vcpus", (void *) 0},		// 14
	{"libvirt_get_domain_cpu_total_time", (void *) 0},	// 15
	{"libvirt_get_domain_vblock_rreq", (void *) 0},		// 16
	{"libvirt_get_domain_vblock_wreq", (void *) 0},		// 17
	{"libvirt_get_domain_vblock_rbytes", (void *) 0},	// 18
	{"libvirt_get_domain_vblock_wbytes", (void *) 0},	// 19
	{"libvirt_get_domain_if_rxbytes", (void *) 0},		// 20
	{"libvirt_get_domain_if_txbytes", (void *) 0},		// 21
	{"libvirt_get_domain_if_rxpackets", (void *) 0},	// 22
	{"libvirt_get_domain_if_txpackets", (void *) 0},	// 23
	{"libvirt_get_domain_if_rxerrors", (void *) 0}, 	// 24
	{"libvirt_get_domain_if_txerrors", (void *) 0},		// 25
	{"libvirt_get_domain_if_rxdrops", (void *) 0}, 		// 26
	{"libvirt_get_domain_if_txdrops", (void *) 0},		// 27
	{"libvirt_set_vcpus", (void *) 0},					// 28
	{"libvirt_set_memory", (void *) 0},					// 29
	{"libvirt_create_domain", (void *) 0},				// 30
	{"libvirt_shutdown_domain", (void *)0},				// 31
	{"get_apache_busy_workers", (void *) 0},			// 32
	{"get_apache_idle_workers", (void *) 0},			// 33
	{"get_apache_req_rate", (void *) 0},				// 34
	{"get_apache_bytes_rate", (void *) 0},				// 35
	{"get_tomcat_req_maxp_time", (void *) 0},			// 36
	{"get_tomcat_req_ptime", (void *) 0},				// 37
	{"get_tomcat_req_count", (void *) 0},				// 38
	{"get_tomcat_req_err_count", (void *) 0},			// 39
	{"get_tomcat_req_bytes_sent", (void *) 0},			// 40
	{"get_tomcat_req_bytes_recv", (void *) 0},			// 41
	{"get_tomcat_max_threads", (void *) 0},				// 42
	{"get_tomcat_current_threads", (void *) 0},			// 43
	{"get_tomcat_busy_threads", (void *) 0},			// 44
	{"get_apache_availability", (void *) 0},			// 45
	{"get_tomcat_availability", (void *) 0},			// 46
	{"system", (void *) 0},								// 47
	{(char *) 0, (void *) 0}
    };*/

    /*
     * some compilers think it isn't a static initialization to put this
     * in the structure above, so do it explicitly.
     */
	/*	 
    externs[0].extern_value = (void *) (long) libvirt_init;
    externs[1].extern_value = (void *) (long) libvirt_open;
    externs[2].extern_value = (void *) (long) libvirt_close;
    externs[3].extern_value = (void *) (long) libvirt_num_domains;
    externs[4].extern_value = (void *) (long) libvirt_num_active_domains;
    externs[5].extern_value = (void *) (long) libvirt_num_inactive_domains;
    externs[6].extern_value = (void *) (long) libvirt_get_domain_info;
    externs[7].extern_value = (void *) (long) libvirt_get_domain_disk_info;
    externs[8].extern_value = (void *) (long) libvirt_get_domain_interface_info;
    externs[9].extern_value = (void *) (long) libvirt_domain_exists;
    externs[10].extern_value = (void *) (long) libvirt_get_domain_id;
    externs[11].extern_value = (void *) (long) libvirt_get_domain_state;
    externs[12].extern_value = (void *) (long) libvirt_get_domain_mem_max;
    externs[13].extern_value = (void *) (long) libvirt_get_domain_mem_used;
    externs[14].extern_value = (void *) (long) libvirt_get_domain_num_vcpus;
    externs[15].extern_value = (void *) (long) libvirt_get_domain_cpu_total_time;
    externs[16].extern_value = (void *) (long) libvirt_get_domain_vblock_rreq;
    externs[17].extern_value = (void *) (long) libvirt_get_domain_vblock_wreq;
    externs[18].extern_value = (void *) (long) libvirt_get_domain_vblock_rbytes;
    externs[19].extern_value = (void *) (long) libvirt_get_domain_vblock_wbytes;
    externs[20].extern_value = (void *) (long) libvirt_get_domain_if_rxbytes;
    externs[21].extern_value = (void *) (long) libvirt_get_domain_if_txbytes;
    externs[22].extern_value = (void *) (long) libvirt_get_domain_if_rxpackets;
    externs[23].extern_value = (void *) (long) libvirt_get_domain_if_txpackets;
    externs[24].extern_value = (void *) (long) libvirt_get_domain_if_rxerrors;
    externs[25].extern_value = (void *) (long) libvirt_get_domain_if_txerrors;
    externs[26].extern_value = (void *) (long) libvirt_get_domain_if_rxdrops;
    externs[27].extern_value = (void *) (long) libvirt_get_domain_if_txdrops;
    externs[28].extern_value = (void *) (long) libvirt_set_vcpus;
    externs[29].extern_value = (void *) (long) libvirt_set_memory;
    externs[30].extern_value = (void *) (long) libvirt_create_domain;
    externs[31].extern_value = (void *) (long) libvirt_shutdown_domain;
    externs[32].extern_value = (void *) (long) get_apache_busy_workers;
    externs[33].extern_value = (void *) (long) get_apache_idle_workers;
    externs[34].extern_value = (void *) (long) get_apache_req_rate;
    externs[35].extern_value = (void *) (long) get_apache_bytes_rate;
    externs[36].extern_value = (void *) (long) get_tomcat_req_maxp_time;
    externs[37].extern_value = (void *) (long) get_tomcat_req_ptime;
    externs[38].extern_value = (void *) (long) get_tomcat_req_count;
    externs[39].extern_value = (void *) (long) get_tomcat_req_err_count;
    externs[40].extern_value = (void *) (long) get_tomcat_req_bytes_sent;
    externs[41].extern_value = (void *) (long) get_tomcat_req_bytes_recv;
    externs[42].extern_value = (void *) (long) get_tomcat_max_threads;
    externs[43].extern_value = (void *) (long) get_tomcat_current_threads;
    externs[44].extern_value = (void *) (long) get_tomcat_busy_threads;
    externs[45].extern_value = (void *) (long) get_apache_availability;
    externs[46].extern_value = (void *) (long) get_tomcat_availability;
    externs[47].extern_value = (void *) (long) system;
	*/

    //static char export_name[] = {"libvirt_init"};
    static char extern_string[] = "\
		void libvirt_init();\n\
		void libvirt_open();\n";

    static cod_extern_entry externs[] = {
	{"libvirt_init", (void *) 0},						// 0
	{"libvirt_open", (void *) 0},						// 1
	{(char *) 0, (void *) 0}
    };

    externs[0].extern_value = (void *) (long) libvirt_init;
    externs[1].extern_value = (void *) (long) libvirt_open;

	//EVadd_standard_routines(cm, extern_string, externs);
	export_dynamic_code(cm, extern_string, externs); 

	CMsleep(cm, 1);
	CMrun_network(cm);
	return 0;
}
