#pragma once 
#include <string>
#include <stdexcept>
#include "core/types.h"

enum class TransactionType 
{
	BUY,
	SELL,
	SALARY,
	BILLS
};

enum class TransactionDirection 
{
	IN,
	OUT
};

enum class TransactionStatus 
{
	COMPLETED,
	PENDING,
	CANCELLED,
};

enum class TransactionEntityType 
{
	ANIMAL,
	EMPLOYEE,
	GOODS
};

struct Transaction
{
	i64 tid ;
	TransactionType type;
	TransactionDirection direction;

	f64 amount;
	
	TransactionEntityType entity_type;
	std::string entity_id;

	std::string description;
	std::string date; // YYYY-MM-DD
	TransactionStatus status = TransactionStatus::COMPLETED;


};


namespace tx{
	extern const char *TransactionTypeStrs[];
    extern const char *TransactionStatusStrs[];
    extern const char *TransactionEntityTypeStrs[];
    extern const char *TransactionDirectionStrs[];

    static const char* toStr(TransactionType t) 
    {
        return TransactionTypeStrs[static_cast<int>(t)];
    }
    
    static const char* toStr(TransactionStatus s)
    {
        return TransactionStatusStrs[static_cast<int>(s)];
    }
    
    static const char* toStr(TransactionEntityType e)
    {
        return TransactionEntityTypeStrs[static_cast<int>(e)];
    }

    static const char* toStr(TransactionDirection d)
    {
        return TransactionDirectionStrs[static_cast<int>(d)];
    }
	
	inline TransactionType stt(const std::string& s)
	{
		if(s == "BUY") return TransactionType::BUY;
		if(s == "SELL") return TransactionType::SELL;
		if(s == "SALARY") return TransactionType::SALARY;
		if(s == "BILLS") return TransactionType::BILLS;
		throw std::runtime_error("Invalid transaction type string: " + s);
	}

	inline TransactionStatus sts(const std::string& s)
	{
		if(s == "COMPLETED") return TransactionStatus::COMPLETED;
		if(s == "PENDING") return TransactionStatus::PENDING;
		if(s == "CANCELLED") return TransactionStatus::CANCELLED;
		throw std::runtime_error("Invalid transaction status string: " + s);
	}

	inline TransactionEntityType ste(const std::string& s)
	{
		if(s == "ANIMAL") return TransactionEntityType::ANIMAL;
		if(s == "EMPLOYEE") return TransactionEntityType::EMPLOYEE;
		if(s == "GOODS") return TransactionEntityType::GOODS;
		throw std::runtime_error("Invalid transaction entity type string: " + s);
	}

	inline TransactionDirection stdi(const std::string& s)
	{
		if(s == "IN") return TransactionDirection::IN;
		if(s == "OUT") return TransactionDirection::OUT;
		throw std::runtime_error("Invalid transaction direction string: " + s);
	}
}