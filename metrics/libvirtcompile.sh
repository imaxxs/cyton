gcc -o libvirt libvirt.c `xml2-config --cflags --libs` -I /usr/include/libxml2 -I /usr/include/curl -I /usr/include/libvirt -L/usr/lib64/ -lvirt -L/usr/lib64/ -lcurl
