#include "transaction.h"

namespace tx {
    const char *TransactionTypeStrs[] = {"BUY", "SELL", "SALARY", "BILLS"};
    const char *TransactionStatusStrs[] = {"COMPLETED", "PENDING", "CANCELLED"};
    const char *TransactionEntityTypeStrs[] = {"ANIMAL", "EMPLOYEE", "GOODS"};
    const char *TransactionDirectionStrs[] = {"IN", "OUT"};

	TransactionDirection typeToDir(TransactionType t)
	{
		switch (t)
		{
		case TransactionType::BILLS :
		case TransactionType::BUY :
		case TransactionType::SALARY :
			return TransactionDirection::OUT;
		case TransactionType::SELL:
			return TransactionDirection::IN;
		}
	}
}