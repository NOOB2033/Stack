#ifndef _stack_hpp
#define _stack_hpp


#include <iostream>
#include <deque>
#include <type_traits>
#include <cassert>

#include "EStackEmpty.hpp"

/* 
*  Однопоточный адаптер для контейнера. По дефолту используется двусторонняя очередь.
*  Аллокатор внутри контейнера должен быть "stdlke".
*  У контейнера должны быть реализованы следующие методы:
*  back(),
*  push_back(),
*  pop_back().
*  У элементов тип должен иметь конструктор по умолчанию и конструктор копированияю.
*/
template<typename Type, typename Container = std::deque<Type>>
class stack final
{
public:
	/* Типы */
	using value_type = typename Container::value_type;
	using pointer = typename Container::pointer;
	using reference = typename Container::reference;
	/* Проверка на соответствие элементов стека и элементов контейнера */
	static_assert(std::is_same_v<Type, value_type>, "container adaptors require consistent types");

	/* Конструкторы и деструктор */
	stack() = default;
	stack(const stack& oth);
	stack(stack&& oth) noexcept;
	~stack() = default;

	/* Операторы */
	stack& operator=(const stack& oth) &;
	stack& operator=(stack&& oth) & noexcept;
	/* Методы */
	Type& top(); /* Возвращает ссылку на верхний элемент стека */
	const Type& top() const; /* Возвращает константную ссылку на верхний элемент стека */

	bool empty() const noexcept; /* Если контейнер пустой, возвращает true, иначе false */
	std::size_t size() const noexcept; /* Возвращает размер контейнера */

	void push(const Type& value); /* Кладет lvalue значение в вершину стека */
	void push(Type&& value); /* Кладет rvalue* значение в вершину стека */

	template<typename... Args>
	void emplace(Args&&... args); /* Создает в вершине стека элемент от входящих аргументов */

	void pop(); /* Удаляет верхний элемент */

	const Container& getContainer() const noexcept;
private:

	Container container{};
};


template<typename Type, typename Container>
stack<Type, Container>::stack(const stack& oth)
	: container(oth.container)
{}

template<typename Type, typename Container>
stack<Type, Container>::stack(stack&& oth) noexcept
	: container(std::move(oth.container))
{}


template<typename Type, typename Container>
stack<Type, Container>& stack<Type, Container>::operator=(const stack& oth) &
{
	container = oth.container;
	return *this;
}

template<typename Type, typename Container>
stack<Type, Container>& stack<Type, Container>::operator=(stack&& oth) & noexcept
{
	container = std::move(oth.container);
	return *this;
}

template<typename Type, typename Container>
Type& stack<Type, Container>::top()
{
	if (container.empty())
		throw EStackEmpty(); /* Если контейнер пустой - кидаем исключение */
	else
		return container.back();
}

template<typename Type, typename Container>
const Type& stack<Type, Container>::top() const
{
	if (container.empty())
		throw EStackEmpty(); /* Если контейнер пустой - кидаем исключение */
	else
		return container.back();
}


template<typename Type, typename Container>
bool stack<Type, Container>::empty() const noexcept
{
	return container.empty();
}

template<typename Type, typename Container>
std::size_t stack<Type, Container>::size() const noexcept
{
	return container.size();
}

template<typename Type, typename Container>
void stack<Type, Container>::push(const Type& value)
{
	container.push_back(value);
}

template<typename Type, typename Container>
void stack<Type, Container>::push(Type&& value)
{
	container.push_back(std::move(value)); /* Вызываем push_back контейнера от "мувнутого" значения */
}

template<typename Type, typename Container>
template<typename... Args>
void stack<Type, Container>::emplace(Args&&... args)
{
	container.emplace_back(std::forward<Args>(args)...); /* Вызываем emplace_back контейнера от "форварднутых" аргументов */
}

template<typename Type, typename Container>
void stack<Type, Container>::pop()
{
	if (container.empty())
		throw EStackEmpty(); /* Если контейнер пустой - кидаем исключение */
	else
		container.pop_back();
}

template<typename Type, typename Container>
const Container& stack<Type, Container>::getContainer() const noexcept
{
	return container;
}


#endif