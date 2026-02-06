#ifndef ANIMAL_H
#define ANIMAL_H

#include <string>

enum class AnimalPurpose {
    Milk,
    Egg,
    Meat
};

enum class AnimalStatus {
    Alive,
    Dead,
    Processed
};

enum class AnimalType {
    Cow,
    Hen
};

class Animal {
private:
    AnimalType _type;
    std::string _breed;
    std::string _gender;

    int _age;          // in years
    double _weight;       // in kg
    int _milk;
    int _eggs;
    int _meat;

    AnimalPurpose _purpose;
    AnimalStatus _status;

    int _productionStartAge;
    int _productionEndAge;
    double _meatWeightLimit;
    double _dailyFoodConsumed;

public:
    Animal(AnimalType type,
           const std::string& breed,
           const std::string& gender,
           AnimalPurpose purpose,
           int age,
           double weight);

    void feed();
    void grow();
    void produce();
    void checkLifecycle();
    void processForMeat();
    void die();

    // Getters
    AnimalType getType() const;
    AnimalPurpose getPurpose() const;
    AnimalStatus getStatus() const;

    int getAge() const;
    double getWeight() const;
    int getMilk() const;
    int getEggs() const;
    int getMeat() const;
    double getDailyFoodConsumed() const;

    bool isAlive() const;
};

#endif

