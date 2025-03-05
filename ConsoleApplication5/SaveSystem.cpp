
#include <fstream>
#include <sstream>
#include "SaveSystem.h"
#include "armor.h"
#include "Food.h"
#include "Weapon.h"
#include "Fighter.h"
#include "Bag.h"
#include "def.h" 


bool SaveSystem::saveCharacterData(const CFighter* fighter, const string& filename) {
    if (!fighter) return false;

    ofstream file(filename);
    if (!file) return false;

    // 保存基本屬性
    file << "NAME=" << fighter->getname() << "\n";
    file << "LEVEL=" << fighter->getLevel() << "\n";
    file << "EXP=" << fighter->getCurrentExp() << "\n";
    file << "HP=" << fighter->getHP() << "\n";
    file << "MAXHP=" << fighter->getMAXHP() << "\n";
    file << "SP=" << fighter->getSP() << "\n";
    file << "MAXSP=" << fighter->getMAXSP() << "\n";
    file << "MONEY=" << fighter->getMoney() << "\n";
    file << "CITY=" << fighter->get_current_city() << "\n";

    // 保存角色能力值
    file << "STR=" << fighter->getStrength() << "\n";
    file << "INT=" << fighter->getIntelligence() << "\n";
    file << "TEN=" << fighter->getTenacity() << "\n";
    file << "LUCK=" << fighter->getLucky() << "\n";
    file << "POINTS=" << fighter->getAvailablePoints() << "\n";

    // 保存裝備資訊
    for (int i = 0; i < 4; i++) {
        CArmor* armor = fighter->getArmor(static_cast<ArmorType>(i));
        if (armor) {
            file << "ARMOR" << i << "=" << armor->getID() << ","
                << armor->getdefensebonus() << "\n";
        }
    }

    // 保存技能資訊
    file << "SKILLS=" << serializeSkills(fighter->getSkills()) << "\n";

    // 保存寵物資訊
    file << "PETS=" << serializePets(fighter->getPets()) << "\n";
    CBag* bag = fighter->getBag();  // 需要在 Fighter 類中添加獲取背包的方法
    if (bag) {
        string bagData = serializeBagItems(bag);
        file << "BAG=" << bagData << "\n";
    }

    return true;
}

bool SaveSystem::loadCharacterData(CFighter* fighter, const string& filename) {
    if (!fighter) return false;

    ifstream file(filename);
    if (!file) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string key;
        getline(iss, key, '=');
        
        string value;
        getline(iss, value);

        if (key == "NAME") fighter->setname(value);
        else if (key == "LEVEL") fighter->setLevel(stoi(value));
        else if (key == "MAXHP") fighter->setMaxHP(stoi(value));
        else if (key == "MAXSP") fighter->setMaxSP(stoi(value));
        else if (key == "HP") fighter->setHP(stoi(value));
        else if (key == "SP") fighter->setSP(stoi(value));
        else if (key == "MONEY") fighter->setMoney(stoi(value));
        else if (key == "CITY") fighter->set_current_city(stoi(value));
        else if (key == "STR") fighter->setStrength(stoi(value));
        else if (key == "INT") fighter->setIntelligence(stoi(value));
        else if (key == "TEN") fighter->setTenacity(stoi(value));
        else if (key == "LUCK") fighter->addLucky(stoi(value));
        else if (key == "POINTS") fighter->addAvailablePoints(stoi(value));
        else if (key == "EXP") {
            try {
                unsigned int exp = stoul(value);  // 使用 stoul 因為經驗值是無符號整數
                fighter->setCurrentExp(exp);
                cout << "成功讀取經驗值：" << exp << endl;  // 加入除錯訊息
            }
            catch (const exception& e) {
                cout << "錯誤：無法解析經驗值：" << value << endl;
            }
        }
       

        else if (key.substr(0, 5) == "ARMOR") {
            istringstream armor_iss(value);
            string id_str, defense_str;
            getline(armor_iss, id_str, ',');
            getline(armor_iss, defense_str);

            int id = stoi(id_str);
            int defense = stoi(defense_str);
            ArmorType type = static_cast<ArmorType>(stoi(key.substr(5)));

            CArmor* armor = new CArmor("", 0, 0, id, defense, type);
            fighter->setArmor(armor);
        }
        else if (key == "BAG") {
            CBag* bag = fighter->getBag();
            if (bag) {
                deserializeBagItems(bag, value);
            }
        }
    }

    return true;
}

