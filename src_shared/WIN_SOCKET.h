//============================================================================
// Name        : Ravioli Racing
// Author      : Branden Lee
// Version     : 0.01
// Copyright   : GPL
// Description : C++ Racing Game
//
// IDE: Eclipse Version: Neon.2 Release (4.6.2)
// Requires Cygwin in windows and GCC in linux
//============================================================================
#ifndef WIN_SOCKET_H
#define WIN_SOCKET_H
#ifdef _WIN32
#define __USE_MINGW_ANSI_STDIO 0 // clears error: multiple definition of `vsnprintf'

#include <cstring>       // strlen
#include <iostream>
#include <locale>
#include <string>
#include <stdio.h>
#include <fstream>
#include <stdexcept>     // std::runtime_error
#include <functional>
#include <chrono>
#include <vector>
#include <signal.h>
#include <io.h>

/* windows sock */
#define _WIN32_WINNT 0x6000 // getaddrinfo and freeaddrinfo
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // MAKEWORD
#include <winsock2.h>
#include <ws2tcpip.h> // getaddrinfo

#define BUFFER_SIZE 1024
#define NO_SOCKET -1
#define EXIT_FAILURE 1

/* Maximum bytes that can be send() or recv() via net by one call.
 * It's a good idea to test sending one byte by one.
 */
#define MAX_SEND_SIZE 100

/* Size of send queue (messages). */
#define MAX_MESSAGES_BUFFER_SIZE 10

#define SENDER_MAXSIZE 128
#define DATA_MAXSIZE 512
#define MSG_DONTWAIT 0

#define EAGAIN          11      /* Try again */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */

class message_t {
public:
	char sender[SENDER_MAXSIZE];
	char data[DATA_MAXSIZE];
};

class message_queue_t {
public:
	int size;
	message_t *data;
	int current;
};

class peer_t {
public:
	int socket;
	sockaddr_in addres;

	/* Messages that waiting for send. */
	message_queue_t send_buffer;

	/* Buffered sending message.
	 *
	 * In case we doesn't send whole message per one call send().
	 * And current_sending_byte is a pointer to the part of data that will be send next call.
	 */
	message_t sending_buffer;
	size_t current_sending_byte;

	/* The same for the receiving message. */
	message_t receiving_buffer;
	size_t current_receiving_byte;

};

/**
 @class WinHTTP
 Uses libraries for a basic winsock application.
 A collection of HTTP methods.
 */
class WIN_SOCKET {
private:
	sockaddr_in servAddr;
	int serverSd;
	int acceptSd;
	bool listenFlag;
	std::string host;
	u_short port;
	std::vector<peer_t*> connection_list;
	fd_set read_fds;
	fd_set write_fds;
	fd_set except_fds;

public:
	WIN_SOCKET(int port);
	WIN_SOCKET(std::string host_, int port);
	void sockSetup();
	void sockConnect();
	void sockBind();
	void sockListen(std::function<void(WIN_SOCKET*)>* conCB, std::function<void(WIN_SOCKET*)>* dconCB, std::function<void(WIN_SOCKET*)>* rcvCB);
	void sockLoop(std::function<void(WIN_SOCKET*)>* listenCB);
	// message --------------------------------------------------------------------
	int prepare_message(char *sender, char *data, message_t *message);
	int print_message(message_t *message);
	// message queue --------------------------------------------------------------
	int create_message_queue(int queue_size, message_queue_t *queue);
	void delete_message_queue(message_queue_t *queue);
	int enqueue(message_queue_t *queue, message_t *message);
	int dequeue(message_queue_t *queue, message_t *message);
	int dequeue_all(message_queue_t *queue);

	int delete_peer(peer_t *peer);
	int create_peer(peer_t *peer);
	char* peer_get_addres_str(peer_t *peer);
	int peer_add_to_send(peer_t *peer, message_t *message);
	int receive_from_peer(peer_t *peer, std::function<int(message_t*)>* CB);
	int send_to_peer(peer_t *peer);

	int read_from_stdin(char *read_buffer, size_t max_len);

	void shutdown_properly(int code);
	int build_fd_sets(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);
	int handle_new_connection();
	int close_client_connection(peer_t *client);
	int handle_read_from_stdin();
	int handle_received_message(message_t *message);

	void stdinListen(std::string str);

	void sendFromServer(std::string data);
};

typedef void (WIN_SOCKET::*WIN_SOCKET_FN)(int code);

class signalHandle {
private:
	static bool cb_flag;
	static WIN_SOCKET o;
	static WIN_SOCKET_FN fn;

public:
	static void cb(int sig_number);
	static int init();
};

#endif
#endif
