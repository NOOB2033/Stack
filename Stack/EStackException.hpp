#ifndef _EStackException_hpp
#define _EStackException_hpp


#include <string>


class EStackException
{
public:
	/* Конструкторы и деструктор */
	EStackException() = default;
	EStackException(const EStackException& oth);
	EStackException(EStackException&& oth) noexcept;
	EStackException(const std::string& str);
	virtual ~EStackException() = default; /* Виртуальный, т.к. класс наследуемый */
protected:

	std::string string{};
};


EStackException::EStackException(const EStackException& oth)
	: string(oth.string)
{}

EStackException::EStackException(EStackException&& oth) noexcept
	: string(std::move(oth.string))
{}

EStackException::EStackException(const std::string& str)
	: string(str)
{}


#endif