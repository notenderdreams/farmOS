#include <iostream>
#include "cli.h"
#include "widgets.h"
#include "core/models/transaction.h"

int deletePerson(const Args& args)
{
    std::string id;
    loadArg(id, 0, "id");
    std::cout << "Deleting person " << id << "\n";
    return 0;
}

int addPerson(const Args& args)
{
    // loadArg(name, 0, "name");
    std::string name = wx::lineInput<std::string>("Enter name: ");
    std::cout << "Adding person " << name << "\n";

    TransactionType tt = wx::selectInput<TransactionType>(
        "Transaction type ? ",
        tx::TransactionTypeStrs,4,
        tx::stt
    );
    std::cout<<tx::toStr(tt);
    return 0;
}

void setupCli(CLI& cli) {
	REG_MOD(cli, person, "Manage persons")
    {
		ADD_CMD(add, "Add a person", addPerson);
		ADD_CMD(delete, "Delete a person", deletePerson);
	}
}

int main(int argc, char** argv) {
    CLI cli("farmOS");
    setupCli(cli);
    return cli.run(argc, argv);
}
