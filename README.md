Readme

A short description of what we need to do.

The system we are writing consists of three main parts.

1) Bw-Tree
	The Bw-Tree is similar conceptually to a B+-tree. Hence, we have two main types of nodes -- internal and external. Internal nodes (also called index nodes) contain an array of <Key, Pointer> pairs and a maxKey (or minKey) variable that are used to determine which path down the tree to take (Please refer to the B-Tree paper linked to by prof. Abadi for details). Hence, all nodes excluding leafs are internal nodes. All internal nodes but for the root must have between k+1 and 2k elements in the array. If they become too big or too small, a split occurs (Please refer to the Bw-Tree paper and pseudo code for details). External nodes carry sorted arrays of pointers to data (in Bw-Tree those are logical pointers -- for instance integers -- that can be mapped to physical addresses using memory map).

	Major Note: All links in the Bw-Tree are logical (PID)!

2) Memory Map
	Please see mem_map.h

3) Memory Manager
	Please see mem_manager.h

I have written pseudo code where I could indicating what I think we should do and how we should do it (more or less). I'll continue doing that, but please feel free to change anything and comment on anything. Hope it helps.