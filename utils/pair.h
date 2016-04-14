/*
	pair.h

	Pair is just a single <Key, Value> pair used in:
		1) Data nodes for storing data
		2) Delta nodes for storing updates/inserts
		3) Index nodes for storing Key, Pter pairs.
*/

#ifndef _PAIR_H_
#define _PAIR_H_

template <typename Key, typename Value>
class Pair {
	public:
		Pair() {};
		Pair(Key k, Value v) {
			key = k;
			value = v; 
		}

		Key key;
		Value value;
};

#endif