#include "animal.h"
#include <cstdlib>

// Constructor
Animal::Animal(AnimalType type,
               const std::string& breed, const std::string& gender,
               AnimalPurpose purpose,
               int age,
               double weight) : _type(type),
                            _breed(breed),
                            _gender(gender),
                            _age(age),
                            _weight(weight),
                            _purpose(purpose),
                            _status(AnimalStatus::Alive),
                            _milk(0),
                            _eggs(0),
                            _meat(0){
    // Defaults based on animal type
    if (_type == AnimalType::Cow) {
        _productionStartAge = 3;
        _productionEndAge   = 8;
        _meatWeightLimit    = 200;
    }
    else { // Hen
        _productionStartAge = 1;
        _productionEndAge   = 7;
        _meatWeightLimit    = 2.5;
    }
}

// Feeding logic
void Animal::feed()
{
    if (!isAlive()) {
        _dailyFoodConsumed = 0.0;
        return;
    }

    if (_type == AnimalType::Cow) {
        _dailyFoodConsumed = _weight * 0.03; // cow eats ~3% body weight
        _weight += 1.0;
    }
    else { // Hen
        _dailyFoodConsumed = 0.11; // ~110 grams/day
        _weight += 0.035;
    }
}


// Aging
void Animal::grow(){
    if (!isAlive()) return;
    ++_age;
}

// Production
void Animal::produce(){
    if (!isAlive()) return;
    if (_gender != "female") return;
    if (_age < _productionStartAge || _age > _productionEndAge) return;

    if (_purpose == AnimalPurpose::Milk && _type == AnimalType::Cow) {
        _milk += 10;
    }

    if (_purpose == AnimalPurpose::Egg && _type == AnimalType::Hen) {
        _eggs += 1;
    }
}

// Lifecycle management
void Animal::checkLifecycle(){
    if (!isAlive()) return;

    if (_purpose == AnimalPurpose::Meat && _weight >= _meatWeightLimit) {
        processForMeat();
    }

    if (_purpose != AnimalPurpose::Meat && _age > _productionEndAge) {
        processForMeat();
    }
}

// Slaughter
void Animal::processForMeat(){
    if (!isAlive()) return;

    _status = AnimalStatus::Processed;

    if (_type == AnimalType::Cow)
        _meat = 200;
    else
        _meat = 2;
}

// Death
void Animal::die(){
    _status = AnimalStatus::Dead;
}

// Getters
AnimalType Animal::getType() const { return _type; }
AnimalPurpose Animal::getPurpose() const { return _purpose; }
AnimalStatus Animal::getStatus() const { return _status; }

int Animal::getAge() const { return _age; }
double Animal::getWeight() const { return _weight; }
int Animal::getMilk() const { return _milk; }
int Animal::getEggs() const { return _eggs; }
int Animal::getMeat() const { return _meat; }
double Animal:: getDailyFoodConsumed() const {
    return _dailyFoodConsumed;
}

bool Animal::isAlive() const{
    return _status == AnimalStatus::Alive;
}
