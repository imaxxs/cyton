#gcc -Wall -o apachemon apachemon.c `pkg-config --cflags --libs libcurl`
#gcc -g -Wall -o tomcatmon tomcatmon.c `curl-config --cflags --libs` `xml2-config --cflags --libs`
gcc -Wall -o apachemon apachemon.c `curl-config --cflags --libs`
