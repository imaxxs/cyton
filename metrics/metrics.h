#ifndef METRICS_H
#define METRICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef SAMPLEBUFFERSIZE
#define SAMPLEBUFFERSIZE 8192 
#endif

#define NUM_CPUSTATES_24X 4
#define NUM_CPUSTATES_26X 7
static unsigned int num_cpustates;

struct stat struct_stat;
#define CPU_FREQ_SCALING_MAX_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define CPU_FREQ_SCALING_MIN_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define CPU_FREQ_SCALING_CUR_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"
#define CPU_FREQ_SCALING_AVAILABLE_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"

#define CPU_FREQ_SCALING_GOVERNOR "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define CPU_FREQ_SCALING_AVAILABLE_GOVERNORS "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors"

#define CPU_FREQ_AFFECTED_CPU "/sys/devices/system/cpu/cpu0/cpufreq/affected_cpus"

#define CPU_FREQ_ONDEMAND_SAMPLING_RATE_MIN "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/sampling_rate_min"
#define CPU_FREQ_ONDEMAND_SAMPLING_RATE_MAX "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/sampling_rate_max"
#define CPU_FREQ_ONDEMAND_SAMPLING_RATE "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/sampling_rate"
#define CPU_FREQ_ONDEMAND_UP_THRESHOLD "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/up_threshold"
#define CPU_FREQ_ONDEMAND_IGNORE_NICE_LOAD "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/ignore_nice_load"
#define CPU_FREQ_ONDEMAND_POWERSAVE_BIAD "/sys/devices/system/cpu/cpu0/cpufreq/ondemand/powersave_bias"

char sys_devices_system_cpu[32];
char sys_devices_system_cpu_available[128];

typedef struct sensor_slurp {
	char *name;
	char buffer[SAMPLEBUFFERSIZE];
} sensor_slurp, *sensor_slurp_ptr;

typedef struct sensor_rec {
	double timestamp;
	char *attr_name;
	float rec_val;
} sensor_rec, *sensor_rec_ptr;

sensor_slurp proc_stat    = { "/proc/stat" };
sensor_slurp proc_loadavg = { "/proc/loadavg" };
sensor_slurp proc_meminfo = { "/proc/meminfo" };
sensor_slurp proc_net_dev = { "/proc/net/dev" };

#endif
