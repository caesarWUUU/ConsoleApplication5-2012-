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
		setname("�L�W�Ԥh");
	}

	setInitSPHP(initHP, initSP);
	Lucky = initLucky;
	cur_city = in_city;
	bag = new CBag();
	equipped_armors.fill(nullptr);
	money = 100000;
	base_attack = FIGHTER_BASE_ATTACK;
	base_defense = FIGHTER_BASE_DEFENSE;

	// ��l�Ʒs�ݩ�
	strength = 0;
	intelligence = 0;
	tenacity = 0;
	available_points = 0;

	// ��l�Ƶ��ũM�g���
	level = 15;
	current_exp = 0;

	// ��l��¾�~����
	job_class = JobClass::NOVICE;  // ��l����ߪ�
	skill_points = 0;              // ��l�ޯ��I�Ƭ�0
	equipped_skills.fill(nullptr); // ��l�Ƨޯ�ֱ��䬰��

	cout << "One Fighter is created with (maxHP, maxSP, maxLucky) = ("
		<< initHP << ", " << initSP << ", " << initLucky << ")" << endl;
	cout << "��l����: " << money << endl;
}
bool CFighter::capturePet(CMonster* monster) {
	// �ˬd�d���ƶq�W��
	if (pets.size() >= PET_MAX_COUNT) {
		cout << "�d���ƶq�w�F�W���I" << endl;
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
	cout << "\n=== �d���C�� ===" << endl;
	if (pets.empty()) {
		cout << "�ثe�S���d���C" << endl;
		return;
	}

	for (size_t i = 0; i < pets.size(); ++i) {
		cout << i + 1 << ". ";
		if (pets[i] == active_pet) {
			cout << "[�X�Ԥ�] ";
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
		cout << "���d�����B��h�Ҫ��A�A�L�k�X�ԡI" << endl;
		return false;
	}

	// �N��e�X���d���]���𮧪��A
	if (active_pet) {
		active_pet->setPetState(PetState::IDLE);
	}

	// �]�w�s���X���d��
	active_pet = selected_pet;
	active_pet->setPetState(PetState::ACTIVE);
	return true;
}
CFighter::~CFighter() {
	
	if (bag) {
		delete bag;
	}
	// �M�z�w�˳ƪ�����
	for (CArmor* armor : equipped_armors) {
		if (armor) {
			delete armor;
		}
	}
}
unsigned int CFighter::getNextLevelExp() const {
	if (level >= FIGHTER_MAX_LEVEL) return 0;

	// �ϥξ�ƭp���קK�B�I�ƻ~�t
	unsigned int required_exp = FIGHTER_BASE_EXP;
	for (int i = 1; i < level; i++) {
		required_exp = static_cast<unsigned int>(required_exp * FIGHTER_EXP_MULTIPLIER);
	}
	return required_exp;
}

void CFighter::addExp(int exp) {
	if (level >= FIGHTER_MAX_LEVEL) {
		cout << "�w�F��̰����šI" << endl;
		return;
	}

	cout << "\n��o " << exp << " �I�g��ȡI" << endl;
	current_exp += exp;

	// ��ܷ�e�g��Ȫ��A
	cout << "��e�g��ȡG" << current_exp << "/" << getNextLevelExp();
	double progress = (static_cast<double>(current_exp) / getNextLevelExp()) * 100.0;
	cout << " (" << fixed << setprecision(2) << progress << "%)" << endl;

	// �ˬd�O�_�i�H�ɯ�
	while (canLevelUp()) {
		levelUp();
		if (level < FIGHTER_MAX_LEVEL) {
			// �p�G�٦��h�l�g��ȡA��ܷs���i��
			cout << "�Ѿl�g��ȡG" << current_exp << "/" << getNextLevelExp();
			progress = (static_cast<double>(current_exp) / getNextLevelExp()) * 100.0;
			cout << " (" << fixed << setprecision(2) << progress << "%)" << endl;
		}
	}
}
ArmorSet CFighter::getCurrentSet() const {
	if (!hasFullSet()) return ArmorSet::eNoSet;  // �קאּ�ϥΧ��㪺�T�|�W��
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

	cout << "\n=== ���Ŵ���! ===" << endl;
	cout << "���Ŵ��ɦ� " << level << " ��!" << endl;

	// ��o�ݩ��I��
	available_points += 5;
	cout << "\n��o 5 �I�ݩ��I!" << endl;
	cout << "��e�����t�ݩ��I: " << available_points << endl;

	// �p�G�w�g��¾�A��o�ޯ��I��
	if (job_class != JobClass::NOVICE) {
		skill_points += 3;
		cout << "\n��o 3 �I�ޯ��I��!" << endl;
		cout << "��e�ޯ��I��: " << skill_points << endl;
	}

	// ���ܪ��a
	cout << "\n�A�i�H�ϥ� status ���O�Ӭd�ݨä��t�ݩ��I" << endl;
	if (job_class != JobClass::NOVICE) {
		cout << "�ϥ� skill ���O�i�H�d�ݩM�ɯŧޯ�" << endl;
	}
	else if (level >= JOB_CHANGE_LEVEL) {
		cout << "�A�w�F��i�H��¾�����šI�ϥ� job ���O�i����¾�C" << endl;
	}

	// ��_���庡�]
	setHP(getMAXHP());
	setSP(getMAXSP());
}

// ���o�g��ȶi�צʤ���
double CFighter::getExpProgress() const {
	if (level >= FIGHTER_MAX_LEVEL) return 100.0;
	return (double)current_exp / getNextLevelExp() * 100;
}
void CFighter::showLevelAndExp() const {
	cout << "\n=== ���Ÿ�T ===" << endl;
	cout << "����: " << level;
	if (level < FIGHTER_MAX_LEVEL) {
		cout << " (�g���: " << current_exp << "/" << getNextLevelExp()
			<< ", " << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
	}
	else {
		cout << " (�w�F�̰�����)" << endl;
	}
}
int CFighter::getAttack() const {
	int totalAttack = base_attack;
	if (getWeapon()) {
		totalAttack += getWeapon()->getattackbonus();
	}

	

	return totalAttack;
}

// �s�W���o�`���m�O�����
int CFighter::getDefense() const {
	int totalDefense = base_defense + getTotalDefenseBonus() + getArmorSetBonus();

	// �[�J¾�~�[��
	

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
		cout << this->getname () << " �r�O�@���A�y�� " << l->getname () << " " << damage << " ��l��" <<endl;			
	} else {
		cout << this->getname () << " �r�O�@���A���O " << l->getname () << " �j�O�@�סA�]���S���y������l��" <<endl;
	}
	return (damage > 0 ? damage : 0);
}

int CFighter::defense(CLifeEntity* l) {
	return getLucky() + getTotalDefenseBonus();  // �]�t���㪺���m�[��
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
	cout << this->getname() << " ��o�F " << in_item->getName() << endl;

	// �ߧY�O�s����ƾ�
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

	// �p�G�O�Z��
	if (ne->itm->isA() == eweapon) {
		CWeapon* old_weapon = this->getWeapon();

		// ����s�Z���q�I�]���X
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		// �˳Ʒs�Z��
		this->setWeapon((CWeapon*)ne->itm);
		cout << this->getname() << " �˳ƤF " << ne->itm->getName() << endl;

		// �p�G���ªZ���A��^�I�]
		if (old_weapon != NULL) {
			CBagEntry* entry = bag->item_lookup(old_weapon->isA(), old_weapon->getID());
			if (!entry) {
				bag->item_insert(old_weapon);
			}
			else {
				entry->addNum(1);
			}
			cout << this->getname() << " �N " << old_weapon->getName() << " ���^�I�]" << endl;
		}

		cout << "\n�I�]�ثe���e�G" << endl;
		this->showAllBagItems();
	}
	// �p�G�O����
	else if (ne->itm->isA() == earmor) {
		CArmor* new_armor = static_cast<CArmor*>(ne->itm);
		ArmorType type = new_armor->getArmorType();

		// �����o�¨���
		CArmor* old_armor = this->getArmor(type);

		// �q�I�]���X�s����
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		// �˳Ʒs����
		this->setArmor(new CArmor(*new_armor));
		cout << this->getname() << " �˳ƤF " << new_armor->getName() << endl;

		// �p�G���¨���A��^�I�]
		if (old_armor != NULL) {
			CBagEntry* entry = bag->item_lookup(old_armor->isA(), old_armor->getID());
			if (!entry) {
				bag->item_insert(old_armor);
			}
			else {
				entry->addNum(1);
				delete old_armor;
			}
			cout << this->getname() << " �N " << old_armor->getName() << " ���^�I�]" << endl;
		}

		cout << "\n�I�]�ثe���e�G" << endl;
		this->showAllBagItems();
	}
	// ��L���~�]�p�������^
	else {
		ne->itm->beUsed(this);
		ne->deleteNum();
		if (ne->getNum() == 0) {
			bag->item_delete(ne);
		}

		cout << "\n�I�]�ثe���e�G" << endl;
		this->showAllBagItems();
	}

	return true;
}

int CFighter::isA() const {  // �קאּ const
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
	// �ϥ� static_cast �N�T�|�ഫ������
	int typeIndex = static_cast<int>(type);

	// �p�G�ӳ���w�g������A�����U
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
	if (equipped_armors[0] == nullptr) return false;  // �������Y��

	ArmorSet baseSet = equipped_armors[0]->getArmorSet();
	if (baseSet == ArmorSet::eNoSet) return false;

	// �ˬd�Ҧ�����O�_���˳ƥB�ݩ�P�@�M��
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
	cout << "\n=== �w�˳ƪ��Z�� ===" << endl;
	if (getWeapon()) {
		cout << "�Z��: " << getWeapon()->getName()
			<< " (�����O +" << getWeapon()->getattackbonus() << ")"
			<< endl;
	}
	else {
		cout << "�Z��: �L" << endl;
	}
}
bool CFighter::changeJob(JobClass newJob) {
	if (level < 15) {
		cout << "���Ť���!�ݭn�F��15�Ť~����¾" << endl;
		return false;
	}

	if (job_class != JobClass::NOVICE) {
		cout << "�w�g��¾�L�F!" << endl;
		return false;
	}

	job_class = newJob;
	skill_points = 30;  // ��l��o3�I�ޯ��I��

	// �ھ�¾�~������l�ޯ�
	if (job_class == JobClass::WARRIOR) {
		cout << "\n=== ���\��¾���Ԥh! ===" << endl;

		// �����Ԥh��l�ޯ�
		learnSkill(new CSkill("�r�O�@��", SkillType::PHYSICAL, 20, 50, 1.2f,
			"��ĤH�y���j�O�����z�ˮ`"));
		learnSkill(new CSkill("�ԧq", SkillType::BUFF, 30, 20, 1.1f,
			"���ɦۨ������O"));

		// �W�[�Ԥh�B�~�ݩ�
		setMaxHP(getMAXHP() + 200);
		setHP(getMAXHP());
		base_attack += 30;
		base_defense += 20;

	}
	else if (job_class == JobClass::MAGE) {
		cout << "\n=== ���\��¾���k�v! ===" << endl;

		// �����k�v��l�ޯ�
		learnSkill(new CSkill("���y�N", SkillType::MAGICAL, 25, 60, 1.3f,
			"�o�g���y�y���]�k�ˮ`"));
		learnSkill(new CSkill("�v¡�N", SkillType::HEAL, 40, 40, 1.2f,
			"��_�ͩR��"));

		// �W�[�k�v�B�~�ݩ�
		setMaxSP(getMAXSP() + 150);
		setSP(getMAXSP());
		intelligence += 40;
	}

	cout << "��o�F��l�ޯ�P3�I�ޯ��I��!" << endl;
	cout << "�i�H�ϥ� skill ���O�d�ݩM�ɯŧޯ�" << endl;

	return true;
}
bool CFighter::learnSkill(CSkill* skill) {
	if (!skill) return false;
	skills.push_back(skill);
	return true;
}

bool CFighter::upgradeSkill(int skillIndex) {
	if (skillIndex < 0 || skillIndex >= skills.size()) {
		cout << "�L�Ī��ޯ����!" << endl;
		return false;
	}

	if (skill_points <= 0) {
		cout << "�ޯ��I�Ƥ���!" << endl;
		return false;
	}

	if (skills[skillIndex]->levelUp()) {
		skill_points--;
		cout << "�Ѿl�ޯ��I��: " << skill_points << endl;
		return true;
	}

	return false;
}
void CFighter::showSkills() const {
	cout << "\n=== �ޯ�C�� ===" << endl;
	cout << "¾�~: " << (job_class == JobClass::WARRIOR ? "�Ԥh" :
		job_class == JobClass::MAGE ? "�k�v" : "��ߪ�") << endl;
	cout << "�ޯ��I��: " << skill_points << endl;
	cout << "\n�߱o���ޯ�:" << endl;

	for (size_t i = 0; i < skills.size(); ++i) {
		const CSkill* skill = skills[i];
		cout << i + 1 << ". " << skill->getName()
			<< " Lv." << skill->getLevel()
			<< " (SP����: " << skill->getSpCost() << ")"
			<< "\n   " << skill->getDescription() << endl;
	}

	cout << "\n�˳ƨ�ֱ��䪺�ޯ�:" << endl;
	const char* keys[] = { "Z", "X", "C" };
	for (size_t i = 0; i < equipped_skills.size(); ++i) {
		cout << keys[i] << ": ";
		if (equipped_skills[i]) {
			cout << equipped_skills[i]->getName() << endl;
		}
		else {
			cout << "�L" << endl;
		}
	}
}
bool CFighter::useSkill(int skillIndex, CLifeEntity* target) {
	if (skillIndex < 0 || skillIndex >= skills.size()) {
		cout << "�L�Ī��ޯ����!" << endl;
		return false;
	}

	return skills[skillIndex]->use(this, target);
}
void CFighter::showEquippedArmor() const {
	cout << "\n=== �w�˳ƪ��˳� ===" << endl;
	const char* positions[] = { "�Y��", "����", "�ⳡ", "�L��" };

	for (size_t i = 0; i < equipped_armors.size(); ++i) {
		cout << positions[i] << ": ";
		if (equipped_armors[i]) {
			cout << equipped_armors[i]->getName()
				<< " (���m�O +" << equipped_armors[i]->getdefensebonus() << ")"
				<< endl;
		}
		else {
			cout << "�L" << endl;
		}
	}
	cout << "�����`�[��: +" << getTotalDefenseBonus() << endl;

	// ��ܮM�ˮĪG
	if (hasFullSet()) {
		ArmorSet currentSet = getCurrentSet();
		cout << "\n=== �M�ˮĪG ===" << endl;

		// �ϥ� switch �ӳB�z���P�M������
		string setName;
		switch (currentSet) {
		case ArmorSet::eLeather:
			setName = "�֭�";
			break;
		case ArmorSet::eSteel:
			setName = "���K";
			break;
		case ArmorSet::eHoly:
			setName = "���t";
			break;
		default:
			setName = "�L";
			break;
		}

		cout << setName << "�M�ˮĪG�ҰʡI���m�O +"
			<< getArmorSetBonus() << endl;
	}
}
void CFighter::addMoney(int amount) {
	money += amount;
	cout << "��o " << amount << " �����I��e����: " << money << endl;
}
bool CFighter::spendMoney(int amount) {
	if (money >= amount) {
		money -= amount;
		cout << "��I " << amount << " �����C�Ѿl����: " << money << endl;
		return true;
	}
	cout << "���������I�ݭn " << amount << " �����A��e�u�� " << money << " �����C" << endl;
	return false;
}
void CFighter::showStatus() const {
	cout << "\n=== ���⪬�A ===" << endl;
	cout << "�W��: " << getname() << endl;

	// ¾�~��T
	cout << "¾�~: ";
	switch (job_class) {
	case JobClass::WARRIOR:
		cout << "�Ԥh";
		break;
	case JobClass::MAGE:
		cout << "�k�v";
		break;
	default:
		cout << "��ߪ�";
		if (level >= JOB_CHANGE_LEVEL) {
			cout << " (�w�F��¾����!)";
		}
	}
	cout << endl;

	// ���ũM�g���
	cout << "����: " << level << endl;
	if (level < FIGHTER_MAX_LEVEL) {
		cout << "�g���: " << current_exp << "/" << getNextLevelExp()
			<< " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
	}

	// ���ݩ�
	cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
	cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
	cout << "�����O: " << getAttack() << " (��¦: " << getBaseAttack() << ")" << endl;
	cout << "���m�O: " << getDefense() << " (��¦: " << getBaseDefense() << ")" << endl;

	// �ݩ��I��
	cout << "\n=== �����ݩ� ===" << endl;
	cout << "�����t�ݩ��I: " << available_points << endl;
	cout << "�O�q: " << strength << endl;
	cout << "���O: " << intelligence << endl;
	cout << "����: " << tenacity << endl;
	cout << "���B: " << Lucky << endl;

	// �p�G�w��¾�A��ܧޯ��I��
	if (job_class != JobClass::NOVICE) {
		cout << "�ޯ��I��: " << skill_points << endl;
	}

	// �˳Ƹ�T
	showWeapon();
	showEquippedArmor();
}