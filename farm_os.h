#pragma once
#include <vector>
#include <memory>
#include <string>
#include "animal.h"

class FarmOS {
public:
    void addAnimal(const std::string& type,
                   const std::string& breed,
                   const std::string& gender,
                   AnimalPurpose purpose,
                   int count = 1);

    void feedAll();
    void growAll();
    void produceAll();
    void processReadyAnimals();
    void removeDead();
    void checkAllLifecycles();
    double dailyFoodConsumed() const;

    void simulateDay(); // feed, grow, produce, process

    // Stats
    int totalAnimals() const;
    int totalMilk() const;
    int totalEggs() const;
    int totalMeat() const;

private:
    std::vector<std::unique_ptr<Animal>> _animals;
};
