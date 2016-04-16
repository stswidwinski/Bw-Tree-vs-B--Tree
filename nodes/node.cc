/*
	node.h

	Node represents a node within the Bw-Tree. Hence, it may be
		a data node, an index node or one of the delta nodes.
		This class is the abstract class from which all nodes 
		inherit.
*/

#ifndef _NODE_H_
#define _NODE_H_

// Nodes can have multiple types
enum NodeType {
  INDEX = 0,
  DATA = 1,
  DELTA_INSERT = 2,
  DELTA_DELETE = 3,
  DELTA_UPDATE = 4,
};

class Node {
	public:
		Node(NodeType type) : type_{type} {};
		
		// returns the type of the node.
		NodeType getType() { 
			return type_;
		}

		virtual ~Node(){};

		// returns the PID of the next page to search.
		// NOTE: data node does not implement this method.
		// virtual int find(int key) {
		// 	return -1;
		// };
	protected:
		NodeType type_;
		
};

#endif