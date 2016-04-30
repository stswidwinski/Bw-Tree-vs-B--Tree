// Author: Alexander Thomson (thomson@cs.yale.edu)
// Modified by: Christina Wallin (christina.wallin@yale.edu)
// Modified by: Kun Ren (kun.ren@yale.edu)


#include "bench/txn_processor.h"
#include <stdio.h>
#include <set>
#include <algorithm>

//#include "txn/lock_manager.h" don't need lock manager since no locks

// Thread & queue counts for StaticThreadPool initialization.
#define THREAD_COUNT 8

TxnProcessor::TxnProcessor(CCMode mode)
    : mode_(mode), tp_(THREAD_COUNT), next_unique_id_(1) {
  storage_ = new Storage();
  storage_->InitStorage();

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
  delete storage_;
}

void TxnProcessor::NewTxnRequest(Txn* txn) {
  // Atomically assign the txn a new number and add it to the incoming txn
  // requests queue.
  mutex_.Lock();
  txn->unique_id_ = next_unique_id_;
  next_unique_id_++;
  txn_requests_.Push(txn);
  mutex_.Unlock();
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
  switch (mode_) {
    case SERIAL:                 RunSerialScheduler(); break;
    case OCC:                    RunOCCScheduler(); break;
    case P_OCC:                  RunOCCParallelScheduler(); break;
  }
}

void TxnProcessor::RunSerialScheduler() {
  Txn* txn;
  while (tp_.Active()) {
    // Get next txn request.
    if (txn_requests_.Pop(&txn)) {
      // Execute txn.
      ExecuteTxn(txn);

      // Commit/abort txn according to program logic's commit/abort decision.
      if (txn->Status() == COMPLETED_C) {
        ApplyWrites(txn);
        txn->status_ = COMMITTED;
      } else if (txn->Status() == COMPLETED_A) {
        txn->status_ = ABORTED;
      } else {
        // Invalid TxnStatus!
        DIE("Completed Txn has invalid TxnStatus: " << txn->Status());
      }

      // Return result to client.
      txn_results_.Push(txn);
    }
  }
}


void TxnProcessor::ExecuteTxn(Txn* txn) {

  // Get the start time
  txn->occ_start_time_ = GetTime();

  // Read everything in from readset.
  for (set<TKey>::iterator it = txn->readset_.begin();
       it != txn->readset_.end(); ++it) {
    // Save each read result iff record exists in storage.
    TValue result;
    if (storage_->Read(*it, &result))
      txn->reads_[*it] = result;
  }

  // Also read everything in from writeset.
  for (set<TKey>::iterator it = txn->writeset_.begin();
       it != txn->writeset_.end(); ++it) {
    // Save each read result iff record exists in storage.
    TValue result;
    if (storage_->Read(*it, &result))
      txn->reads_[*it] = result;
  }

  // Execute txn's program logic.
  txn->Run();

  // Hand the txn back to the RunScheduler thread.
  completed_txns_.Push(txn);
}

void TxnProcessor::ExecuteTxnParallel(Txn* txn) {

  // Get the start time
  txn->occ_start_time_ = GetTime();

  // Read everything in from readset.
  for (set<TKey>::iterator it = txn->readset_.begin();
       it != txn->readset_.end(); ++it) {
    // Save each read result iff record exists in storage.
    TValue result;
    if (storage_->Read(*it, &result))
      txn->reads_[*it] = result;
  }

  // Also read everything in from writeset.
  for (set<TKey>::iterator it = txn->writeset_.begin();
       it != txn->writeset_.end(); ++it) {
    // Save each read result iff record exists in storage.
    TValue result;
    if (storage_->Read(*it, &result))
      txn->reads_[*it] = result;
  }

  // Execute txn's program logic.
  txn->Run();

  set<Txn*> localSet;
  active_set_mutex_.Lock();
  localSet = active_set_.GetSet();
  active_set_.Insert(txn);
  active_set_mutex_.Unlock();

  // validation
  set<TKey> uni;
  set_union(txn->writeset_.begin(),txn->writeset_.end(),txn->readset_.begin(),txn->readset_.end(), std::inserter(uni, uni.begin()));
  std::set<TKey>::iterator it;
  bool isValid = true;
  for (it = uni.begin(); it != uni.end(); ++it) {
    double lastUpdate = storage_->Timestamp(*it);
    if (lastUpdate > txn->occ_start_time_) {
      isValid = false;
      break;
    }
  }

  if(isValid) {
      std::set<Txn*>::iterator iter;
      for (iter = localSet.begin(); iter != localSet.end(); ++iter) {
        set<TKey> readInts, writeInts, write2Ints;
        // check if txn's write set intersects with t's read or write set
        set_intersection(txn->writeset_.begin(),txn->writeset_.end(),(*iter)->readset_.begin(),(*iter)->readset_.end(), std::inserter(readInts, readInts.begin()));
        if(!writeInts.empty()) {
            isValid = false;
            break;
        }
        set_intersection(txn->writeset_.begin(),txn->writeset_.end(), (*iter)->writeset_.begin(), (*iter)->writeset_.end(), std::inserter(writeInts, writeInts.begin()));
        if(!readInts.empty()) {
            isValid = false;
            break;
        }

        // check if txn's read set intersects with t's write set
        set_intersection(txn->readset_.begin(),txn->readset_.end(),(*iter)->writeset_.begin(),(*iter)->writeset_.end(), std::inserter(write2Ints, write2Ints.begin()));
        if(!write2Ints.empty()) {
            isValid = false;
            break;
        }
      }
  }

  if (isValid) {
    ApplyWrites(txn);
    active_set_mutex_.Lock();
    active_set_.Erase(txn);
    active_set_mutex_.Unlock();
    txn->status_ = COMMITTED;
    // Return result to client.
    txn_results_.Push(txn);
  } else {
    active_set_mutex_.Lock();
    active_set_.Erase(txn);
    active_set_mutex_.Unlock();
    cleanup_txn(txn);
    restart_txn(txn);
  }
}

void TxnProcessor::ApplyWrites(Txn* txn) {
  // Write buffered writes out to storage.
  for (map<TKey, TValue>::iterator it = txn->writes_.begin();
       it != txn->writes_.end(); ++it) {
    storage_->Write(it->first, it->second, txn->unique_id_);
  }
}

void TxnProcessor::cleanup_txn(Txn *txn) {
    txn->reads_.clear();
    txn->writes_.clear();
    txn->status_ = INCOMPLETE;
}

void TxnProcessor::restart_txn(Txn *txn) {
    mutex_.Lock();
    txn->unique_id_ = next_unique_id_;
    next_unique_id_++;
    txn_requests_.Push(txn);
    mutex_.Unlock();
}

void TxnProcessor::RunOCCParallelScheduler() {
  // CPSC 438/538:
  //
  // Implement this method! Note that implementing OCC with parallel
  // validation may need to create another method, like
  // TxnProcessor::ExecuteTxnParallel.
  // Note that you can use active_set_ and active_set_mutex_ we provided
  // for you in the txn_processor.h
  //
  // [For now, run serial scheduler in order to make it through the test
  // suite]

  Txn* txn;
  while (tp_.Active()) {
    // Start processing the next incoming transaction request.
    if (txn_requests_.Pop(&txn)) {
      tp_.RunTask(new Method<TxnProcessor, void, Txn*>(
            this,
            &TxnProcessor::ExecuteTxnParallel,
            txn));
    }
  }
}
