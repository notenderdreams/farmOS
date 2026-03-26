#pragma once
#include "cli/cli.h"

int inventoryAdd(const Args& args);
int inventoryList(const Args& args);
int inventoryShow(const Args& args);
int inventoryUpdateQuantity(const Args& args);
int inventoryDelete(const Args& args);
