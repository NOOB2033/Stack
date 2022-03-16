#ifndef _list_hpp
#define _list_hpp


#include <iostream>

/*
*  Одноопоточный контейнер с поддержкой пользовательского "stdlike" аллокатора
*  У элементов тип должен иметь конструктор по умолчанию и конструктор копированияю.
*  Отсутствует reverse-iterator.
*/
template<typename Type, typename Alloc = std::allocator<Type>> // Лучше переделать с Base_Node //
class list final
{
public:
	/* Типы */
	using value_type = Type;
	using pointer = Type*;
	using reference = Type&;
	/* Конструкторы и деструктор */
	explicit list(const Alloc& alloc = Alloc());
	list(std::size_t size, const Type& value = Type(), const Alloc& alloc = Alloc());
	list(const list& oth);
	list(list&& oth) noexcept;
	~list();

	/* Операторы */
	list& operator=(const list& oth) &;
	list& operator=(list&& oth) & noexcept;
	/* Методы */
	Type& front(); /* Возвращает ссылку на начало списка */
	const Type& front() const; /* Возвращает константную ссылку на начало списка */
	Type& back(); /* Возвращает ссылку на конец списка */
	const Type& back() const; /* Возвращает константную ссылку на конец списка */

	bool empty() const noexcept; /* Если контейнер пустой, возвращает true, иначе false */
	std::size_t size() const noexcept; /* Возвращает размер контейнера */
	void clear() noexcept; /* Чистит контейнер */

	void push_front(const Type& value); /* Кладет lvalue значение в начало */
	void push_front(Type&& value); /* Кладет rvalue* значение в начало */
	void push_back(const Type& value); /* Кладет lvalue значение в конец */
	void push_back(Type&& value); /* Кладет rvalue* значение в конец */

	template<typename... Args>
	void emplace_front(Args&&... args); /* Создает в начале элемент от входящих аргументов */
	template<typename... Args>
	void emplace_back(Args&&... args); /* Создает в конце элемент от входящих аргументов */

	void pop_front() noexcept; /* Удаляет элемент из начала */
	void pop_back() noexcept; /* Удаляет элемент из конца */
private:
	/* Узел списка */
	struct Node final
	{
		Type value = Type();
		Node* prev = nullptr;
		Node* next = nullptr;
		
		Node() = default;

		Node(const Type& value)
			: value(value)
		{}

		Node(Type&& value) noexcept
			: value(std::move(value))
		{}

		~Node() = default;
	};
public:
	/* Объявляем "ребайнднутый" тип аллокатора, который будет аллоцировать не Type, а Node<Type> */
	using RebindAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
	/* Объявляем обертку для нашешо типа */
	using AllocTraits = typename std::allocator_traits<RebindAlloc>;

	/* Итератор */
	template<bool isConst> /* База итератора */
	class base_iterator final
	{
	private:
		/* Типы */
		using curT = std::conditional_t<isConst, const Type, Type>;
		using curTRef = std::conditional_t<isConst, const Type&, Type&>;
		using curTPtr = std::conditional_t<isConst, const Type*, Type*>;

		Node* ptr = nullptr;
	public:
		/* Типы */
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = Type;
		using reference = Type&;
		using pointer = Type*;

		base_iterator(Node* ptr = nullptr) : ptr(ptr) {}
		~base_iterator() = default;

		/* Операторы */
		base_iterator& operator=(const base_iterator& another) noexcept
		{
			ptr = another.ptr;
			return *this;
		}

		bool operator!=(const base_iterator& another) const noexcept
		{
			return ptr != another.ptr;
		}

		bool operator==(const base_iterator& another) const noexcept
		{
			return ptr == another.ptr;
		}

		base_iterator& operator++()
		{
			if (ptr)
				ptr = ptr->next;

			return *this;
		}

		base_iterator& operator--()
		{
			if (ptr)
				ptr = ptr->prev;

			return *this;
		}

		base_iterator operator++(int) {
			base_iterator copy = *this;
			++(*this);
			return copy;
		}

		base_iterator operator--(int) {
			base_iterator copy = *this;
			--(*this);
			return copy;
		}

		curTRef operator*() const
		{
			return ptr->value;
		}

		curTPtr operator->() const
		{
			return &(ptr->value);
		}
	};

private:
	/* Поля */
	Node* head = nullptr;
	Node* tail = nullptr;
	RebindAlloc rebind_alloc{};
public:
	/* Тип итератора */
	using iterator = base_iterator<false>;
	using const_iterator = base_iterator<true>;
	/* Методы для работы с итераторами */
	iterator begin()
	{
		return iterator(head);
	}

	const_iterator begin() const
	{
		return const_iterator(head);
	}

	iterator end()
	{
		return iterator(nullptr);
	}

	const_iterator end() const
	{
		return const_iterator(nullptr);
	}

