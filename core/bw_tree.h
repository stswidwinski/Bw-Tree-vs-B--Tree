/*
	bw_tree.h

	Bw-tree is purely logical and does not exist as an explicit structure.
	Hence, we only need to know the root PID and have a collection of
	functions that do searches, inserts etc. 
*/

#ifndef _BW_TREE_H_
#define _BW_TREE_H_

#include "core/mem_map.h"
#include "core/mem_manager.h"
#include "utils/common.h"
#include "utils/pair.h"
#include "utils/triple.h"
#include "nodes/node.cc"
#include "nodes/data_node.h"
#include "nodes/delta_node.h"
#include "nodes/index_node.h"

typedef unsigned char byte;

class BwTree {
	public:
		BwTree();

		~BwTree() {
            // todo fix the mem management... i.e. searchArray and root
			delete map_;
		}
		
		// get the PID of the next page to search
		byte* get(int key, MemoryManager* man);

        // add a delta record with a new value for an existing key
        int update(int key, byte *value, MemoryManager* man);
        // inserts a new delta record for a previously not existing key
        int insert(int key, byte *value, MemoryManager* man);

        void populate(DataNode * oldPt, DataNode* newPt, int kp, MemoryManager * man);
        void populate(IndexNode * oldPt, IndexNode* newPt, int kp, MemoryManager * man);
        void consolidate(Node* top, Node* bot, PID topPID, MemoryManager * man);

        PID getRootPID() {
        	return rootPid_;
        }

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

			void insert(byte payload)
				
			Steps:
				0) Obtain the key corresponding to payload. This would be a hashfunction, probably within
				the mem_map
				1) Traverse the tree to find the correct page, just as above. There is pretty much no
				difference in this regard
				2) Create (ask from mem_manager) for a delta-insert record. Propagate will all information.
				This record now points to the page PID we obtained from the end of the former step.
				3) Use CAS to update the memory map. Now, the PID for the page returned from step 1 should
				point to the delta record. 
			NOTICE: CAS can fail and the failure case is not well described in the whole paper. I presume we can 
			just restart the transaction. The only CAS failure case discussed is that happening during consolidation.

			// I'm not positive how to write even invoke this (?)
			void consolidate()

			Consolidation is triggered when the length of a delta chain exceeds predetermined length. It is triggered 
			and carried out by any thread (Accessing, updating etc.). The consolidation means creating a new 
			node/page and updating it with all the changes from delta nodes directly preceeding the page. Then, the
			newly created page is installed into memory map using CAS. On failure of CAS, just abandon this task.
			This means that the original task that the thread was on carries is still executed.

			void finishSMO()

			Since we are only implementing splitting and not merging, finishing SMO means finishing a split. In this
			scenario, one must create an index-delta node, propagate it with information and use CAS to install it in
			place of the current parent in memory map. This delta record will point to the current parent.

			CAS:
			GCC has __sync_val_compare_and_swap() built in. I think we could use it. Better ideas?
		*/

	private:
		MemoryMap<Node>* map_;
		PID rootPid_;

		// attempt to find the node corresponding to key. Returns a 
		// triple with the pid of the node, node* pointer to the correct
		// sub-node, and byte* pointer to the record itself.
		//
		// If record does not exist, byte* will be a nullptr, Node* will
		// point to the base page last checked and PID will point to
		// the page in which the record should be if it existed.
		//
		// If record does exist, byte* will point to the current value
		// of the record, pid will be the PID of the page containing the
		// record and Node* will point to the node specifying the record
		// (this can be delta or data node)
		Triple<PID, Node*, byte*> 
		findNode(int key, MemoryManager* man);
		
		void consolidate(Node* chainStart, PID chainStartPID,
			MemoryManager* man);

		// attempts to split the node. Returns the pointer to the next node
		// to traverse both in case of failure and success.
		Node* split(PID ppid, PID pid, MemoryManager* man, DataNode* toSplit, Node* firstInChain);

		Node* split(PID ppid, PID pid, MemoryManager* man, IndexNode* toSplit, Node* firstInChain);

};

#endif
