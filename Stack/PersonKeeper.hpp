#ifndef _PersonKeeper_hpp
#define _PersonKeeper_hpp


#include <string>
#include <fstream>


#include "stack.hpp"
#include "Person.hpp"


class PersonKeeper final
{
public:

	using container = stack<Person>;

	static PersonKeeper& instance();
	stack<Person> readPersons(std::fstream& fstream) const; /* Записываем из файла в стек и возвращаем стек */
	void writePersons(const container& stack, std::fstream& fstream) const; /* Записываем входящего из стека в файл */
private:

	PersonKeeper() = default;
	~PersonKeeper() = default;

	PersonKeeper(const Person& oth) = delete;
	PersonKeeper(Person&& oth) = delete;
	PersonKeeper& operator=(const PersonKeeper& oth) = delete;
	PersonKeeper& operator=(PersonKeeper&& oth) = delete;
};


PersonKeeper& PersonKeeper::instance()
{
	static PersonKeeper keeper;
	return keeper;
}

stack<Person> PersonKeeper::readPersons(std::fstream& fstream) const
{
	if (!fstream.is_open()) /* Проверяем, открыли или нет */
		throw std::runtime_error("File not found\n");

	stack<Person> stack;
	std::string buffer;
	while (std::getline(fstream, buffer)) /* Записываем строки в буфер */
	{
		auto it = buffer.cbegin();

		std::string last_name; /* Идем до пробела, и записываем во временную "фамилию" */
		for (; it != buffer.cend(); ++it)
		{
			if (*it == ' ')
			{
				++it;
				break;
			}
			last_name.push_back(*it);
		}

		std::string first_name;
		for (; it != buffer.cend(); ++it) /* Идем до след. пробела, и записываем во временное "имя" */
		{
			if (*it == ' ')
			{
				++it;
				break;
			}
			first_name.push_back(*it);
		}

		std::string patronymic; /* Остаток записываем в "отчество" */
		for (; it != buffer.cend(); ++it)
			patronymic.push_back(*it);

		stack.push(Person(std::move(last_name), std::move(first_name), std::move(patronymic)));/* Пушим в стек */
	}

	return stack;
}

void PersonKeeper::writePersons(const container& stack, std::fstream& fstream) const
{
	if (!fstream.is_open())
		throw std::runtime_error("File not found\n");

	for (auto it : stack.getContainer()) /* Записываем в стек из контейнера */
		fstream << it.getLastName() + ' ' + it.getFirstName() + ' ' + it.getPatronymic() + '\n';
}


#endif