	const_iterator cbegin() const
	{
		return begin();
	}

	const_iterator cend() const
	{
		return end();
	}
};


template<typename Type, typename Alloc>
list<Type, Alloc>::list(const Alloc& alloc)
	: rebind_alloc(alloc)
{}

template<typename Type, typename Alloc>
list<Type, Alloc>::list(std::size_t size, const Type& value, const Alloc& alloc)
	: rebind_alloc(alloc)
{
	if (size < 1)
		return;

	head = AllocTraits::allocate(rebind_alloc, 1);
	std::size_t counter = 0; /* Счетчик сконструированных узлов */
	try
	{
		AllocTraits::construct(rebind_alloc, head, value);  /* Аллоцируем память для головы */
		tail = head;
		++counter;
		for (; counter < size; ++counter, tail = tail->next)
		{ /* Выделяем память под новые узлы и конструируем на них узлы */
			tail->next = AllocTraits::allocate(rebind_alloc, 1);
			AllocTraits::construct(rebind_alloc, tail->next, value);
			tail->next->prev = tail;
		}
	}
	catch (...)
	{ /* Если исключение, то сначала уничтожаем объекты по счетчику, затем деаллоцируем память */
		Node* temp = head;
		for (std::size_t i = 0; i < counter; ++i)
		{
			Node* _temp = temp;
			temp = temp->next;
			AllocTraits::destroy(rebind_alloc, _temp);
		}
		for (temp = head; head; temp = head)
		{
			head = head->next;
			AllocTraits::deallocate(rebind_alloc, temp, 1);
		}
		tail = nullptr;
		throw; /* Пробрассываем исключение */
	}
}/* Далее, в конструкторах все аналогично, за малым исключением */

template<typename Type, typename Alloc>
list<Type, Alloc>::list(const list& oth)
/* Если аллокатор не переопределил select_on_cont.... То возвращаем то же аллокатор */
	: list(std::allocator_traits<Alloc>::select_on_container_copy_construction(oth.rebind_alloc))
{
	if (oth.head != nullptr)
		return;

	head = AllocTraits::allocate(rebind_alloc, 1);
	std::size_t counter = 0;
	try
	{
		AllocTraits::construct(rebind_alloc, head, oth.head->value);
		tail = head;
		++counter;
		for (Node* temp = oth.head->next; temp; ++counter, temp = temp->next)
		{
			tail->next = AllocTraits::allocate(rebind_alloc, 1);
			AllocTraits::construct(rebind_alloc, tail->next, temp->value);
			tail->next->prev = tail;
			tail = tail->next;
		}
	}
	catch (...)
	{
		Node* temp = head;
		for (std::size_t i = 0; i < counter; ++i)
		{
			Node* _temp = temp;
			temp = temp->next;
			AllocTraits::destroy(rebind_alloc, _temp);
		}
		for (temp = head; head; temp = head)
		{
			head = head->next;
			AllocTraits::deallocate(rebind_alloc, temp, 1);
		}
		tail = nullptr;
		throw;
	}
}

template<typename Type, typename Alloc>
list<Type, Alloc>::list(list&& oth) noexcept
	: head(oth.head),
	tail(oth.tail),
	rebind_alloc(std::move(oth.rebind_alloc))
{
	oth.head = nullptr;
	oth.tail = nullptr;
}

template<typename Type, typename Alloc>
list<Type, Alloc>::~list()
{
	clear();
}


template<typename Type, typename Alloc>
list<Type, Alloc>& list<Type, Alloc>::operator=(const list& oth) &
{
	if (this == std::addressof(oth))
		return *this;

	clear();
	/* Определяем, должны ли мы создавать копию пула аллокатора, или просто копию без нового пула */
	if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value
		&& rebind_alloc != oth.rebind_alloc)
		rebind_alloc = oth.rebind_alloc;

	head = AllocTraits::allocate(rebind_alloc, 1);
	std::size_t counter = 0;
	try
	{
		AllocTraits::construct(rebind_alloc, head, oth.head->value);
		tail = head;
		++counter;
		for (Node* temp = oth.head->next; temp; ++counter, temp = temp->next)
		{
			tail->next = AllocTraits::allocate(rebind_alloc, 1);
			AllocTraits::construct(rebind_alloc, tail->next, temp->value);
			tail->next->prev = tail;
			tail = tail->next;
		}
	}
	catch (...)
	{
		Node* temp = head;
		for (std::size_t i = 0; i < counter; ++i)
		{
			Node* _temp = temp;
			temp = temp->next;
			AllocTraits::destroy(rebind_alloc, _temp);
		}
		for (temp = head; head; temp = head)
		{
			head = head->next;
			AllocTraits::deallocate(rebind_alloc, temp, 1);
		}
		tail = nullptr;
		throw;
	}
	return *this;
}

