#include <iostream>
#include "Fighter.h"
#include "food.h"
#include "Bag.h"
#include "def.h"
#include "GlobalInfo.h"
#include "MapData.h"
#include "armor.h"
#include <iomanip>
#include "LifeEntity.h"
#include <windows.h>
#include"SaveDirectoryManager.h"
#include"AccountManager.h"
#include"SaveSystem.h"

using namespace std;

CFighter::CFighter(int initHP, int initSP, int initLucky, int in_city)
	: CLifeEntity()
{
	if (initHP == 0 && initSP == 0) {
		initHP = 1000;
		initSP = 2000;
		setname("無名戰士");
	}

	setInitSPHP(initHP, initSP);
	Lucky = initLucky;
	cur_city = in_city;
	bag = new CBag();
	equipped_armors.fill(nullptr);
	money = 100000;
	base_attack = FIGHTER_BASE_ATTACK;
	base_defense = FIGHTER_BASE_DEFENSE;

	// 初始化新屬性
	strength = 0;
	intelligence = 0;
	tenacity = 0;
	available_points = 0;

	// 初始化等級和經驗值
	level = 15;
	current_exp = 0;

	// 初始化職業相關
	job_class = JobClass::NOVICE;  // 初始為初心者
	skill_points = 0;              // 初始技能點數為0
	equipped_skills.fill(nullptr); // 初始化技能快捷鍵為空

	cout << "One Fighter is created with (maxHP, maxSP, maxLucky) = ("
		<< initHP << ", " << initSP << ", " << initLucky << ")" << endl;
	cout << "初始金錢: " << money << endl;
}
bool CFighter::capturePet(CMonster* monster) {
	// 檢查寵物數量上限
	if (pets.size() >= PET_MAX_COUNT) {
		cout << "寵物數量已達上限！" << endl;
		return false;
	}


	CPet* new_pet = new CPet(monster, this);
	pets.push_back(new_pet);


	 if (pets.size() == 1) {
	     active_pet = new_pet;
	 }

	return true;
}

void CFighter::showPets() const {
	cout << "\n=== 寵物列表 ===" << endl;
	if (pets.empty()) {
		cout << "目前沒有寵物。" << endl;
		return;
	}

	for (size_t i = 0; i < pets.size(); ++i) {
		cout << i + 1 << ". ";
		if (pets[i] == active_pet) {
			cout << "[出戰中] ";
		}
		cout << pets[i]->getNickname() << " (Lv." << pets[i]->getLevel() << ")" << endl;
	}
}
const vector<CPet*>& CFighter::getPets() const {
	return pets;
}

CPet* CFighter::getActivePet() const {
	return active_pet;
}
bool CFighter::switchPet(int pet_index) {
	if (pet_index < 0 || pet_index >= pets.size()) {
		return false;
	}

	CPet* selected_pet = pets[pet_index];
	if (selected_pet->getPetState() == PetState::TIRED) {
		cout << "此寵物正處於疲勞狀態，無法出戰！" << endl;
		return false;
	}

	// 將當前出戰寵物設為休息狀態
	if (active_pet) {
		active_pet->setPetState(PetState::IDLE);
	}

	// 設定新的出戰寵物
	active_pet = selected_pet;
	active_pet->setPetState(PetState::ACTIVE);
	return true;
}
CFighter::~CFighter() {
	
	if (bag) {
		delete bag;
	}
	// 清理已裝備的防具
	for (CArmor* armor : equipped_armors) {
		if (armor) {
			delete armor;
		}
	}
}
unsigned int CFighter::getNextLevelExp() const {
	if (level >= FIGHTER_MAX_LEVEL) return 0;

	// 使用整數計算避免浮點數誤差
	unsigned int required_exp = FIGHTER_BASE_EXP;
	for (int i = 1; i < level; i++) {
		required_exp = static_cast<unsigned int>(required_exp * FIGHTER_EXP_MULTIPLIER);
	}
	return required_exp;
}

