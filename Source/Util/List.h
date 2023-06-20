#pragma once
namespace Util
{
	template<class T>
	struct ListNode
	{
		T data = NULL;
		ListNode<T>* m_next = nullptr;
	};

	template<typename T>
	class List
	{
	public:
		List() = default;
		~List() { clear(); };

		T operator [] (int i)
		{
			return get(i);
		}

		int search(const T& value)
		{
			unsigned int i = 0;

			while (i < m_size) {
				ListNode<T>* headerNode = getNode(i);
					
				if(headerNode && value == headerNode->data)
					return i;

				i++;
			}

			return -1;
		};
		size_t size()
		{
			return m_size;
		};
		bool isEmpty()
		{
			return m_size == 0;
		};
		void add(size_t m_index, const T& value)
		{
			//  returns if the values are not with in the list size and below zero
			if (m_index < 0 || m_index >= m_size)
				return;

			// Creates a new Node 
			ListNode<T>* newNode = new ListNode<T>();

			// Sets the value to the value
			newNode->data = value;

			if (m_index == 0)
			{
				newNode->m_next = m_begin->m_next;

				m_begin = newNode;
			}
			else
			{
				ListNode<T>* previousNode = getNode(m_index - 1);
				newNode->m_next = previousNode->m_next;
				previousNode->m_next = newNode;
			}

			m_size++;
		};
		// Const is because I am sure I won't change any value
		// only adding it to the list
		void add(const T& value)
		{
			// create new node
			ListNode<T>* newNode = new ListNode<T>();
			// sets the data for the new node
			newNode->data = value;

			if (!m_begin)
				m_begin = newNode;

			// adds the new node to the end and make the m_previous end connect with the new end
			if (m_end) {
				m_end->m_next = newNode;
				m_end = newNode;
			}
			else {
				m_end = newNode;
			}

			// Makes the list bigger
			m_size++;
		};
		void push(const T& value)
		{
			// create new node
			ListNode<T>* newNode = new ListNode<T>();
			// sets the data for the new node
			newNode->data = value;

			newNode->m_next = m_begin;
			m_begin = newNode;

			m_size++;
		};
		void replace(size_t m_index, const T& value)
		{
			if ((m_index >= 0) && (m_index < m_size))
				getNode(m_index)->data = value;
		};
		void remove(size_t m_index)
		{
			if ((m_index < 0) || (m_index >= m_size))
				return;

			ListNode<T>* nodeToDelete = getNode(m_index);


			if (m_index == 0)
			{
				m_begin = nodeToDelete->m_next;
			}
			else
			{
				// just sets the m_next value of the deleted node connect with the m_previous node
				// m_previous node -> deleted node -> m_next node of deleted node 
				// = m_previous node -> m_next node of deleted node 
				ListNode<T>* previousNode = getNode(m_index - 1);

				assert(previousNode->m_next);
				previousNode->m_next = nodeToDelete->m_next;
				
				if(m_size - 1 == m_index)
					m_end = previousNode;
			}

			delete nodeToDelete;

			ResetSavedNode();

			m_size--;

			if (m_size == 0)
				m_end = nullptr;
		};
		T pop()
		{
			T data = getLast();

			remove(m_size - 1);

			return data;
		};
		T& get(int m_index)
		{
			ListNode<T>* selectedNode = getNode(m_index);

			return selectedNode->data;
		};
		T& getFirst()
		{
			return get(0);
		};
		T& getLast()
		{
			return get(m_size - 1);
		};
		void ResetSavedNode()
		{
			m_last = nullptr;
			m_lastIndex = -1;
			m_savedNode = false;
		}
		ListNode<T>* getNode(size_t m_index)
		{
			//  returns if the values are not with in the list size and below zero
			assert(m_index >= 0 && m_index < m_size);

			ListNode<T>* header = m_begin;
			size_t count = 0;

			// If the last node was the node we got it sets the value to it
			if (m_savedNode && m_index == m_lastIndex)
			{
				count = m_lastIndex;
				header = m_last;
			}

			// Loops through the linked list
			while (header && count < m_index)
			{
				header = header->m_next;
				count++;
			}

			// it saves the old node
			if (header) {
				m_savedNode = true;
				m_lastIndex = count;
				m_last = header;
			}

			assert(header != nullptr);

			return header;
		};
		void removeFirst() 
		{
			remove(0);
		}
		void clear()
		{
			while (m_size > 0) removeFirst();

			m_size = 0;
			m_begin = nullptr;
			m_end = nullptr;

			ResetSavedNode();
		};

	protected:
		size_t m_size = 0;
		ListNode<T>* m_begin = nullptr;
		ListNode<T>* m_end = nullptr;

		ListNode<T>* m_last = nullptr;
		size_t m_lastIndex = -1;
		bool m_savedNode = false;

		bool m_sorted = true;
	};

	template<typename T>
	class Vector
	{
	public:
		Vector() {};
	private:
		T* dataArray;
	};
};