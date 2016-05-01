#ifndef _TXN_H_
#define _TXN_H_

#include "utils/common.h"

enum TxnType {
  INSERT,
  UPDATE,
  READ
};

enum TxnStatus {
  SUCCESS,
  FAILURE
};

class Txn {
  public:
    Txn(TxnType type, int key, byte* value = nullptr) 
    : type_(type),
    key_(key),
    value_(value)
    {}

    TxnType type_;
    int key_;
    byte* value_;
    TxnStatus status_;
};

#endif