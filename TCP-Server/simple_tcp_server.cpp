#include "simple_tcp_server.h"

shared_ptr<simple_tcp_server> simple_tcp_server::m_server = nullptr;

simple_tcp_server::simple_tcp_server(string address, int port, int _capacity, int _timeout,
	function<void(string, int port, SOCKET)> connect_func,
	function<void(string, SOCKET)> worker_func,
	function<void(string, int port, SOCKET)> disconnect_func)
	:m_address(address),
	 m_port(port),
	 m_server_capacity(_capacity),
	 m_timeout(_timeout),
	 m_worker_call_back(worker_func),
	 m_connect_call_back(connect_func),
	 m_disconnect_call_back(disconnect_func)
{
}

simple_tcp_server::~simple_tcp_server()
{
}
void simple_tcp_server::run() {
	if (m_thread == nullptr) {
		m_thread = make_shared<thread>(&simple_tcp_server::worker, this);
		m_thread->detach();
	}
}
void simple_tcp_server::worker()
{
	WSAData wsaData;
	int err_msg = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (NO_ERROR != err_msg) {
		printf("wsastartup failure error code:%d\n",err_msg);
		return;
	}
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
	{
		printf("create socket failed error code: %d\n", WSAGetLastError());
		WSACleanup();
		return ;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_port);
	serverAddr.sin_addr.s_addr = inet_addr(m_address.c_str());

	err_msg = ::bind(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err_msg == SOCKET_ERROR) {
		printf("bind failed error code: %d\n", err_msg);
		return;
	}
	else {
	}
	if (listen(m_sock, m_server_capacity) == SOCKET_ERROR) {
		printf("listen failed \n");
		return;
	}
	printf("tcp://%s:%d server runing...\n", m_address.c_str(), m_port);
	while (true) {
		sockaddr_in remote_sockaddr;
		int len = sizeof(sockaddr_in);
		tcp_client remote_endpoint;
		remote_endpoint.m_sock = accept(m_sock, (sockaddr*)&remote_sockaddr, &len);
		remote_endpoint.m_address = inet_ntoa(remote_sockaddr.sin_addr);
		remote_endpoint.m_port = remote_sockaddr.sin_port;

		//printf("%s:%d connect\n",remote_endpoint.m_address.c_str() , remote_endpoint.m_port);		
		thread t(&simple_tcp_server::sub_worker,this, remote_endpoint);
		t.detach();
	}
}
void simple_tcp_server::sub_worker(tcp_client remote_endpoint) {
	int rx_buffer_size = 0;
	char rx_buffer[2048] = "";
	if(m_timeout > 0)setsockopt(remote_endpoint.m_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_timeout, sizeof(int));

	m_connect_call_back(remote_endpoint.m_address.c_str(), remote_endpoint.m_port,remote_endpoint.m_sock);
	while (true) {
		rx_buffer_size = SOCKET_ERROR;
		memset(rx_buffer, 0x00, sizeof(rx_buffer));

		rx_buffer_size = recv(remote_endpoint.m_sock, (char*)rx_buffer, sizeof(rx_buffer), 0);
		if (rx_buffer_size == SOCKET_ERROR) {
			//printf("%s:%d disconnect\n", remote_endpoint.m_address.c_str(), remote_endpoint.m_port);
			closesocket(remote_endpoint.m_sock);
			m_disconnect_call_back(remote_endpoint.m_address.c_str(), remote_endpoint.m_port, remote_endpoint.m_sock);
			return;
		}
		if (rx_buffer_size == 0) {
			//printf("%s:%d disconnect\n", remote_endpoint.m_address.c_str(), remote_endpoint.m_port);
			closesocket(remote_endpoint.m_sock);
			m_disconnect_call_back(remote_endpoint.m_address.c_str(), remote_endpoint.m_port, remote_endpoint.m_sock);
			return;

		}
		m_worker_call_back(rx_buffer, remote_endpoint.m_sock);
	}
}
void simple_tcp_server::run(string server, int port, int _capacity, int _timeout,
	function<void(string, int, SOCKET)> connect_func,
	function<void(string, SOCKET)> worker,
	function<void(string, int, SOCKET)> disconnect_func)
{
	if (m_server == nullptr) {
		m_server = make_shared<simple_tcp_server>(server, port, _capacity, _timeout, connect_func, worker, disconnect_func);
		m_server->run();
	}
}
