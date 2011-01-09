#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <fcntl.h>
#include <inttypes.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