std::string SaveSystem::serializeSkills(const std::vector<CSkill*>& skills) {
    std::stringstream ss;
    for (const auto& skill : skills) {
        ss << skill->getName() << ","
            << static_cast<int>(skill->getType()) << ","
            << skill->getLevel() << ";";
    }
    return ss.str();
}

bool SaveSystem::deserializeSkills(CFighter* fighter, const std::string& skillData) {
    std::stringstream ss(skillData);
    std::string skill_entry;
    while (std::getline(ss, skill_entry, ';')) {
        std::stringstream skill_ss(skill_entry);
        std::string name, type_str, level_str;
        std::getline(skill_ss, name, ',');
        std::getline(skill_ss, type_str, ',');
        std::getline(skill_ss, level_str, ',');

        CSkill* skill = new CSkill(name,
            static_cast<SkillType>(std::stoi(type_str)),
            20, 50, 1.2f, "");  // 基本屬性，可以根據需要調整
        for (int i = 1; i < std::stoi(level_str); i++) {
            skill->levelUp();
        }
        fighter->learnSkill(skill);
    }
    return true;
}

std::string SaveSystem::serializePets(const std::vector<CPet*>& pets) {
    std::stringstream ss;
    for (const auto& pet : pets) {
        ss << pet->getNickname() << ","
            << pet->getLevel() << ","
            << static_cast<int>(pet->getPetState()) << ";";
    }
    return ss.str();
}

bool SaveSystem::deserializePets(CFighter* fighter, const std::string& petData) {
    std::stringstream ss(petData);
    std::string pet_entry;
    while (std::getline(ss, pet_entry, ';')) {
        if (pet_entry.empty()) continue;

        std::stringstream pet_ss(pet_entry);
        std::string name, level_str, state_str;
        std::getline(pet_ss, name, ',');
        std::getline(pet_ss, level_str, ',');
        std::getline(pet_ss, state_str, ',');

        // 這裡需要根據具體的寵物系統實現來創建寵物
        // 下面是示例代碼，需要根據實際情況修改
        CMonster* monster = new CMonster(/*根據需要填入參數*/);
        CPet* pet = new CPet(monster, fighter);
        pet->setNickname(name);
        // 設置其他寵物屬性...

        fighter->capturePet(monster);
    }
    return true;
}




bool SaveSystem::deserializeBagItems(CBag* bag, const string& data) {
    if (!bag || data.empty()) return false;

    stringstream ss(data);
    string item_entry;

    while (getline(ss, item_entry, ';')) {
        if (item_entry.empty()) continue;

        stringstream item_ss(item_entry);
        string type_str, id_str, name, size_str, weight_str, num_str;

        getline(item_ss, type_str, ',');
        getline(item_ss, id_str, ',');
        getline(item_ss, name, ',');
        getline(item_ss, size_str, ',');
        getline(item_ss, weight_str, ',');
        getline(item_ss, num_str, ',');

        // 根據類型創建相應的物品
        CItem* item = nullptr;
        int type = stoi(type_str);
        int id = stoi(id_str);
        int size = stoi(size_str);
        int weight = stoi(weight_str);
        int num = stoi(num_str);

        switch (type) {
        case eweapon: {
            CWeapon* weapon = new CWeapon(name, size, weight, id);
            item = weapon;
            break;
        }
        case earmor: {
            CArmor* armor = new CArmor(name, size, weight, id);
            item = armor;
            break;
        }
        case efood: {
            CFood* food = new CFood(name, size, weight, id);
            item = food;
            break;
        }
                  // 添加其他物品類型...
        }

        if (item) {
            CBagEntry* entry = bag->item_lookup(type, id);
            if (entry) {
                entry->addNum(num);
            }
            else {
                bag->item_insert(item);
                entry = bag->item_lookup(type, id);
                if (entry) {
                    entry->number = num;
                }
            }
        }
    }

    return true;
}
std::string SaveSystem::serializeBagItems(const CBag* bag) {
    if (!bag) return "";

    stringstream ss;
    CBagEntry* entry = bag->listhead.lh_first;

    while (entry) {
        CItem* item = entry->itm;
        if (item) {
            ss << item->isA() << "," // 物品類型
                << item->getID() << "," // 物品ID
                << item->getName() << "," // 名稱
                << item->getSize() << "," // 大小
                << item->getWeight() << "," // 重量
                << entry->getNum() // 數量
                << ";"; // 分隔不同物品
        }
        entry = entry->next_link.le_next;
    }

    return ss.str();
}