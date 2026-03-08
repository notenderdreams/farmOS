#include "core/animal_service.h"
#include "core/models/schema.h"
#include <stdexcept>
#include <vector>

AnimalService::AnimalService(sqlite3* db)
    : Database(db) {}

AnimalService::AnimalService(const std::string& db_path)
    : Database(db_path) {}

void AnimalService::initTable()
{
    execute(std::string(farmos::models::ANIMALS_TABLE));
}

void AnimalService::addAnimal(const AnimalRecord& record)
{
    const char* sql = R"(
        INSERT INTO animals (
            type, breed, gender, purpose, status,
            age, weight, milk, eggs, meat
        )
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_text  (stmt, 1,  anim::toStr(record.type),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2,  record.breed.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3,  record.gender.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 4,  anim::toStr(record.purpose), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 5,  anim::toStr(record.status),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt, 6,  record.age);
    sqlite3_bind_double(stmt, 7,  record.weight);
    sqlite3_bind_int   (stmt, 8,  record.milk);
    sqlite3_bind_int   (stmt, 9,  record.eggs);
    sqlite3_bind_int   (stmt, 10, record.meat);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to insert animal");
    }

    sqlite3_finalize(stmt);
}

// ── shared row-reader helper ──────────────────────────────────────────────────
static AnimalRecord rowToRecord(sqlite3_stmt* stmt)
{
    AnimalRecord r;
    r.animal_id  = sqlite3_column_int64(stmt, 0);
    r.type       = anim::strToType   (reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    r.breed      =                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    r.gender     =                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    r.purpose    = anim::strToPurpose(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    r.status     = anim::strToStatus (reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
    r.age        = sqlite3_column_int   (stmt, 6);
    r.weight     = sqlite3_column_double(stmt, 7);
    r.milk       = sqlite3_column_int   (stmt, 8);
    r.eggs       = sqlite3_column_int   (stmt, 9);
    r.meat       = sqlite3_column_int   (stmt, 10);
    r.created_at =                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
    return r;
}

std::vector<AnimalRecord> AnimalService::getAllAnimals()
{
    const char* sql = "SELECT * FROM animals;";
    sqlite3_stmt* stmt = prepare(sql);

    std::vector<AnimalRecord> animals;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        animals.push_back(rowToRecord(stmt));
    }

    sqlite3_finalize(stmt);
    return animals;
}

AnimalRecord AnimalService::getAnimalById(i64 animal_id)
{
    const char* sql = "SELECT * FROM animals WHERE animal_id = ?;";
    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_int64(stmt, 1, animal_id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        AnimalRecord r = rowToRecord(stmt);
        sqlite3_finalize(stmt);
        return r;
    }

    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: No animal found with the given ID");
}

void AnimalService::updateAnimal(const AnimalRecord& record)
{
    const char* sql = R"(
        UPDATE animals
        SET type    = ?,
            breed   = ?,
            gender  = ?,
            purpose = ?,
            status  = ?,
            age     = ?,
            weight  = ?,
            milk    = ?,
            eggs    = ?,
            meat    = ?
        WHERE animal_id = ?;
    )";

    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_text  (stmt, 1,  anim::toStr(record.type),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2,  record.breed.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3,  record.gender.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 4,  anim::toStr(record.purpose), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 5,  anim::toStr(record.status),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt, 6,  record.age);
    sqlite3_bind_double(stmt, 7,  record.weight);
    sqlite3_bind_int   (stmt, 8,  record.milk);
    sqlite3_bind_int   (stmt, 9,  record.eggs);
    sqlite3_bind_int   (stmt, 10, record.meat);
    sqlite3_bind_int64 (stmt, 11, record.animal_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to update animal");
    }

    if (sqlite3_changes(db) == 0)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No animal found with the given ID");
    }

    sqlite3_finalize(stmt);
}

void AnimalService::updateStatus(i64 animal_id, AnimalStatus new_status)
{
    const char* sql = "UPDATE animals SET status = ? WHERE animal_id = ?;";
    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_text (stmt, 1, anim::toStr(new_status), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, animal_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to update animal status");
    }

    if (sqlite3_changes(db) == 0)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: No animal found with the given ID");
    }

    sqlite3_finalize(stmt);
}

void AnimalService::deleteAnimal(i64 animal_id)
{
    const char* sql = "DELETE FROM animals WHERE animal_id = ?;";
    sqlite3_stmt* stmt = prepare(sql);

    sqlite3_bind_int64(stmt, 1, animal_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("DB: Failed to delete animal");
    }

    sqlite3_finalize(stmt);
}
