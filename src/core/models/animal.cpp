#include "animal.h"

namespace anim {
    const char* AnimalTypeStrs[]    = { "COW", "HEN" };
    const char* AnimalPurposeStrs[] = { "MILK", "EGG", "MEAT" };
    const char* AnimalStatusStrs[]  = { "ALIVE", "DEAD", "PROCESSED" };
}



Animal::Animal(const AnimalRecord& record)
    : _r(record)
{
    if (_r.type == AnimalType::COW) {
        _productionStartAge = 3;
        _productionEndAge   = 8;
        _meatWeightLimit    = 200.0;
    } 
    else { 
        _productionStartAge = 1;
        _productionEndAge   = 7;
        _meatWeightLimit    = 2.5;
    }
}


void Animal::feed()
{
    if (!isAlive()) {
        _dailyFoodConsumed = 0.0;
        return;
    }

    if (_r.type == AnimalType::COW) {
        _dailyFoodConsumed = _r.weight * 0.03; // ~3% body weight
        _r.weight += 1.0;
    } else { 
        _dailyFoodConsumed = 0.11;             // ~110 g/day
        _r.weight += 0.035;
    }
}


void Animal::grow()
{
    if (!isAlive()) return;
    ++_r.age;
}


void Animal::produce()
{
    if (!isAlive()) return;
    if (_r.gender != "female") return;
    if (_r.age < _productionStartAge || _r.age > _productionEndAge) return;

    if (_r.purpose == AnimalPurpose::MILK && _r.type == AnimalType::COW)
        _r.milk += 10;

    if (_r.purpose == AnimalPurpose::EGG && _r.type == AnimalType::HEN)
        _r.eggs += 1;
}


void Animal::checkLifecycle()
{
    if (!isAlive()) return;

    if (_r.purpose == AnimalPurpose::MEAT && _r.weight >= _meatWeightLimit)
        processForMeat();

    if (_r.purpose != AnimalPurpose::MEAT && _r.age > _productionEndAge)
        processForMeat();
}


void Animal::processForMeat()
{
    if (!isAlive()) return;
    _r.status = AnimalStatus::PROCESSED;
    _r.meat   = (_r.type == AnimalType::COW) ? 200 : 2;
}


void Animal::die()
{
    _r.status = AnimalStatus::DEAD;
}
