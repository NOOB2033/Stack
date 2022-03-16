#ifndef _Person_hpp
#define _Person_hpp


#include <iostream>
#include <string>


class Person final
{
public:
	/* Конструкторы */
	Person() = default;
	Person(const Person& oth);
	Person(Person&& oth) noexcept;
	Person(const std::string& last_name, const std::string& first_name, const std::string& patronymic);
	Person(std::string&& last_name, std::string&& first_name, std::string&& patronymic) noexcept;
	~Person() = default;

	/* Операторы */
	Person& operator=(const Person& oth) &;
	Person& operator=(Person&& oth) & noexcept;
	/* Методы */
	const std::string& getLastName() const noexcept;
	const std::string& getFirstName() const noexcept;
	const std::string& getPatronymic() const noexcept;

	void setLastName(const std::string& last_name);
	void setLastName(std::string&& last_name) noexcept;
	void setFirstName(const std::string& first_name);
	void setFirstName(std::string&& first_name) noexcept;
	void setPatronymic(const std::string& patronymic);
	void setPatronymic(std::string&& patronymic) noexcept;
private:

	std::string last_name{};
	std::string first_name{};
	std::string patronymic{};
};


Person::Person(const Person& oth)
	: last_name(oth.last_name),
	first_name(oth.first_name),
	patronymic(oth.patronymic)
{}

Person::Person(Person&& oth) noexcept
	: last_name(std::move(oth.last_name)),
	first_name(std::move(oth.first_name)),
	patronymic(std::move(oth.patronymic))
{}

Person::Person(const std::string& last_name, const std::string& first_name, const std::string& patronymic)
	: last_name(last_name),
	first_name(first_name),
	patronymic(patronymic)
{}

Person::Person(std::string&& last_name, std::string&& first_name, std::string&& patronymic) noexcept
	: last_name(std::move(last_name)),
	first_name(std::move(first_name)),
	patronymic(std::move(patronymic))
{}


Person& Person::operator=(const Person& oth) &
{
	if (this == std::addressof(oth))
		return *this;

	last_name = oth.last_name;
	first_name = oth.first_name;
	patronymic = oth.patronymic;
	return *this;
}

Person& Person::operator=(Person&& oth) & noexcept
{
	if (this == std::addressof(oth))
		return *this;

	last_name = std::move(oth.last_name);
	first_name = std::move(oth.first_name);
	patronymic = std::move(oth.patronymic);
	return *this;
}

const std::string& Person::getLastName() const noexcept
{
	return last_name;
}

const std::string& Person::getFirstName() const noexcept
{
	return first_name;
}

const std::string& Person::getPatronymic() const noexcept
{
	return patronymic;
}

void Person::setLastName(const std::string& last_name)
{
	this->last_name = last_name;
}

void Person::setLastName(std::string&& last_name) noexcept
{
	this->last_name = std::move(last_name);
}

void Person::setFirstName(const std::string& first_name)
{
	this->first_name = first_name;
}

void Person::setFirstName(std::string&& first_name) noexcept
{
	this->first_name = std::move(first_name);
}

void Person::setPatronymic(const std::string& patronymic)
{
	this->patronymic = patronymic;
}

void Person::setPatronymic(std::string&& patronymic) noexcept
{
	this->patronymic = std::move(patronymic);
}


#endif