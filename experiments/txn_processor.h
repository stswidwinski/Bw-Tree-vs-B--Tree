// Author: Alexander Thomson (thomson@cs.yale.edu)
// Modified by: Christina Wallin (christina.wallin@yale.edu)
// Modified by: Kun Ren (kun.ren@yale.edu)
//
// Cut down to use in final project: Stan Swidwinski (stanislaw.swidwinski@yale.edu)

#ifndef _TXN_PROCESSOR_H_
#define _TXN_PROCESSOR_H_

class TxnProcessor;

#include "experiments/txn.h"
#include "experiments/atomic.h"
#include "experiments/static_thread_pool.h"
#include "core/bw_tree.h"
#include "core/mem_manager.h"

using std::vector;

class TxnProcessor {
 public:
  // The TxnProcessor's constructor starts the TxnProcessor running in the
  // background.
  explicit TxnProcessor();

  // The TxnProcessor's destructor stops all background threads and deallocates
  // all objects currently owned by the TxnProcessor, except for Txn objects.
  ~TxnProcessor();

  // Registers a new txn request to be executed by the TxnProcessor.
  // Ownership of '*txn' is transfered to the TxnProcessor.
  void NewTxnRequest(Txn* txn);

  // Returns a pointer to the next COMMITTED or ABORTED Txn. The caller takes
  // ownership of the returned Txn.
  Txn* GetTxnResult();

  // Main loop implementing all concurrency control/thread scheduling.
  void RunScheduler();
  
  static void* StartScheduler(void * arg);
  
 private:

  BwTree* tree_;

  // Performs all reads required to execute the transaction, then executes the
  // transaction logic.
  void ExecuteTxn(Txn* txn, MemoryManager* man);

  // Thread pool managing all threads used by TxnProcessor.
  StaticThreadPool tp_;

  // Queue of incoming transaction requests.
  AtomicQueue<Txn*> txn_requests_;

  // Queue of transaction results (already committed or aborted) to be returned
  // to client.
  AtomicQueue<Txn*> txn_results_;

};

#endif  // _TXN_PROCESSOR_H_

