// Author: Alexander Thomson (thomson@cs.yale.edu)
// Modified by: Christina Wallin (christina.wallin@yale.edu)
// Modified by: Kun Ren (kun.ren@yale.edu)
//
// Modified and simplified for final project: Stan Swidwinski

#include "experiments/txn_processor.h"

// Thread & queue counts for StaticThreadPool initialization.
#define THREAD_COUNT 8

TxnProcessor::TxnProcessor()
    : tp_(THREAD_COUNT) {
  tree_ = new BwTree();
  
  // Start 'RunScheduler()' running.
  cpu_set_t cpuset;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  CPU_SET(1, &cpuset);       
  CPU_SET(2, &cpuset);
  CPU_SET(3, &cpuset);
  CPU_SET(4, &cpuset);
  CPU_SET(5, &cpuset);
  CPU_SET(6, &cpuset);  
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  pthread_t scheduler_;
  pthread_create(&scheduler_, &attr, StartScheduler, reinterpret_cast<void*>(this));  
}

void* TxnProcessor::StartScheduler(void * arg) {
  reinterpret_cast<TxnProcessor *>(arg)->RunScheduler();
  return NULL;
}

TxnProcessor::~TxnProcessor() {
}

void TxnProcessor::NewTxnRequest(Txn* txn) {
  // Add the to the incoming txn requests queue.
  txn_requests_.Push(txn);
}

Txn* TxnProcessor::GetTxnResult() {
  Txn* txn;
  while (!txn_results_.Pop(&txn)) {
    // No result yet. Wait a bit before trying again (to reduce contention on
    // atomic queues).
    sleep(0.000001);
  }
  return txn;
}

void TxnProcessor::RunScheduler() {
  Txn* txn;
  while(tp_.Active()) {
    // start processing the next incoming transaction request
    if(txn_requests_.Pop(&txn)) {
      // use a new thread. Enqueue in thread pool.
      // do NOT specify the memory manager for now. The thread
      // will insert the memory manager.
      tp_.RunTask(new Method<TxnProcessor, void, Txn*, MemoryManager*>(
        this, 
        &TxnProcessor::ExecuteTxn,
        txn,
        nullptr));
    }
  }
}

void TxnProcessor::ExecuteTxn(Txn* txn, MemoryManager* man) {
  switch(txn->type_) {
    case READ:
      txn->status_ = (tree_->get(txn->key_, man) == nullptr) ?
        FAILURE : SUCCESS;
      break;
    case UPDATE:
      txn->status_ = tree_->update(txn->key_, txn->value_, man) ?
        FAILURE : SUCCESS;
      break;
    case INSERT:
      txn->status_ = tree_->insert(txn->key_, txn->value_, man) ?
        FAILURE : SUCCESS;
        break;
  }

  txn_results_.Push(txn);
}