#pragma once 
#include <string>

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
	size_t tid ;
	TransactionType type;
	TransactionDirection direction;

	double amount;
	
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