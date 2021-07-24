#ifndef __CONNECT_SOCKET
#define __CONNECT_SOCKET

#include <WS2tcpip.h>
#include <string>
#include <iostream>

#ifndef MAX_MESSAGE_LENGTH
#define MAX_MESSAGE_LENGTH 8192
#endif

class socket_exception
{
private:
	const char* error;

public:
	socket_exception(const char* _error);

	~socket_exception();

	const char* what();
};



class connect_socket
{
private:
	SOCKET socket_obj = INVALID_SOCKET;

public:
	~connect_socket() noexcept;

	void connect_to_server(const std::string& _adress);

	bool is_valid() const noexcept;

	std::string get_message() const noexcept;

	void send_message(const std::string& _message) const noexcept;

};

#endif