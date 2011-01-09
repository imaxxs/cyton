include_HEADERS =
noinst_HEADERS =  
INCLUDES = -I /usr/include/libxml2 -I /usr/include/curl -I /usr/include/libvirt 
LIBS = -L/usr/lib64/ -lvirt -L/usr/lib64/ -lcurl
SUBDIRS =  

#lib_LTLIBRARIES = libmetrics.la
noinst_LTLIBRARIES = libmetrics.la

#noinst_PROGRAMS = metrics 
#noinst_bindir = .

AM_CXXFLAGS = -g
libmetrics_la_SOURCES = libvirt.c apachemon.c tomcatmon.c
#libmetrics_la_SOURCES = apachemon.c tomcatmon.c

#metrics_LDADD = -lvirt 
#libmetrics_la_LIBADD = bootstrap/libbootstrap.la functions/libfunctions.la node/libnode.la
