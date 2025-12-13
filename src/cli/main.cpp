#include <iostream>
#include "cli.h"
#include "core/farm_os.h"

int deletePerson(const Args& args)
{
    std::string id;
    loadArg(id, 0, "id");
    std::cout << "Deleting person " << id << "\n";
    return 0;
}

int addPerson(const Args& args)
{
    std::string name;
    loadArg(name, 0, "name");
    std::cout << "Adding person " << name << "\n";
    return 0;
}

void setupCli(CLI& cli) {
    cli.registerModule({
        "person",
        "Manage persons",
        {
            { "delete", { "Delete a person",  deletePerson }},
            { "add", {"Add a person", addPerson}}
        }
    });
}

int main(int argc, char** argv) {
    CLI cli("farmOS");
    setupCli(cli);
    return cli.run(argc, argv);
}
