send.cgi: send.cpp
	g++ -o send.cgi `cups-config --cflags` send.cpp `cups-config --libs` -lcgicc -lfcgi
