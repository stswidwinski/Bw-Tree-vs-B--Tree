/*
	node.h

	Node represents a node within the Bw-Tree. Hence, it may be
		a data node, an index node or one of the delta nodes.
		This class is the abstract class from which all nodes 
		inherit.
*/

#ifndef _NODE_H_
#define _NODE_H_

#include "utils/common.h"

// Nodes can have multiple types
enum NodeType {
  INDEX,
  DATA,
  DATA_CONS,
  DELTA_INSERT,
  DELTA_UPDATE,
  DELTA_SPLIT,
  DELTA_INDEX_SPLIT
};

class Node {
	public:
		Node(NodeType type) : type_{type} {};
		
		// returns the type of the node.
		NodeType getType() { 
			return type_;
		}

		virtual ~Node(){};

		virtual PID nextPid(int key) {
			DIE("nextPID called on wrong node type");
			return -1;
		}

		virtual bool doSplit() {
			DIE("doSplit called on wrong node type");
			return -1;
		}

		// returns the PID of the next page to search.
		// NOTE: data node does not implement this method.
		// virtual int find(int key) {
		// 	return -1;
		// };
	protected:
		NodeType type_;
		
};

#endif
