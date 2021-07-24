#include "answer_data.h"

void answer_data::parse_groups(const std::string& _answer) noexcept
{
	int offset = 2;
	while (offset < _answer.size())
	{
		if (_answer[offset] == '&')
		{
			++offset;
			break;
		}
		++offset;
	}
	if (offset == _answer.size())
	{
		return;
	}

	for (int i = offset; i < _answer.size(); ++i)
	{
		if (_answer[i] == '&')
		{
			++groups_count;
		}
	}
	++groups_count;

	groups = new std::string[groups_count];

	for (int i = 0; i < groups_count; ++i)
	{
		int size = _answer[offset];
		groups[i].reserve(size);
		++offset;
		while (offset < _answer.size())
		{
			if (_answer[offset] != '&')
			{
				groups[i] += _answer[offset];
			}
			else
			{
				break;
			}
			++offset;
		}
		++offset;
	}
}



answer_data::answer_data(const std::string& _answer) noexcept
{
	if (_answer[0] != '/' || _answer[1] != '/')
	{
		answer == NOT_AN_ANSWER;
		return;
	}

	int offset = 2;
	int size = 0;

	while (offset + size < _answer.size())
	{
		if (_answer[offset + size] == '&')
		{
			break;
		}
		++size;
	}
	answer.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		answer += _answer[offset + i];
	}

	if (answer == LIST)
	{
		parse_groups(_answer);
	}
}

answer_data::~answer_data() noexcept
{
	delete[] groups;
}



const std::string& answer_data::get_answer() const noexcept
{
	return answer;
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