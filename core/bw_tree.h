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

	//private:
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
		//
		// startPID is the PID of the subtree that we will start the search from.
		// Used by the insert and update functions if CAS failes. In case of 
		// startPID not being given, the search starts from the root.
		Triple<PID, Node*, byte*> 
		findNode(int key, MemoryManager* man, PID startPID = 0);
		
		// attempts to split the node. Returns the pointer to the next node
		// to traverse both in case of failure and success.
		void split(PID ppid, PID pid, MemoryManager* man, DataNode* toSplit, Node* firstInChain);

		void split(PID ppid, PID pid, MemoryManager* man, IndexNode* toSplit, Node* firstInChain);

};

#endif
