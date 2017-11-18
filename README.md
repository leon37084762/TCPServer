# TCPServer
Simple TCP Server in C++11(first version is vc2015 project)
``` 
void client_connect_func(string address,int point, SOCKET remote_sock);
void working_func(string str_rx, SOCKET remote_sock);
void client_disconnect_func(string address, int point, SOCKET remote_sock);


int main(int argc, char* argv[]) {
    //"0.0.0.0" ip
    //35698 local port
    //100   capacity
    //10000 recv time out,  millisecond   
    simple_tcp_server::run("0.0.0.0", 35698, 100, 10000,
		client_connect_func,
		working_func, 
		client_disconnect_func);

	while (true) {
		Sleep(1000);
	}
	return 0x01;
}
void client_connect_func(string address, int port, SOCKET remote_sock) {
	printf("%s:%d connect\n", address.c_str(), port);
}
void working_func(string str_rx, SOCKET remote_sock) {
	printf("%s\n", str_rx.c_str());
    //echo to client
	send(remote_sock, str_rx.c_str(), str_rx.size(), 0);
}
void client_disconnect_func(string address, int port, SOCKET remote_sock) {
	printf("%s:%d disconnect\n", address.c_str(), port);
}
```