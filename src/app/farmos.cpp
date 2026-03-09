#include <iostream>
#include "cli.h"
#include "app_state.h"
#include "handle_tx.h"
#include "handle_animal.h"
#include "handle_inventory.h"


int main(int argc, char** argv) {
    CLI cli("farmos");

    cli.registerState(new AppState("../../farm.db"));

    REG_MOD(cli, tx, "Transaction management") {
        ADD_CMD(add,    "Add a new transaction",     txAdd);
        ADD_CMD(list,   "List all transactions",     txList);
        ADD_CMD(show,   "Show transaction details",  txShow);
        ADD_CMD(status, "Update transaction status", txUpdateStatus);
    }

    REG_MOD(cli, animal, "Animal management") {
        ADD_CMD(buy,    "Buy an animal",             animalBuy);
        ADD_CMD(sell,   "Sell an animal",            animalSell);
        ADD_CMD(list,   "List all animals",          animalList);
        ADD_CMD(show,   "Show animal details",       animalShow);
        ADD_CMD(status, "Update animal status",      animalUpdateStatus);
        ADD_CMD(delete, "Delete an animal record",   animalDelete);
    }

    REG_MOD(cli, inventory, "Inventory management") {
        ADD_CMD(add,    "Add item & log purchase",         inventoryAdd);
        ADD_CMD(list,   "List all items",                  inventoryList);
        ADD_CMD(show,   "Show item details",               inventoryShow);
        ADD_CMD(update, "Update quantity & log transaction", inventoryUpdateQuantity);
        ADD_CMD(delete, "Delete an item",                  inventoryDelete);
    }


    return cli.run(argc, argv);
}