void CFighter::addExp(int exp) {
	if (level >= FIGHTER_MAX_LEVEL) {
		cout << "已達到最高等級！" << endl;
		return;
	}

	cout << "\n獲得 " << exp << " 點經驗值！" << endl;
	current_exp += exp;

	// 顯示當前經驗值狀態
	cout << "當前經驗值：" << current_exp << "/" << getNextLevelExp();
	double progress = (static_cast<double>(current_exp) / getNextLevelExp()) * 100.0;
	cout << " (" << fixed << setprecision(2) << progress << "%)" << endl;

	// 檢查是否可以升級
	while (canLevelUp()) {
		levelUp();
		if (level < FIGHTER_MAX_LEVEL) {
			// 如果還有多餘經驗值，顯示新的進度
			cout << "剩餘經驗值：" << current_exp << "/" << getNextLevelExp();
			progress = (static_cast<double>(current_exp) / getNextLevelExp()) * 100.0;
			cout << " (" << fixed << setprecision(2) << progress << "%)" << endl;
		}
	}
}
ArmorSet CFighter::getCurrentSet() const {
	if (!hasFullSet()) return ArmorSet::eNoSet;  // 修改為使用完整的枚舉名稱
	return equipped_armors[0]->getArmorSet();
}

bool CFighter::canLevelUp() const {
	if (level >= FIGHTER_MAX_LEVEL) return false;
	return current_exp >= static_cast<unsigned long long>(getNextLevelExp());
}


void CFighter::levelUp() {
	if (level >= FIGHTER_MAX_LEVEL) return;

	unsigned int required_exp = getNextLevelExp();
	current_exp -= required_exp;
	level++;

	cout << "\n=== 等級提升! ===" << endl;
	cout << "等級提升至 " << level << " 級!" << endl;

	// 獲得屬性點數
	available_points += 5;
	cout << "\n獲得 5 點屬性點!" << endl;
	cout << "當前未分配屬性點: " << available_points << endl;

	// 如果已經轉職，獲得技能點數
	if (job_class != JobClass::NOVICE) {
		skill_points += 3;
		cout << "\n獲得 3 點技能點數!" << endl;
		cout << "當前技能點數: " << skill_points << endl;
	}

	// 提示玩家
	cout << "\n你可以使用 status 指令來查看並分配屬性點" << endl;
	if (job_class != JobClass::NOVICE) {
		cout << "使用 skill 指令可以查看和升級技能" << endl;
	}
	else if (level >= JOB_CHANGE_LEVEL) {
		cout << "你已達到可以轉職的等級！使用 job 指令進行轉職。" << endl;
	}

	// 恢復滿血滿魔
	setHP(getMAXHP());
	setSP(getMAXSP());
}

// 取得經驗值進度百分比
double CFighter::getExpProgress() const {
	if (level >= FIGHTER_MAX_LEVEL) return 100.0;
	return (double)current_exp / getNextLevelExp() * 100;
}
void CFighter::showLevelAndExp() const {
	cout << "\n=== 等級資訊 ===" << endl;
	cout << "等級: " << level;
	if (level < FIGHTER_MAX_LEVEL) {
		cout << " (經驗值: " << current_exp << "/" << getNextLevelExp()
			<< ", " << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
	}
	else {
		cout << " (已達最高等級)" << endl;
	}
}
int CFighter::getAttack() const {
	int totalAttack = base_attack;
	if (getWeapon()) {
		totalAttack += getWeapon()->getattackbonus();
	}

	

	return totalAttack;
}

// 新增取得總防禦力的函數
int CFighter::getDefense() const {
	int totalDefense = base_defense + getTotalDefenseBonus() + getArmorSetBonus();

	// 加入職業加成
	

	return totalDefense;
}
int CFighter::physicaldamage (){
	return (rand () % getSP ());
}

int CFighter::attack (CLifeEntity *l){
	int damage = physicaldamage () - l->defense (l); 
	if (damage > l->getHP ())
		damage = l->getHP ();
	
	l->gethurt (damage);
	
	if (damage > 0){
		cout << this->getname () << " 猛力一揮，造成 " << l->getname () << " " << damage << " 血損失" <<endl;			
	} else {
		cout << this->getname () << " 猛力一揮，但是 " << l->getname () << " 強力一擋，因此沒有造成任何損失" <<endl;
	}
	return (damage > 0 ? damage : 0);
}

int CFighter::defense(CLifeEntity* l) {
	return getLucky() + getTotalDefenseBonus();  // 包含防具的防禦加成
}

