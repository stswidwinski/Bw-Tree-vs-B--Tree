/*
	triple.h

	Triple is just a single <Key, Value, Rec> triple used in:
		Returning PID, Node, and Values when searching/inserting/updating records
*/

#ifndef _TRIPLE_H_
#define _TRIPLE_H_

template <typename Key, typename Value, typename Record>
class Triple {
	public:
		Triple() {};
		Triple(Key k, Value v, Record r) {
			key = k;
			value = v; 
			record = r;
		}

		Key key;
		Value value;
		Record record;
};

#endif