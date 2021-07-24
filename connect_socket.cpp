#include "connect_socket.h"

//socket exception class methods

socket_exception::socket_exception(const char* _error)
	: error(_error)
{ }

socket_exception::~socket_exception()
{
	
}

const char* socket_exception::what()
{
	return error;
}



connect_socket::~connect_socket() noexcept
{
	closesocket(socket_obj);
}

void connect_socket::connect_to_server(const std::string& _adress)
{
	int error;

	addrinfo* result = nullptr;
	addrinfo hints;

	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = nullptr;
	hints.ai_addr = nullptr;
	hints.ai_next = nullptr;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	error = getaddrinfo(_adress.c_str(), "27015", &hints, &result);

	if (error != 0)
	{
		throw socket_exception("connection error occured");
	}

	socket_obj = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (socket_obj == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		throw socket_exception("connection error occured");
	}

	error = connect(socket_obj, result->ai_addr, result->ai_addrlen);
	if (error == SOCKET_ERROR)
	{
		closesocket(socket_obj);
		socket_obj = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (socket_obj == INVALID_SOCKET)
	{
		throw socket_exception("connection error occured");
	}
}

bool connect_socket::is_valid() const noexcept
{
	if (socket_obj == INVALID_SOCKET)
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::string connect_socket::get_message() const noexcept
{
	char buffer[MAX_MESSAGE_LENGTH];
	int size;

	size = recv(socket_obj, buffer, MAX_MESSAGE_LENGTH, 0);

	if (size == -1)
	{
		return std::string();
	}
	else
	{
		buffer[size] = 0;
		return std::string(buffer);
	}
}

void connect_socket::send_message(const std::string& _message) const noexcept
{
	send(socket_obj, _message.c_str(), _message.size(), 0);
}