void CFighter::captureItem(CItem* in_item, bool replace) {
	if (!in_item) return;

	CBagEntry* entry = bag->item_lookup(in_item->isA(), in_item->getID());
	if (!entry) {
		bag->item_insert(in_item, 1, replace);
	}
	else {
		if (replace) {
			entry->setNum(1);
		}
		else {
			entry->addNum(1);
		}
		delete in_item;
	}
	cout << this->getname() << " 獲得了 " << in_item->getName() << endl;

	// 立即保存角色數據
	auto& saveManager = SaveDirectoryManager::getInstance();
	string savePath = saveManager.getCharacterSavePath(
		AccountManager::getInstance().getCurrentUsername(),
		this->getname()
	);
	SaveSystem::saveCharacterData(this, savePath);
}

int CFighter::showAllBagItems (){	
	return bag->showAllItems ();	
}

bool CFighter::useBagItems(int no) {
	CBagEntry* ne = bag->item_lookup(no);
	if (!ne) {
		return false;
	}

	// 如果是武器
	if (ne->itm->isA() == eweapon) {
		CWeapon* old_weapon = this->getWeapon();

		// 先把新武器從背包移出
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		// 裝備新武器
		this->setWeapon((CWeapon*)ne->itm);
		cout << this->getname() << " 裝備了 " << ne->itm->getName() << endl;

		// 如果有舊武器，放回背包
		if (old_weapon != NULL) {
			CBagEntry* entry = bag->item_lookup(old_weapon->isA(), old_weapon->getID());
			if (!entry) {
				bag->item_insert(old_weapon);
			}
			else {
				entry->addNum(1);
			}
			cout << this->getname() << " 將 " << old_weapon->getName() << " 收回背包" << endl;
		}

		cout << "\n背包目前內容：" << endl;
		this->showAllBagItems();
	}
	// 如果是防具
	else if (ne->itm->isA() == earmor) {
		CArmor* new_armor = static_cast<CArmor*>(ne->itm);
		ArmorType type = new_armor->getArmorType();

		// 先取得舊防具
		CArmor* old_armor = this->getArmor(type);

		// 從背包移出新防具
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		// 裝備新防具
		this->setArmor(new CArmor(*new_armor));
		cout << this->getname() << " 裝備了 " << new_armor->getName() << endl;

		// 如果有舊防具，放回背包
		if (old_armor != NULL) {
			CBagEntry* entry = bag->item_lookup(old_armor->isA(), old_armor->getID());
			if (!entry) {
				bag->item_insert(old_armor);
			}
			else {
				entry->addNum(1);
				delete old_armor;
			}
			cout << this->getname() << " 將 " << old_armor->getName() << " 收回背包" << endl;
		}

		cout << "\n背包目前內容：" << endl;
		this->showAllBagItems();
	}
	// 其他物品（如食物等）
	else {
		ne->itm->beUsed(this);
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		cout << "\n背包目前內容：" << endl;
		this->showAllBagItems();
	}

	return true;
}

int CFighter::isA() const {  // 修改為 const
	return efighter;
}

int CFighter::goto_next_city (int next_dir){
	int next_city = CGlobalInfo::map_data->next_city (cur_city, next_dir);	
	if (next_city){
		cur_city = next_city;
		return cur_city;
	}
	return 0;
}

int CFighter::get_current_city() const {
	return cur_city;
}
void CFighter::set_current_city(int new_city) {
	cur_city = new_city;
}
void CFighter::setArmor(CArmor* armor) {
	if (!armor) return;

	ArmorType type = armor->getArmorType();
	// 使用 static_cast 將枚舉轉換為索引
	int typeIndex = static_cast<int>(type);

	// 如果該部位已經有防具，先卸下
	if (equipped_armors[typeIndex] != nullptr) {
		equipped_armors[typeIndex]->UnUsed(this);
		delete equipped_armors[typeIndex];
	}

	equipped_armors[typeIndex] = armor;
	armor->beUsed(this);
}
void CFighter::removeArmor(ArmorType type) {
	int typeIndex = static_cast<int>(type);
	if (equipped_armors[typeIndex]) {
		equipped_armors[typeIndex]->UnUsed(this);
		delete equipped_armors[typeIndex];
		equipped_armors[typeIndex] = nullptr;
	}
}
CArmor* CFighter::getArmor(ArmorType type) const {
	return equipped_armors[static_cast<int>(type)];
}
int CFighter::getTotalDefenseBonus() const {
	int total = 0;
	for (CArmor* armor : equipped_armors) {
		if (armor) {
			total += armor->getdefensebonus();
		}
	}
	return total;
}
bool CFighter::hasFullSet() const {
	if (equipped_armors[0] == nullptr) return false;  // 必須有頭盔

	ArmorSet baseSet = equipped_armors[0]->getArmorSet();
	if (baseSet == ArmorSet::eNoSet) return false;

	// 檢查所有部位是否都裝備且屬於同一套裝
	for (const auto& armor : equipped_armors) {
		if (armor == nullptr || armor->getArmorSet() != baseSet) {
			return false;
		}
	}
	return true;
}


