#include "answer_data.h"

void answer_data::parse_groups(const std::string& _answer, int _offset) noexcept
{
	if (_offset == _answer.size())
	{
		return;
	}

	groups_count = (unsigned int)_answer[_offset];

	_offset += 2;

	groups = new std::string[groups_count];

	for (int i = 0; i < groups_count; ++i)
	{
		int size = _answer[_offset];
		groups[i].reserve(size);
		++_offset;
		while (_offset < _answer.size())
		{
			if (_answer[_offset] != '&')
			{
				groups[i] += _answer[_offset];
			}
			else
			{
				break;
			}
			++_offset;
		}
		++_offset;
	}
}



answer_data::answer_data(const std::string& _answer) noexcept
{
	if (_answer[0] != '/')
	{
		command == NOT_AN_ANSWER;
		return;
	}

	int size = _answer[1];

	command.reserve(size);
	for (int i = 1; i < size + 1; ++i)
	{
		command += _answer[i];
	}

	int offset = 2;

	while (_answer[offset] != '&')
	{
		++offset;
	}
	++offset;

	size = _answer[offset];

	status.reserve(size);
	for (int i = 1; i < size + 1; ++i)
	{
		status += _answer[i];
	}

	if (command == LIST)
	{
		parse_groups(_answer, offset + size + 1);
	}
}

answer_data::~answer_data() noexcept
{
	delete[] groups;
}



const std::string& answer_data::get_command() const noexcept
{
	return command;
}

const std::string& answer_data::get_status() const noexcept
{
	return status;
}

int answer_data::get_group_count() const noexcept
{
	return groups_count;
}

const std::string& answer_data::get_group(int _index) const
{
	if (_index >= groups_count || _index < 0)
	{
		throw _index;
	}

	return groups[_index];
}