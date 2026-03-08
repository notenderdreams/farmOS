#pragma once

#include <string>
#include <stdexcept>
#include "core/types.h"


enum class AnimalType {
    COW,
    HEN
};

enum class AnimalPurpose {
    MILK,
    EGG,
    MEAT
};

enum class AnimalStatus {
    ALIVE,
    DEAD,
    PROCESSED
};


struct AnimalRecord {
    i64 animal_id = 0;

    AnimalType    type;
    std::string   breed;
    std::string   gender;       // "male" | "female"
    AnimalPurpose purpose;
    AnimalStatus  status = AnimalStatus::ALIVE;

    i32 age    = 0;             // years
    f64 weight = 0.0;           // kg

    i32 milk = 0;               // litres  
    i32 eggs = 0;               // count   
    i32 meat = 0;               // kg processing

    std::string created_at;
};


class Animal {
public:
    explicit Animal(const AnimalRecord& record);

    void feed();
    void grow();
    void produce();
    void checkLifecycle();
    void processForMeat();
    void die();

    const AnimalRecord& getRecord() const { return _r; }

    AnimalType    getType()    const { return _r.type;    }
    AnimalPurpose getPurpose() const { return _r.purpose; }
    AnimalStatus  getStatus()  const { return _r.status;  }

    i32  getAge()    const { return _r.age;    }
    f64  getWeight() const { return _r.weight; }
    i32  getMilk()   const { return _r.milk;   }
    i32  getEggs()   const { return _r.eggs;   }
    i32  getMeat()   const { return _r.meat;   }
    f64  getDailyFoodConsumed() const { return _dailyFoodConsumed; }

    const std::string& getBreed()  const { return _r.breed;  }
    const std::string& getGender() const { return _r.gender; }

    bool isAlive() const { return _r.status == AnimalStatus::ALIVE; }

private:
    AnimalRecord _r;
    f64 _dailyFoodConsumed = 0.0;

    i32 _productionStartAge = 0;
    i32 _productionEndAge   = 0;
    f64 _meatWeightLimit    = 0.0;
};


namespace anim {

    extern const char* AnimalTypeStrs[];
    extern const char* AnimalPurposeStrs[];
    extern const char* AnimalStatusStrs[];

    static const char* toStr(AnimalType t)    { return AnimalTypeStrs   [static_cast<int>(t)]; }
    static const char* toStr(AnimalPurpose p) { return AnimalPurposeStrs[static_cast<int>(p)]; }
    static const char* toStr(AnimalStatus s)  { return AnimalStatusStrs [static_cast<int>(s)]; }

    inline AnimalType strToType(const std::string& s) {
        if (s == "COW") return AnimalType::COW;
        if (s == "HEN") return AnimalType::HEN;
        throw std::runtime_error("Invalid animal type: " + s);
    }

    inline AnimalPurpose strToPurpose(const std::string& s) {
        if (s == "MILK") return AnimalPurpose::MILK;
        if (s == "EGG")  return AnimalPurpose::EGG;
        if (s == "MEAT") return AnimalPurpose::MEAT;
        throw std::runtime_error("Invalid animal purpose: " + s);
    }

    inline AnimalStatus strToStatus(const std::string& s) {
        if (s == "ALIVE")     return AnimalStatus::ALIVE;
        if (s == "DEAD")      return AnimalStatus::DEAD;
        if (s == "PROCESSED") return AnimalStatus::PROCESSED;
        throw std::runtime_error("Invalid animal status: " + s);
    }

} 
