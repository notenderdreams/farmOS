#pragma once

#include "core/database.h"
#include "core/models/animal.h"
#include <vector>

class AnimalService : public Database
{
public:
    AnimalService(sqlite3* db);
    AnimalService(const std::string& db_path);

    void initTable();

    void addAnimal(const AnimalRecord& record);
    std::vector<AnimalRecord> getAllAnimals();
    AnimalRecord getAnimalById(i64 animal_id);

    void updateAnimal(const AnimalRecord& record);
    void updateStatus(i64 animal_id, AnimalStatus new_status);
    void deleteAnimal(i64 animal_id);
};
