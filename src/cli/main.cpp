#include <iostream>
#include "cli.h"
#include "app_state.h"
#include "handle_tx.h"

int main(int argc, char** argv) {
    CLI cli("farmos");
    
    cli.registerState(new AppState("../../farm.db"));
    
    REG_MOD(cli, tx, "Transaction management") {
        ADD_CMD(add, "Add a new transaction", txAdd);
        ADD_CMD(list, "List all transactions", txList);
        ADD_CMD(show, "Show transaction details", txShow);
        ADD_CMD(status, "Update transaction status", txUpdateStatus);
    }
    
    return cli.run(argc, argv);
}