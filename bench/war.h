/*
	index_node.h
*/

#ifndef _WAR_H_
#define _WAR_H_

class War {
	public:
	
		// no initialization. Buffer pool doesn't need it since we 
		// use setVariables any way.
		War();

		~War();
	private:
		// after a split, we can only store or attempt to find
		// a key lower than the split key. this is the key.
		int highKey_;
};

#endif
