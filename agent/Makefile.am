include_HEADERS = 
noinst_HEADERS = 

SUBDIRS =  
#LIBS = -L/opt/euca-libvirt/lib/ 

#lib_LTLIBRARIES = libnodeagent.la
#noinst_LTLIBRARIES = libnodeagent.la

noinst_PROGRAMS = nodeagent
#noinst_bindir = .

AM_CXXFLAGS = -g
#prog_CFLAGS = $(AM_CFLAGS)
nodeagent_SOURCES = nodeagent.cpp nodeagent.h 

nodeagent_LDADD = ../util/libutil.la ../context/libcontext.la ../formats/libformats.la ../graph/libgraph.la ../metrics/libmetrics.la ../monitoring/libmonitoring.la ../planner/libplanner.la -levpath 
