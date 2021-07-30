#include <WS2tcpip.h>

#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#include "connect_socket.h"
#include "answer_data.h"

//user status

#define NOT_AUTHORISED	1
#define AUTHORISED		2

//command retur values

#define NO_ACTION		100
#define COMMAND_ERROR	101
#define INPUT_OK		111
#define INPUT_ERROR		112
#define INPUT_CANCELED	113

//utility

bool is_letter_or_digit(char _c)
{
	if ((_c > 47 && _c < 58) || (_c > 64 && _c < 91) || (_c > 96 && _c < 123) || (_c == '_')) return true;
	else return false;
}

//command functions

int new_data_input(std::string& _data)
{
	std::string name, password;
	std::cout << "enter group name and password or \"/cancel\".\n"
		"name and password can only contain english letters, digits or \'_\' and can't be longer then 20 symbols\n";
	std::cout << "name: ";
	std::cin >> name;
	std::cout << "\n";
	if (name == "/cancel") return INPUT_CANCELED;
	if (name.size() > 20 || name.size() == 0) return INPUT_ERROR;
	for (int i = 0; i < name.size(); ++i)
	{
		if (is_letter_or_digit(name[i]) == false) return INPUT_ERROR;
	}

	std::cout << "password: ";
	std::cin >> password;
	std::cout << "\n";
	if (password == "/cancel") return INPUT_CANCELED;
	if (password.size() > 20 || password.size() == 0) return INPUT_ERROR;
	for (int i = 0; i < password.size(); ++i)
	{
		if (is_letter_or_digit(password[i]) == false) return INPUT_ERROR;
	}

	_data += '&';
	_data += (char)name.size();
	_data += name;
	_data += '&';
	_data += (char)password.size();
	_data += password;

	return INPUT_OK;
}

int connect_data_input(std::string& _data)
{
	std::string name, password;
	std::cout << "enter group name and password or \"/cancel\".\n";
	std::cout << "name: ";
	std::cin >> name;
	std::cout << "\n";
	if (name == "/cancel") return INPUT_CANCELED;
	if (name.size() > 20 || name.size() == 0) return INPUT_ERROR;
	for (int i = 0; i < name.size(); ++i)
	{
		if (is_letter_or_digit(name[i]) == false) return INPUT_ERROR;
	}

	std::cout << "password: ";
	std::cin >> password;
	std::cout << "\n";
	if (password == "/cancel") return INPUT_CANCELED;
	if (password.size() > 20 || password.size() == 0) return INPUT_ERROR;
	for (int i = 0; i < password.size(); ++i)
	{
		if (is_letter_or_digit(password[i]) == false) return INPUT_ERROR;
	}

	_data += '&';
	_data += (char)name.size();
	_data += name;
	_data += '&';
	_data += (char)password.size();
	_data += password;

	return INPUT_OK;
}

void print_help(int _mode)
{
	if (_mode == NOT_AUTHORISED)
	{
		std::cout << "here are availible commands:\n"
			"\t\"/new\" - create new group and connect to it\n"
			"\t\"/connect\" - connect to existing group\n"
			"\t\"/list\" - get list of groups\n\n";
	}
	else if (_mode == AUTHORISED)
	{
		std::cout << "here are availible commands:\n"
			"\t\"/disconnect\" - leave group\n\n";
	}
}



//unites some additional functions

int complete_command(std::string& _data, int _mode)
{
	if (_data == "/help")
	{
		print_help(_mode);
		return NO_ACTION;
	}

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
			return INPUT_OK;
		}
	}
	else if (_mode == AUTHORISED)
	{
		if (_data[0] != '/')
		{
			return INPUT_OK;
		}
		else if (_data == "/disconnect")
		{
			return INPUT_OK;
		}
	}

	return COMMAND_ERROR;
}

//function for thread

void get_message(const connect_socket const* _socket, int* const _mode)
{
	std::string message;

	do
	{
		message = _socket->get_message();

		answer_data ad(message);

		if (*_mode == NOT_AUTHORISED)
		{
			if (ad.get_command() == NEW)
			{
				if (ad.get_status() == SUCCESS)
				{
					*_mode = AUTHORISED;
					std::cout << "you have successfuly created and connected to the group\n\n";
				}
				else if (ad.get_status() == FAIL)
				{
					std::cout << "could not connect to the group\n\n";
				}
			}
			else if (ad.get_command() == CONNECT)
			{
				if (ad.get_command() == SUCCESS)
				{
					*_mode = AUTHORISED;
					std::cout << "you have successfuly connected to the group\n\n";
				}
				else if (ad.get_command() == FAIL)
				{
					std::cout << "could not connect to the group\n\n";
				}
			}
			else if (ad.get_command() == LIST)
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
			if (ad.get_command() == DISCONNECT)
			{
				std::cout << "you have disconnected from group\n\n";
				*_mode = NOT_AUTHORISED;
			}
			else if (ad.get_command() == NOT_AN_ANSWER)
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

	int action_code;

	do
	{
		std::cin >> message;

		action_code = complete_command(message, mode);

		if (action_code == INPUT_OK)
		{
			cs.send_message(message);
		}
		else if (action_code == INPUT_ERROR)
		{
			std::cout << "some data is incorrect\n\n";
		}
		else if (action_code == INPUT_CANCELED)
		{
			std::cout << "command was canceled\n\n";
		}
		else if (action_code == COMMAND_ERROR)
		{
			std::cout << "this command does not exist\n\n";
		}
		

		/*if (message == "/help")
		{
			
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
		}*/

	} while (true);

	WSACleanup();

	return 0;
}