template<typename Type, typename Alloc>
list<Type, Alloc>& list<Type, Alloc>::operator=(list&& oth) & noexcept
{
	if (this == std::addressof(oth))
		return *this;

	clear();
	/* То же самое, что и в operator=, только сейчас муваем */
	if (std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value
		&& rebind_alloc != oth.rebind_alloc)
		rebind_alloc = std::move(oth.rebind_alloc);

	head = oth.head;
	oth.head = nullptr;
	tail = oth.tail;
	oth.tail = nullptr;
	return *this;
}

template<typename Type, typename Alloc>
Type& list<Type, Alloc>::front()
{
	if (head)
		return head->value;
	else
		throw std::runtime_error("Stack is empty!\n"); /* Если запрашиваем элемент из пустого контейнера */
}

template<typename Type, typename Alloc>
const Type& list<Type, Alloc>::front() const
{
	if (head)
		return head->value;
	else
		throw std::runtime_error("Stack is empty!\n"); /* Если запрашиваем элемент из пустого контейнера */
}

template<typename Type, typename Alloc>
Type& list<Type, Alloc>::back()
{
	if (tail)
		return tail->value;
	else
		throw std::runtime_error("Stack is empty!\n"); /* Если запрашиваем элемент из пустого контейнера */
}

template<typename Type, typename Alloc>
const Type& list<Type, Alloc>::back() const
{
	if (tail)
		return tail->value;
	else
		throw std::runtime_error("Stack is empty!\n"); /* Если запрашиваем элемент из пустого контейнера */
}

template<typename Type, typename Alloc>
bool list<Type, Alloc>::empty() const noexcept
{
	return head == nullptr;
}

template<typename Type, typename Alloc>
std::size_t list<Type, Alloc>::size() const noexcept
{
	std::size_t size = 0;
	for (auto iter : *this)
	{
		++size;
	}
	return size;
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::clear() noexcept
{
	for (Node* temp = head; head; temp = head)
	{
		head = head->next;

		AllocTraits::destroy(rebind_alloc, temp);
		AllocTraits::deallocate(rebind_alloc, temp, 1);
	}
	tail = nullptr;
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::push_front(const Type& value)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, value);
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}

	if (head) /* Если не список не пустой, то добавляем перед головой */
	{
		temp->next = head;
		head->prev = temp;
		head = temp;
	}
	else /* в противном случае, в голову */ /* Далее, аналогично */
	{
		head = temp;
		tail = head;
	}
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::push_front(Type&& value)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, std::move(value));
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}
	
	if (head)
	{
		temp->next = head;
		head->prev = temp;
		head = temp;
	}
	else
	{
		head = temp;
		tail = head;
	}
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::push_back(const Type& value)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, value);
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}

	if (tail)
	{
		tail->next = temp;
		tail->next->prev = tail;
		tail = tail->next;
	}
	else
	{
		tail = temp;
		head = tail;
	}
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::push_back(Type&& value)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, std::move(value));
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}

	if (tail)
	{
		tail->next = temp;
		tail->next->prev = tail;
		tail = tail->next;
	}
	else
	{
		tail = temp;
		head = tail;
	}
}

template<typename Type, typename Alloc>
template<typename ...Args>
void list<Type, Alloc>::emplace_front(Args&& ...args)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, std::forward<Args>(args)...);
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}

	if (head)
	{
		temp->next = head;
		head->prev = temp;
		head = temp;
	}
	else
	{
		head = temp;
		tail = head;
	}
}

template<typename Type, typename Alloc>
template<typename ...Args>
void list<Type, Alloc>::emplace_back(Args&& ...args)
{
	Node* temp = AllocTraits::allocate(rebind_alloc, 1);

	try
	{
		AllocTraits::construct(rebind_alloc, temp, std::forward<Args>(args)...);
	}
	catch (...)
	{
		AllocTraits::deallocate(rebind_alloc, temp, 1);
		throw;
	}

	if (tail)
	{
		tail->next = temp;
		tail->next->prev = tail;
		tail = tail->next;
	}
	else
	{
		tail = temp;
		head = tail;
	}
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::pop_front() noexcept
{
	if (empty())
		return;

	Node* temp = head;
	head = head->next;
	if (head) /* Если элемент был не единственный */
		head->prev = nullptr;
	else /* Если единственный, то еще и обнуляем хвост */
		tail = nullptr;

	AllocTraits::destroy(rebind_alloc, temp);
	AllocTraits::deallocate(rebind_alloc, temp, 1);
}

template<typename Type, typename Alloc>
void list<Type, Alloc>::pop_back() noexcept
{
	if (empty())
		return;

	Node* temp = tail;
	tail = tail->prev;
	if (tail)
		tail->next = nullptr;
	else
		head = nullptr;

	AllocTraits::destroy(rebind_alloc, temp);
	AllocTraits::deallocate(rebind_alloc, temp, 1);
}


#endif