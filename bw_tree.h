/*
	bw_tree.h

	The actual implementation of Bw-tree. 
*/

#ifndef _BW_TREE_H_
#define _BW_TREE_H_

class BwTree {
	public:
		BwTree() {}

		// The interface:

		/* byte* get (int key)
			the search proceeds as in B+-Tree
			1) Traverse the B-tree as follows:
				a) If an index node 
				b) if key < minKey of node, take the first pointer. It is the K -\infty in B-link paper
					We can implement that as just an additional pointer in the node class.
				c) else, find a key k_i such that k_i-1 < k_0 <= k_i. If we have traversed the whole
					list and did not find such a key, insert a new one.
			2) Once a leaf node has been reached, attempt to get the PID corresponding to 
				key. Return it if found. Otherwise, return "did not found" -- nullptr
		*/

		// void insert (byte* data)
			//

	private:
		// the root is actualy just an index node. see node.h and index_node.h.
		Node* root_;

};

#endif