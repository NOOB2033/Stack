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
	const stack<Person>& readPersons(const std::string& filename); /* Записываем из файла в стек и возвращаем стек */
	void writePersons(const container& stack, const std::string& filename) const; /* Записываем входящего из стека в файл */
	//void writePersons(const stack<Person>& stack);
private:

	PersonKeeper() = default;
	~PersonKeeper() = default;

	PersonKeeper(const Person& oth) = delete;
	PersonKeeper(Person&& oth) = delete;
	PersonKeeper& operator=(const PersonKeeper& oth) = delete;
	PersonKeeper& operator=(PersonKeeper&& oth) = delete;

	container stack{};
};


PersonKeeper& PersonKeeper::instance()
{
	static PersonKeeper keeper;
	return keeper;
}

const stack<Person>& PersonKeeper::readPersons(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open()) /* Проверяем, открыли или нет */
		throw std::runtime_error("File not found\n");

	std::string buffer;
	while (std::getline(file, buffer)) /* Записываем строки в буфер */
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

		stack.push(Person(std::move(last_name), std::move(first_name), std::move(patronymic))); /* Пушим в стек */
	}

	return stack;
}

void PersonKeeper::writePersons(const container& stack, const std::string& filename) const
{
	std::ofstream file(filename);

	if (!file.is_open())
		throw std::runtime_error("File not found\n");

	for (auto it : stack.getContainer()) /* Записываем в стек из контейнера */
		file << it.getLastName() + ' ' + it.getFirstName() + ' ' + it.getPatronymic() + '\n';
}


#endif