RDMA_Server:server.o RDMA_Client
	g++ server.o -L/usr/local/ofed/lib64 -L/usr/local/ofed/lib -lpthread -libverbs -lrdmacm -o rdma_server
RDMA_Client:client.o
	g++ client.o -L/usr/local/ofed/lib64 -L/usr/local/ofed/lib -lpthread -libverbs -lrdmacm -o rdma_client
client.o:
	g++ -c client.c -o client.o -Wall -O2 -I/usr/local/ofed/include
server.o:
	g++ -Wall -O2 -c server.c -o server.o -I/usr/local/ofed/include
