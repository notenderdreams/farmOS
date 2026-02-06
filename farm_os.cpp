#include "farm_os.h"
#include <algorithm>

// Helper: convert string to AnimalType
static AnimalType parseAnimalType(const std::string& type){
    if (type == "cow") return AnimalType::Cow;
    return AnimalType::Hen;
}

void FarmOS::addAnimal(const std::string& type,
                       const std::string& breed,
                       const std::string& gender,
                       AnimalPurpose purpose,
                       int count){
    AnimalType atype = parseAnimalType(type);

    for (int i = 0; i < count; ++i) {
        _animals.push_back(
            std::make_unique<Animal>(
                atype,
                breed,
                gender,
                purpose,
                0,      // age
                (atype == AnimalType::Cow) ? 80 : .7  // initial weight
            )
        );
    }
}

// Daily actions
void FarmOS::feedAll(){
    for (auto& a : _animals) a->feed();
}

void FarmOS::growAll(){
    for (auto& a : _animals) a->grow();
}

void FarmOS::produceAll(){
    for (auto& a : _animals) a->produce();
}

void FarmOS::checkAllLifecycles(){
    for (auto& a : _animals) a->checkLifecycle();
}

void FarmOS::removeDead(){
    _animals.erase(
        std::remove_if(
            _animals.begin(),
            _animals.end(),
            [](const std::unique_ptr<Animal>& a) {
                return a->getStatus() != AnimalStatus::Alive;
            }),
        _animals.end()
    );
}

// One full day simulation
void FarmOS::simulateDay(){
    feedAll();
    produceAll();
    growAll();
    checkAllLifecycles();
    removeDead();
}

// Stats
int FarmOS::totalAnimals() const{
    int count = 0;
    for (const auto& a : _animals)
        if (a->isAlive()) ++count;
    return count;
}

int FarmOS::totalMilk() const{
    int sum = 0;
    for (const auto& a : _animals)
        sum += a->getMilk();
    return sum;
}

int FarmOS::totalEggs() const{
    int sum = 0;
    for (const auto& a : _animals)
        sum += a->getEggs();
    return sum;
}

int FarmOS::totalMeat() const{
    int sum = 0;
    for (const auto& a : _animals)
        sum += a->getMeat();
    return sum;
}

double FarmOS::dailyFoodConsumed() const
{
    double total = 0.0;
    for (const auto& a : _animals)
        total += a->getDailyFoodConsumed();
    return total;
}

