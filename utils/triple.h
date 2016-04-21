/*
	triple.h

	Triple is just a single <Key, Node, Rec> triple used in:
		Returning PID, Node, and Values when searching/inserting/updating records
*/

#ifndef _TRIPLE_H_
#define _TRIPLE_H_

template <typename Key, typename Node, typename Record>
class Triple {
	public:
		Triple() {};
		Triple(Key k, Node v, Record r) {
			key = k;
			node = v; 
			record = r;
		}

		Key key;
		Node node;
		Record record;
};

#endif