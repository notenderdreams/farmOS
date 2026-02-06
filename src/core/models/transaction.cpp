#include "transaction.h"

namespace tx {
    const char *TransactionTypeStrs[] = {"BUY", "SELL", "SALARY", "BILLS"};
    const char *TransactionStatusStrs[] = {"COMPLETED", "PENDING", "CANCELLED"};
    const char *TransactionEntityTypeStrs[] = {"ANIMAL", "EMPLOYEE", "GOODS"};
    const char *TransactionDirectionStrs[] = {"IN", "OUT"};
}