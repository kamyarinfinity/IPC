all:
	g++-7 server.cpp -I/usr/local/include /usr/local/lib/libboost_system-mt.dylib /usr/local/lib/libboost_filesystem-mt.dylib -lstdc++fs -o server
	g++-7 client.cpp -I/usr/local/include /usr/local/lib/libboost_system-mt.dylib /usr/local/lib/libboost_filesystem-mt.dylib -lstdc++fs -o client