int CFighter::getArmorSetBonus() const {
	if (!hasFullSet()) return 0;

	ArmorSet currentSet = getCurrentSet();
	return CArmor::getSetBonus(currentSet);
}
void CFighter::showWeapon() const {
	cout << "\n=== 已裝備的武器 ===" << endl;
	if (getWeapon()) {
		cout << "武器: " << getWeapon()->getName()
			<< " (攻擊力 +" << getWeapon()->getattackbonus() << ")"
			<< endl;
	}
	else {
		cout << "武器: 無" << endl;
	}
}
bool CFighter::changeJob(JobClass newJob) {
	if (level < 15) {
		cout << "等級不足!需要達到15級才能轉職" << endl;
		return false;
	}

	if (job_class != JobClass::NOVICE) {
		cout << "已經轉職過了!" << endl;
		return false;
	}

	job_class = newJob;
	skill_points = 30;  // 初始獲得3點技能點數

	// 根據職業給予初始技能
	if (job_class == JobClass::WARRIOR) {
		cout << "\n=== 成功轉職為戰士! ===" << endl;

		// 給予戰士初始技能
		learnSkill(new CSkill("猛力一擊", SkillType::PHYSICAL, 20, 50, 1.2f,
			"對敵人造成強力的物理傷害"));
		learnSkill(new CSkill("戰吼", SkillType::BUFF, 30, 20, 1.1f,
			"提升自身攻擊力"));

		// 增加戰士額外屬性
		setMaxHP(getMAXHP() + 200);
		setHP(getMAXHP());
		base_attack += 30;
		base_defense += 20;

	}
	else if (job_class == JobClass::MAGE) {
		cout << "\n=== 成功轉職為法師! ===" << endl;

		// 給予法師初始技能
		learnSkill(new CSkill("火球術", SkillType::MAGICAL, 25, 60, 1.3f,
			"發射火球造成魔法傷害"));
		learnSkill(new CSkill("治癒術", SkillType::HEAL, 40, 40, 1.2f,
			"恢復生命值"));

		// 增加法師額外屬性
		setMaxSP(getMAXSP() + 150);
		setSP(getMAXSP());
		intelligence += 40;
	}

	cout << "獲得了初始技能與3點技能點數!" << endl;
	cout << "可以使用 skill 指令查看和升級技能" << endl;

	return true;
}
bool CFighter::learnSkill(CSkill* skill) {
	if (!skill) return false;
	skills.push_back(skill);
	return true;
}

