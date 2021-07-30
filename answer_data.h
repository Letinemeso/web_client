#ifndef __ANSWER_DATA
#define __ANSWER_DATA

#include <string>

#define NOT_AN_ANSWER ""
#define FAIL "fail"
#define SUCCESS "success"
#define NEW "new"
#define CONNECT "connect"
#define LIST "list"
#define DISCONNECT "disconnect"

class answer_data
{
private:
	std::string command, status;

	int groups_count = 0;
	std::string* groups = nullptr;

private:
	void parse_groups(const std::string& _answer, int _offset) noexcept;

public:
	answer_data() = delete;
	answer_data(answer_data&& _answer_data) = delete;
	answer_data(const answer_data& _answer_data) = delete;

	answer_data(const std::string& _answer) noexcept;
	
	~answer_data() noexcept;

	const std::string& get_command() const noexcept;
	const std::string& get_status() const noexcept;
	int get_group_count() const noexcept;
	const std::string& get_group(int _index) const;

};

#endif