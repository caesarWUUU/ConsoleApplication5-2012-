// SaveSystem.h
#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <string>
#include "Fighter.h"
#include "Pet.h"
#include "Bag.h"      // 添加 Bag 相關的頭文件

class SaveSystem {
public:
    static bool saveCharacterData(const CFighter* fighter, const std::string& filename);
    static bool loadCharacterData(CFighter* fighter, const std::string& filename);

private:
    // 已有的函數聲明
    static std::string serializeSkills(const std::vector<CSkill*>& skills);
    static bool deserializeSkills(CFighter* fighter, const std::string& skillData);
    static std::string serializePets(const std::vector<CPet*>& pets);
    static bool deserializePets(CFighter* fighter, const std::string& petData);

    // 添加背包相關的函數聲明
    static std::string serializeBagItems(const CBag* bag);
    static bool deserializeBagItems(CBag* bag, const std::string& data);
};

#endif