bool CFighter::upgradeSkill(int skillIndex) {
	if (skillIndex < 0 || skillIndex >= skills.size()) {
		cout << "無效的技能索引!" << endl;
		return false;
	}

	if (skill_points <= 0) {
		cout << "技能點數不足!" << endl;
		return false;
	}

	if (skills[skillIndex]->levelUp()) {
		skill_points--;
		cout << "剩餘技能點數: " << skill_points << endl;
		return true;
	}

	return false;
}
void CFighter::showSkills() const {
	cout << "\n=== 技能列表 ===" << endl;
	cout << "職業: " << (job_class == JobClass::WARRIOR ? "戰士" :
		job_class == JobClass::MAGE ? "法師" : "初心者") << endl;
	cout << "技能點數: " << skill_points << endl;
	cout << "\n習得的技能:" << endl;

	for (size_t i = 0; i < skills.size(); ++i) {
		const CSkill* skill = skills[i];
		cout << i + 1 << ". " << skill->getName()
			<< " Lv." << skill->getLevel()
			<< " (SP消耗: " << skill->getSpCost() << ")"
			<< "\n   " << skill->getDescription() << endl;
	}

	cout << "\n裝備到快捷鍵的技能:" << endl;
	const char* keys[] = { "Z", "X", "C" };
	for (size_t i = 0; i < equipped_skills.size(); ++i) {
		cout << keys[i] << ": ";
		if (equipped_skills[i]) {
			cout << equipped_skills[i]->getName() << endl;
		}
		else {
			cout << "無" << endl;
		}
	}
}
bool CFighter::useSkill(int skillIndex, CLifeEntity* target) {
	if (skillIndex < 0 || skillIndex >= skills.size()) {
		cout << "無效的技能索引!" << endl;
		return false;
	}

	return skills[skillIndex]->use(this, target);
}
void CFighter::showEquippedArmor() const {
	cout << "\n=== 已裝備的裝備 ===" << endl;
	const char* positions[] = { "頭部", "身體", "手部", "腿部" };

	for (size_t i = 0; i < equipped_armors.size(); ++i) {
		cout << positions[i] << ": ";
		if (equipped_armors[i]) {
			cout << equipped_armors[i]->getName()
				<< " (防禦力 +" << equipped_armors[i]->getdefensebonus() << ")"
				<< endl;
		}
		else {
			cout << "無" << endl;
		}
	}
	cout << "防具總加成: +" << getTotalDefenseBonus() << endl;

	// 顯示套裝效果
	if (hasFullSet()) {
		ArmorSet currentSet = getCurrentSet();
		cout << "\n=== 套裝效果 ===" << endl;

		// 使用 switch 來處理不同套裝類型
		string setName;
		switch (currentSet) {
		case ArmorSet::eLeather:
			setName = "皮革";
			break;
		case ArmorSet::eSteel:
			setName = "鋼鐵";
			break;
		case ArmorSet::eHoly:
			setName = "神聖";
			break;
		default:
			setName = "無";
			break;
		}

		cout << setName << "套裝效果啟動！防禦力 +"
			<< getArmorSetBonus() << endl;
	}
}
void CFighter::addMoney(int amount) {
	money += amount;
	cout << "獲得 " << amount << " 金幣！當前金幣: " << money << endl;
}
bool CFighter::spendMoney(int amount) {
	if (money >= amount) {
		money -= amount;
		cout << "支付 " << amount << " 金幣。剩餘金幣: " << money << endl;
		return true;
	}
	cout << "金幣不足！需要 " << amount << " 金幣，當前只有 " << money << " 金幣。" << endl;
	return false;
}
void CFighter::showStatus() const {
	cout << "\n=== 角色狀態 ===" << endl;
	cout << "名稱: " << getname() << endl;

	// 職業資訊
	cout << "職業: ";
	switch (job_class) {
	case JobClass::WARRIOR:
		cout << "戰士";
		break;
	case JobClass::MAGE:
		cout << "法師";
		break;
	default:
		cout << "初心者";
		if (level >= JOB_CHANGE_LEVEL) {
			cout << " (已達轉職等級!)";
		}
	}
	cout << endl;

	// 等級和經驗值
	cout << "等級: " << level << endl;
	if (level < FIGHTER_MAX_LEVEL) {
		cout << "經驗值: " << current_exp << "/" << getNextLevelExp()
			<< " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
	}

	// 基本屬性
	cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
	cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
	cout << "攻擊力: " << getAttack() << " (基礎: " << getBaseAttack() << ")" << endl;
	cout << "防禦力: " << getDefense() << " (基礎: " << getBaseDefense() << ")" << endl;

	// 屬性點數
	cout << "\n=== 角色屬性 ===" << endl;
	cout << "未分配屬性點: " << available_points << endl;
	cout << "力量: " << strength << endl;
	cout << "智力: " << intelligence << endl;
	cout << "韌性: " << tenacity << endl;
	cout << "幸運: " << Lucky << endl;

	// 如果已轉職，顯示技能點數
	if (job_class != JobClass::NOVICE) {
		cout << "技能點數: " << skill_points << endl;
	}

	// 裝備資訊
	showWeapon();
	showEquippedArmor();
}