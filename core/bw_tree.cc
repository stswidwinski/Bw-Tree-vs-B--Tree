/*
	bw_tree.cc
*/

#include "core/bw_tree.h"

// delta updates
// not much diff between this and inserts
void BwTree::update(BKey key, byte *pay, unsigned int n) {

}

/*byte* BwTree::get(int key) {
	
	currentPid = rootPid_;
	currentNode = root;

	// Get the next pid until we find a non-index node
	while((currentNode.getType() == INDEX) || (currentNode.getType() == DELTA_INDEX_SPLIT)) {
		currentPid = currentNode.nextPid(key);
		currentNode = map_.get(currentPid);
	}

	// Go through the delta nodes to get to the base page
	currentType = currentNode.getType();
	while ((currentType != DATA)){

		if (currentType == DELTA_SPLIT) {
			// @TODO have to deal with SMO -- idk 

		// Node is either insert, delete, update,
		}else if(currentNode.getNewKey() == key){
			if (currentType == DELTA_DELETE){
				// @TODO check this is correct
				return nullptr;
			} else
				return currentNode.getNewValue();
			}
		}
	}*/

	// Now currentNode is the data node, go through data node
	



	



		/*
	Interface description

	int get(key request)

	Steps:
		0) Traverse the tree to find the correct page (index nodes and index-delta-nodes)
			a) Traverse the chain of index delta records (one linked to another by PID, so
				we must repeatedly query memory map for physical pointers)
				i) If the correct pointer is found, query memory map to obtain a pointer to that node.
					Continue recursively
				ii) If the correct pointer is not found, one eventually reaches the index node.
			b) Within the index node find a key k_{i+1} such that 
				k_i < request <= k_{i+1}
			Notice that k_i might be virtual (- infty if request is lower then the smallest key). 
			Similarly, k_{i+1} might also be virtual (if request is higher then the largest key). 
			Those edge cases must be taken care of.
		1) Arrive at non-index nodes. This can be one of: update delta node, delete delta node,
			insert delta node, split delta node.
				i) If a split delta node is found and to find request one must take the side 
				pointer to reach the correct page, the search must halt and complete the SMO. 
				For more information, see below.
				ii) If any other delta node gives a PID for the requested key, return this PID.
				Do not continue searching.
				iii) If no delta node corresponds to our key, return the PID of the actual page.
				Obtaining the data corresponding to the request key now will involve a binary
				search on the contents of the page itself.
		NOTICE: If the length of delta chain has exceeding a predetermined length, we must trigger 
		consolidation. That is true for both the index and page delta chains. Details of consolidation
		later. 
		*/



