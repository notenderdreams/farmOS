#include "handle_animal.h"
#include "cli/widgets.h"
#include "cli/color.h"
#include "core/models/animal.h"
#include "core/models/transaction.h"
#include "core/animal_service.h"
#include "core/transaction_service.h"
#include "cli/cli.h"
#include "app_state.h"
#include "utils.h"
#include <iostream>


static std::string strToGender(const std::string& s) { return s; }


static void printAnimal(const AnimalRecord& r, bool show_separator = true)
{
    std::cout << color::GREEN << "ID: " << color::RESET << r.animal_id << "\n";
    std::cout << "\tType:    " << anim::toStr(r.type)    << "\n";
    std::cout << "\tBreed:   " << r.breed                << "\n";
    std::cout << "\tGender:  " << r.gender               << "\n";
    std::cout << "\tPurpose: " << anim::toStr(r.purpose) << "\n";
    std::cout << "\tStatus:  " << anim::toStr(r.status)  << "\n";
    std::cout << "\tAge:     " << r.age    << " yr\n";
    std::cout << "\tWeight:  " << r.weight << " kg\n";
    // std::cout << "\tMilk:    " << r.milk   << " L\n";
    // std::cout << "\tEggs:    " << r.eggs   << "\n";
    // std::cout << "\tMeat:    " << r.meat   << " kg\n";

    if (show_separator)
        color::printSeperator();
}


int animalBuy(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    auto* tx_service     = app_state->getTransactionService();
    if (!animal_service || !tx_service) {
        color::printError("Failed to initialize services");
        return 1;
    }

    AnimalRecord r;

    r.type = wx::selectInput<AnimalType>(
        "Animal type:",
        anim::AnimalTypeStrs, 2,
        anim::strToType
    );

    r.breed = wx::lineInput<std::string>("Breed: ");

    static const char* genderOpts[] = { "male", "female" };
    r.gender = wx::selectInput<std::string>(
        "Gender:",
        genderOpts, 2,
        strToGender
    );

    r.purpose = wx::selectInput<AnimalPurpose>(
        "Purpose:",
        anim::AnimalPurposeStrs, 3,
        anim::strToPurpose
    );

    r.age    = wx::lineInput<int>   ("Age (years): ");
    r.weight = wx::lineInput<double>("Weight (kg): ");

    double price            = wx::lineInput<double>     ("Purchase price: $");
    std::string description = wx::lineInput<std::string>("Description: ");

    try {
        animal_service->addAnimal(r);

        auto all   = animal_service->getAllAnimals();
        i64 new_id = all.back().animal_id;

        Transaction tx;
        tx.type        = TransactionType::BUY;
        tx.direction   = tx::typeToDir(TransactionType::BUY);
        tx.amount      = price;
        tx.entity_type = TransactionEntityType::ANIMAL;
        tx.entity_id   = std::to_string(new_id);
        tx.description = description;
        tx.date        = getCurrentDate();
        tx.status      = TransactionStatus::COMPLETED;

        tx_service->addTransaction(tx);

        std::cout << color::GREEN
                  << "Animal purchased and transaction recorded"
                  << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to buy animal: ") + e.what());
        return 1;
    }

    return 0;
}


int animalSell(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    auto* tx_service     = app_state->getTransactionService();
    if (!animal_service || !tx_service) {
        color::printError("Failed to initialize services");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "animal_id");

    try {
        i64 aid = std::stoll(id_str);

        AnimalRecord r = animal_service->getAnimalById(aid);

        if (r.status != AnimalStatus::ALIVE) {
            color::printError("Only ALIVE animals can be sold");
            return 1;
        }

        double price            = wx::lineInput<double>     ("Sale price: $");
        std::string description = wx::lineInput<std::string>("Description: ");

        animal_service->updateStatus(aid, AnimalStatus::PROCESSED);

        Transaction tx;
        tx.type        = TransactionType::SELL;
        tx.direction   = tx::typeToDir(TransactionType::SELL);
        tx.amount      = price;
        tx.entity_type = TransactionEntityType::ANIMAL;
        tx.entity_id   = std::to_string(aid);
        tx.description = description;
        tx.date        = getCurrentDate();
        tx.status      = TransactionStatus::COMPLETED;

        tx_service->addTransaction(tx);

        std::cout << color::GREEN
                  << "Animal sold and transaction recorded"
                  << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to sell animal: ") + e.what());
        return 1;
    }

    return 0;
}


int animalList(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    if (!animal_service) {
        color::printError("Failed to initialize animal service");
        return 1;
    }

    try {
        auto animals = animal_service->getAllAnimals();

        if (animals.empty()) {
            std::cout << color::YELLOW << "No animals found" << color::RESET << "\n";
            return 0;
        }

        std::cout << color::BLUE << "Animals:" << color::RESET << "\n";
        color::printSeperator();

        for (const auto& r : animals)
            printAnimal(r);

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to list animals: ") + e.what());
        return 1;
    }

    return 0;
}


int animalShow(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    if (!animal_service) {
        color::printError("Failed to initialize animal service");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "animal_id");

    try {
        i64 aid = std::stoll(id_str);
        auto r = animal_service->getAnimalById(aid);

        std::cout << color::BLUE << "Animal #" << r.animal_id << color::RESET << "\n";
        color::printSeperator();
        printAnimal(r, false);
        std::cout << "\tRegistered: " << r.created_at << "\n";
        color::printSeperator();

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to show animal: ") + e.what());
        return 1;
    }

    return 0;
}


int animalUpdateStatus(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    if (!animal_service) {
        color::printError("Failed to initialize animal service");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "animal_id");

    try {
        i64 aid = std::stoll(id_str);
        AnimalRecord r = animal_service->getAnimalById(aid);

        std::cout << "Current status for ID: " << aid << "\n";
        switch (r.status) {
            case AnimalStatus::ALIVE:
                std::cout << color::GREEN << "ALIVE";     break;
            case AnimalStatus::DEAD:
                std::cout << color::RED   << "DEAD";      break;
            case AnimalStatus::PROCESSED:
                std::cout << color::GREY  << "PROCESSED"; break;
        }
        std::cout << color::RESET << "\n";

        if (r.status != AnimalStatus::ALIVE) {
            std::cout << color::YELLOW
                      << "Only ALIVE animals can be updated."
                      << color::RESET << "\n";
            return 0;
        }

        AnimalStatus new_status = wx::selectInput<AnimalStatus>(
            "New status:",
            anim::AnimalStatusStrs, 3,
            anim::strToStatus
        );

        animal_service->updateStatus(aid, new_status);
        std::cout << color::GREEN << "Animal status updated" << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to update animal status: ") + e.what());
        return 1;
    }

    return 0;
}


int animalDelete(const Args& args)
{
    auto* app_state = getAppState(args);
    if (!app_state) return 1;

    auto* animal_service = app_state->getAnimalService();
    if (!animal_service) {
        color::printError("Failed to initialize animal service");
        return 1;
    }

    std::string id_str;
    loadArg(id_str, 0, "animal_id");

    try {
        i64 aid = std::stoll(id_str);
        animal_service->deleteAnimal(aid);
        std::cout << color::GREEN << "Animal deleted" << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(std::string("Failed to delete animal: ") + e.what());
        return 1;
    }

    return 0;
}
