#include <WS2tcpip.h>

#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#include "connect_socket.h"
#include "answer_data.h"


#define NOT_AUTHORISED 1
#define AUTHORISED 2



//command functions

bool new_data_input(std::string& _data)
{
	std::string name, password;
	std::cout << "enter group name and password:\n";
	std::cin >> name >> password;

	if (name.size() == 0 || password.size() == 0)
	{
		return false;
	}

	_data += '&';
	_data += (char)name.size();
	_data += name;
	_data += '&';
	_data += (char)password.size();
	_data += password;

	return true;
}

bool connect_data_input(std::string& _data)
{
	std::string name, password;
	std::cout << "enter group name and password:\n";
	std::cin >> name >> password;

	if (name.size() == 0 || password.size() == 0)
	{
		return false;
	}

	_data += '&';
	_data += (char)name.size();
	_data += name;
	_data += '&';
	_data += (char)password.size();
	_data += password;

	return true;
}

bool complete_command(std::string& _data, int _mode)
{
	if (_mode == NOT_AUTHORISED)
	{
		if (_data == "/new")
		{
			return new_data_input(_data);
		}
		else if (_data == "/connect")
		{
			return connect_data_input(_data);
		}
		else if (_data == "/list")
		{
			return true;
		}
	}
	else if (_mode == AUTHORISED)
	{
		if (_data == "/disconnect")
		{
			return true;
		}
	}

	return false;
}

//function for thread

void get_message(const connect_socket const* _socket, int* _mode)
{
	std::string message;

	do
	{
		message = _socket->get_message();

		answer_data ad(message);

		if (*_mode == NOT_AUTHORISED)
		{
			if (ad.get_answer() == SUCCESS)
			{
				*_mode = AUTHORISED;
				std::cout << "you have successfuly connected to the group\n\n";
			}
			else if (ad.get_answer() == FAIL)
			{
				std::cout << "could not connect to the group\n\n";
			}
			else if (ad.get_answer() == LIST)
			{
				if (ad.get_group_count() == 0)
				{
					std::cout << "no groups currently exist\n\n";
				}
				else
				{
					std::cout << "list of existing groups:\n";
					for (int i = 0; i < ad.get_group_count(); ++i)
					{
						std::cout << "\t" << ad.get_group(i) << "\n";
					}
					std::cout << "\n";
				}
			}
		}
		else if(*_mode == AUTHORISED)
		{
			if (ad.get_answer() == DISCONNECTED)
			{
				std::cout << "you disconnected from group\n\n";
				*_mode = NOT_AUTHORISED;
			}
			else if (ad.get_answer() == NOT_AN_ANSWER)
			{
				std::cout << "got an incoming message:\n" << message << "\n\n";
			}
		}

	} while (message.size() != 0);

	std::cout << "connection lost\n\n";
}







int main()
{
	WSADATA wdata;

	int error = WSAStartup(MAKEWORD(2, 2), &wdata);
	
	if (error != 0)
	{
		std::cout << "startup error: " << error << "\n\n";
		WSACleanup();
		return 1;
	}

	connect_socket cs;

	while (cs.is_valid() != true)
	{
		std::string adress;
		std::cout << "enter ip adress:\n";
		std::cin >> adress;
		std::cout << "\n";

		try
		{
			cs.connect_to_server(adress);
		}
		catch (socket_exception& e)
		{
			std::cout << e.what() << "\n\n";
		}
	}
	
	


	int mode = NOT_AUTHORISED;

	std::thread get_message_thread(&get_message, &cs, &mode);
	get_message_thread.detach();

	std::string message;

	std::cout << "connected to the server \n\n type in \"/help\" to see list of availible commands\n\n";

	do
	{
		bool no_input_error = false;
		std::cin >> message;

		if (message == "/help")
		{
			if (mode == NOT_AUTHORISED)
			{
				std::cout << "here are availible commands:\n"
					"\t\"/new\" - create new group and connect to it\n"
					"\t\"/connect\" - connect to existing group\n"
					"\t\"/list\" - get list of groups\n\n";
			}
			else if (mode == AUTHORISED)
			{
				std::cout << "here are availible commands:\n"
					"\t\"/disconnect\" - leave group\n\n";
			}
		}
		else
		{
			if (mode == NOT_AUTHORISED)
			{
				no_input_error = complete_command(message, mode);

				if (no_input_error == true)
				{
					cs.send_message(message);
				}
				else
				{
					std::cout << "some entered data is incorrect\n\n";
				}
			}
			else if (mode == AUTHORISED)
			{
				if (message[0] == '/')
				{
					no_input_error = complete_command(message, mode);

					if (no_input_error == true)
					{
						cs.send_message(message);
					}
					else
					{
						std::cout << "some entered data is incorrect\n\n";
					}

				}
				else
				{
					cs.send_message(message);
				}
			}
		}

	} while (true);

	WSACleanup();

	return 0;
}