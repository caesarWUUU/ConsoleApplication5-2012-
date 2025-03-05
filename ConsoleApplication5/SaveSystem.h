// SaveSystem.h
#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <string>
#include "Fighter.h"
#include "Pet.h"
#include "Bag.h"      // �K�[ Bag �������Y���

class SaveSystem {
public:
    static bool saveCharacterData(const CFighter* fighter, const std::string& filename);
    static bool loadCharacterData(CFighter* fighter, const std::string& filename);

private:
    // �w��������n��
    static std::string serializeSkills(const std::vector<CSkill*>& skills);
    static bool deserializeSkills(CFighter* fighter, const std::string& skillData);
    static std::string serializePets(const std::vector<CPet*>& pets);
    static bool deserializePets(CFighter* fighter, const std::string& petData);

    // �K�[�I�]����������n��
    static std::string serializeBagItems(const CBag* bag);
    static bool deserializeBagItems(CBag* bag, const std::string& data);
};

#endif