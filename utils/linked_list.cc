/*
	linked_list.h

	ListElement is a container for elements stored in the linked list
	that allows us to link them together.
*/

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <iostream>

template <typename Element>
class LinkedList {
	public:
		LinkedList() {
			head_ = nullptr;
			tail_ = nullptr;
		};

		// a cut-down version of standard pop. We do NOT
		// free any memory. It's too slow and memory leaks are fine
		// in this context.
		Element* pop() {
			if(tail_ == nullptr) {
				return nullptr;
			}

			Element* result = tail_->contents;
			tail_ = tail_->next;
			return result;
		}

		// append to the head_. This is standard.
		void push (Element* elem) {
			ListElement* listElem = new ListElement(elem);
			
			// first element or any next element.
			if (head_ == nullptr) {
				head_ = listElem;
				tail_ = head_;
			} else {
				head_->next = listElem;
				head_ = listElem;
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

		// standard head_ and tail_ of the list
		ListElement* head_;
		ListElement* tail_;
};

#endif