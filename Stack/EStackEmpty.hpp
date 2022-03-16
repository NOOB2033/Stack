#ifndef _EStackEmpty_hpp
#define _EStackEmpty_hpp


#include "EStackException.hpp"


class EStackEmpty final : public EStackException
{
public:
	/* Конструкторы и деструктор */
	EStackEmpty();
	EStackEmpty(const EStackEmpty& oth);
	EStackEmpty(EStackEmpty&& oth) noexcept;
	virtual ~EStackEmpty() = default;

	const std::string& what() const noexcept; /* Возвращает строку с ошибкой */
};


EStackEmpty::EStackEmpty()
	: EStackException("Stack is empty!\n")
{}

EStackEmpty::EStackEmpty(const EStackEmpty& oth)
	: EStackException(oth)
{}

EStackEmpty::EStackEmpty(EStackEmpty&& oth) noexcept
	: EStackException(std::move(oth))
{}


const std::string& EStackEmpty::what() const noexcept
{
	return string;
}


#endif