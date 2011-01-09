#gcc -g -Wall -o tomcatmon tomcatmon.c `pkg-config --cflags --libs libcurl` `xml2-config --cflags --libs`
gcc -g -Wall -o tomcatmon tomcatmon.c `curl-config --cflags --libs` `xml2-config --cflags --libs`
