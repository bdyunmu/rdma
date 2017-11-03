RDMA_Server:rdma_server.o RDMA_Client
	g++ -std=c++11 rdma_server.o -L/usr/local/ofed/lib64 -L/usr/local/ofed/lib -lpthread -libverbs -lrdmacm -o rdma_server
RDMA_Client:rdma_client.o
	g++ -std=c++11 rdma_client.o -L/usr/local/ofed/lib64 -L/usr/local/ofed/lib -lpthread -libverbs -lrdmacm -o rdma_client
rdma_client.o:
	g++ -std=c++11 -c rdma_client.c -o rdma_client.o -Wall -O2 -I/usr/local/ofed/include
rdma_server.o:
	g++ -std=c++11 -Wall -O2 -c rdma_server.c -o rdma_server.o -I/usr/local/ofed/include
clean:
	rm *.o rdma_server rdma_client
