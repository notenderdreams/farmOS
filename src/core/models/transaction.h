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

	static const char* toStr(TransactionType t) 
	{
		switch (t)
		{
		case TransactionType::BUY:
			return "BUY";
		case TransactionType::SELL:
			return "SELL";
		case TransactionType::SALARY:
			return "SALARY";
		case TransactionType::BILLS:
			return "BILLS";
		default:
			return "UNKNOWN";
		}
	}
	
	static const char* toStr(TransactionStatus s)
	{
		switch (s)
		{
		case TransactionStatus::COMPLETED:
			return "COMPLETED";
		case TransactionStatus::PENDING:
			return "PENDING";
		case TransactionStatus::CANCELLED:
			return "CANCELLED";
		default:
			return "UNKNOWN";
		}
	}
	
	static const char* toStr(TransactionEntityType e)
	{
		switch (e)
		{
		case TransactionEntityType::ANIMAL:
			return "ANIMAL";
		case TransactionEntityType::EMPLOYEE:
			return "EMPLOYEE";
		case TransactionEntityType::GOODS:
			return "GOODS";
		default:
			return "UNKNOWN";
		}
	}
	static const char* toStr(TransactionDirection d)
	{
		switch (d)
		{
		case TransactionDirection::IN:
			return "IN";
		case TransactionDirection::OUT:
			return "OUT";
		default:
			return "UNKNOWN";
		}
	}
};


namespace tx{
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