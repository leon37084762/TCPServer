#pragma once
#ifndef _SOCKET_SERVER_H
#define _SOCKET_SERVER_H
#include <bits\stdc++.h>
#include <thread>
#include "winsock2.h"  
#pragma comment(lib, "ws2_32.lib")  
using namespace std;
struct tcp_client {
	SOCKET m_sock;
	string m_address;
	int m_port;
};
class simple_tcp_server
{
public:
	simple_tcp_server(string ip, int port, int _capacity, int _timeout,
		function<void(string,int , SOCKET)> connect_func,
		function<void(string,SOCKET)> worker,
		function<void(string,int , SOCKET)> disconnect_func);
	~simple_tcp_server();
public:
	void run();
private:
	void worker();
	void sub_worker(tcp_client remote_endpoint);
private:
	function<void(string,SOCKET)> m_worker_call_back;
	function<void(string, int, SOCKET)> m_connect_call_back;
	function<void(string, int, SOCKET)> m_disconnect_call_back;
	shared_ptr<thread> m_thread;
	SOCKET m_sock;
	string m_address;
	int m_port;
	int m_server_capacity;
	int m_timeout;
public:
	static void run(string ip, int port,int _capacity,int _timeout,
		function<void(string, int, SOCKET)> connect_func,
		function<void(string, SOCKET)> worker,
		function<void(string, int, SOCKET)> disconnect_func);
private:
	static shared_ptr<simple_tcp_server> m_server;
};

#endif