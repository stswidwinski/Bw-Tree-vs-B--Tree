/*
	linked_list.h

	ListElement is a container for elements stored in the linked list
	that allows us to link them together.
*/

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

template <typename Element>
class LinkedList {
	public:
		LinkedList() {
			head = nullptr;
			tail = nullptr;
		};

		// a cut-down version of standard pop. We do NOT
		// free any memory. It's too slow and memory leaks are fine
		// in this context.
		Element* pop() {
			if(tail == nullptr)
				return nullptr;

			Element* result = tail->contents;
			tail = tail->next;
			return result;
		}

		// append to the head. This is standard.
		void push (Element* elem) {
			ListElement* listElem = new ListElement(elem);
			
			// first element or any next element.
			if (head == nullptr) {
				head = listElem;
				tail = head;
			} else {
				head->next = listElem;
				head = listElem;
			}
		}

		~LinkedList(){}

	private:
		// container for elements stored in the linked list that 
		// allows us to link them together
		class ListElement {
			public:
				ListElement(Element* content) {
					contents = content;
					next = nullptr;
				}

				ListElement* next;
				Element* contents;
		};

		// standard head and tail of the list
		ListElement* head;
		ListElement* tail;


};

#endif