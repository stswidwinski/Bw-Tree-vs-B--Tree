// Author: Alexander Thomson (thomson@cs.yale.edu)
// Modified by: Kun Ren (kun.ren@yale.edu)

#include "experiments/txn_processor.h"
#include "experiments/experiments_common.h"
#include "experiments/txn.h"

#include <vector>
#include <iostream>

#include "utils/common.h"
#include "utils/testing.h"

// Returns a human-readable string naming of the providing mode.
string ModeToString(BwMode mode) {
  switch (mode) {
    case SERIAL:                 return " Serial   ";
    case CONCUR:                 return "Concurrent";
    default:                     return "INVALID MODE";
  }
}

class LoadGen {
 public:
  virtual ~LoadGen() {}
  virtual Txn* NewTxn() = 0;
  virtual int GetInitSize() = 0;
};

// Read Update Insert Load Generator
class RUILoadGen : public LoadGen {
  public:
    RUILoadGen(int initDbSize, int readPerc, int updatePerc, int insertPerc) 
      : dbInitSize_(initDbSize),
      readPerc_(readPerc),
      updatePerc_(updatePerc),
      insertPerc_(insertPerc)
      {
        assert(readPerc + updatePerc + insertPerc == 100);
      }

    // read, update and insert give the percentages of actions executed. 
    // They must sum to 100. 
    virtual Txn* NewTxn() {
      int txnType = rand() % 100;
      int key = rand() % UINT64_MAX;
      byte* payload = new byte[LENGTH_RECORDS];
      for(int i = 0; i < LENGTH_RECORDS; i++) {
        payload[i] = (byte) i;
      }

      if(txnType < readPerc_) {
        delete[] payload;
        return new Txn(READ, key);
      } else if (txnType < readPerc_ + updatePerc_) {
        return new Txn(UPDATE, key, payload);
      } else {
        return new Txn(INSERT, key, payload);
      }
    }

    virtual int GetInitSize() {
      return dbInitSize_;
    }

  private:
    int dbInitSize_;
    int readPerc_;
    int updatePerc_;
    int insertPerc_;
};

void Benchmark(const vector<LoadGen*>& lg) {
  // Number of transaction requests that can be active at any given time.
  int active_txns = 100;
  deque<Txn*> doneTxns;
  
  // For each MODE...
  for (BwMode mode = SERIAL;
      mode <= CONCUR;
      mode = static_cast<BwMode>(mode+1)) {
    // Print out mode name.
    cout << ModeToString(mode) << flush;
    // For each experiment, run 3 times and get the average.
    for (unsigned int exp = 0; exp < lg.size(); exp++) {
      double throughput[3];
      for (unsigned int round = 0; round < 3; round++) {

        int txn_count = 0;

        // Create TxnProcessor in next mode.
        TxnProcessor* p = new TxnProcessor(mode);
          
          // initialize the size of the tree
          int size = 0;
          while(size < lg[exp]->GetInitSize()) {
            p->NewTxnRequest(lg[exp]->NewTxn());
            size++;
          }

          // Wait for all of them to finish.
          for (int i = 0; i < size; i++) {
            Txn* txn = p->GetTxnResult();
            doneTxns.push_back(txn);
          }
          doneTxns.clear();

        // Record start time.
        double start = GetTime();

        // Start specified number of txns running.
        for (int i = 0; i < active_txns; i++)
          p->NewTxnRequest(lg[exp]->NewTxn());

        // Keep 100 active txns at all times for the first full second.
        while (GetTime() < start + 1) {
          Txn* txn = p->GetTxnResult();
          doneTxns.push_back(txn);
          txn_count++;
          p->NewTxnRequest(lg[exp]->NewTxn());
        }

        // Wait for all of them to finish.
        for (int i = 0; i < active_txns; i++) {
          Txn* txn = p->GetTxnResult();
          doneTxns.push_back(txn);
          txn_count++;
        }

        // Record end time.
        double end = GetTime();
      
        throughput[round] = txn_count / (end-start);

        doneTxns.clear();
        delete p;
      }
      
      // Print throughput
      cout << "\t" << (throughput[0] + throughput[1] + throughput[2]) / 3 << "\t" << flush;
    }
  }
  cout << endl;
}

int main(int argc, char** argv) {
  cout << "\t\t\t    Initial Tree Size" << endl;
  cout << "\t\t100\t\t10000\t\t10000000";
  cout << endl;

  cpu_set_t cs;
  CPU_ZERO(&cs);
  CPU_SET(7, &cs);
  int ret = sched_setaffinity(0, sizeof(cs), &cs);
  if (ret) {
    perror("sched_setaffinity");
    assert(false);
  }

  // the following is left as a reference.
  vector<LoadGen*> lg;

  cout << "Insert only, huge key space." << endl;
  lg.push_back(new RUILoadGen(100, 0, 0, 100));
  lg.push_back(new RUILoadGen(100, 0, 0, 100));
  lg.push_back(new RUILoadGen(100, 0, 0, 100));

  Benchmark(lg);

  for (unsigned int i = 0; i < lg.size(); i++)
    delete lg[i];
  lg.clear();
  
}

