/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "utils/common.h"
#include "nodes/node.cc" 
#include "utils/pair.h"
#include "utils/triple.h"

typedef unsigned char byte;

class DataNode : public Node {
	public:
		// full initialization
		DataNode(int dataLength,
			PID sidePter,
			int lowKey,
			int highKey);

		// initialization for buffer.
		DataNode();

        ~DataNode();
        
		// for initialization of node after retrieving from
		// buffer pool.
		void setVariables(Pair<int, byte*>* data,
			int dataLength,
			PID sidePter,
			int lowKey,
			int highKey);

		// Points record to the byte corresponding to key. On error
		// this is not guaranteed.
		//
		// sets record to point to the byte retrieved.
		// returns 0 on success and appropriate error code on error.
		// see utils for codes -- NOT_FOUND and OVER_HIGH
		int pointToRecord(int key, byte ** record);

		// returns true if we should split. False otherwise.
		bool doSplit() override;

		// Get PID of sibling if searched value is too great
		PID getSibling() {
			return sidePter_;
		}

		int getSplittingKey();

		int getHighKey();

		void sort();

		void setSidePter(PID sidePtr) {
                    sidePter_ = sidePtr;    
                };

		void setLowKey(int lowKey) {
                    lowKey_ = lowKey;    
                };

		void setHighKey(int highKey) {
                    highKey_ = highKey;    
                };

		int getLowKey() {
                    return lowKey_; 
                };

		void insertChainData(int key, byte *ptr) {
                    // linear search to make sure it's not in the damn thing
                    for (int i = 0; i < dataLength_; i++) {
                        if (key == data_[i].key) {
                            return;
                        }
                    }

                    data_[dataLength_].key = key;
                    data_[dataLength_].value = ptr;
                    dataLength_++;
                };

                byte* getValue(int key) {
                        // binary search on the data_ array.
                        int left = 0, right = dataLength_, middle = 0, midVal = 0;
                        while(left <= right) {
                                middle = left + (right-left)/2;
                                midVal = data_[middle].key;

                                if(midVal > key) {
                                        right = middle - 1;
                                } else if (midVal < key) {
                                        left = middle + 1;
                                } else {
                                        break;
                                }
                        }

                        // if nothing found, return null. Else, return what is found.
                        return left < right ? data_[middle].value : nullptr;
                }

void merge(int low,int mid,int high,int dataLength)
{
 int h,i,j,b[dataLength],k;
 h=low;
 i=low;
 j=mid+1;

 while((h<=mid)&&(j<=high))
 {
  if(data_[h].key <= data_[j].key)
  {
   b[i]=data_[h].key;
   h++;
  }
  else
  {
   b[i]=data_[j].key;
   j++;
  }
  i++;
 }
 if(h>mid)
 {
  for(k=j;k<=high;k++)
  {
   b[i]=data_[k].key;
   i++;
  }
 }
 else
 {
  for(k=h;k<=mid;k++)
  {
   b[i]= data_[k].key;
   i++;
  }
 }
 for(k=low;k<=high;k++)  data_[k].key=b[k];
}

void merge_sort(int low,int high, int dataLength)
{
 int mid;
 if(low<high)
 {
  mid = low + (high-low)/2; //This avoids overflow when low, high are too large
  merge_sort(low,mid, dataLength);
  merge_sort(mid+1,high, dataLength);
  merge(low,mid,high, dataLength);
 }
}


                void mergesort() {
                        merge_sort(0, dataLength_-1, dataLength_);
                        // binary search on the data_ array.
//                        int key = 2;
//                        key++;
                }

	private:
		// the lowest and highest key that can be stored.
		// must be consistent with the index nodes above it.
		int lowKey_;
		int highKey_;
		// pointer to node's immediate right sibling on the same level.
		PID sidePter_;
		// array of records <Key, PID>
		Pair<int, byte*>* data_;
		// length of the data array
		int dataLength_;
};

#endif
