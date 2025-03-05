#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <iomanip>
#include <limits>
#include <utility>
#include <vector>
#include <algorithm>
#include "Fighter.h"
#include "def.h"
#include "CmdParser.h"
#include "GlobalInfo.h"
#include "User.h"
#include "MapData.h"
#include "Monster.h"
#include "Place.h"
#include "ItemData.h"
#include "Weapon.h"
#include "food.h"
#include "armor.h"
#define NOMINMAX  // ���� windows.h �w�q min �M max 
#include <windows.h>
#include "Skill.h"
#include "SaveSystem.h"
#include "AccountManager.h"
#include "SaveDirectoryManager.h"

using namespace std;


int function_exit(vector<string>& tokens) {
	return -1;
}

int function_next_direction(vector<string>& tokens) {
	if (tokens.size() != 1) {
		cout << "���ʫ��O�榡���~" << endl;
		return 0;
	}

	int next_dir = -1;
	string dir = tokens[0];

	if (dir == "east" || dir == "d" || dir == "D") {
		next_dir = eEast;
	}
	else if (dir == "west" || dir == "a" || dir == "A") {
		next_dir = eWest;
	}
	else if (dir == "north" || dir == "w" || dir == "W") {
		next_dir = eNorth;
	}
	else if (dir == "south" || dir == "s" || dir == "S") {
		next_dir = eSouth;
	}
	else if (dir == "eastnorth") {
		next_dir = eEastNorth;
	}
	else if (dir == "eastsouth") {
		next_dir = eEastSouth;
	}
	else if (dir == "westnorth") {
		next_dir = eWestNorth;
	}
	else if (dir == "westsouth") {
		next_dir = eWestSouth;
	}
	else {
		cout << "�L�Ī����ʤ�V" << endl;
		return 0;
	}

	int next_city = CGlobalInfo::user->goto_next_city(next_dir);
	if (next_city > 0) {
		system("cls");
		CGlobalInfo::map_data->show_description(next_city);

		cout << "\n�i���ʤ�V�G" << endl;
		CPlace* current_place = CGlobalInfo::map_data->get_place_by_id(next_city);
		if (current_place) {
			vector<int> available_directions = current_place->get_available_directions();
			for (int dir : available_directions) {
				switch (dir) {
				case eNorth: cout << "�_��(W) "; break;
				case eEast: cout << "�F��(D) "; break;
				case eSouth: cout << "�n��(S) "; break;
				case eWest: cout << "���(A) "; break;
				}
			}
			cout << endl;
		}
	}
	else {
		cout << "�L�k���Ӥ�V���ʡI�нT�{���|�O�_�s�b�C" << endl;
	}
	return 0;
}

int function_list(vector<string>& tokens) {
	if (tokens.size() != 1) {
		for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
			cerr << (*it) << " ";
		}
		cerr << " command error" << endl;
		return 0;
	}
	int city = CGlobalInfo::user->get_current_city();
	CPlace* cityptr = CGlobalInfo::map_data->get_place_by_id(city);
	if (cityptr) {
		cityptr->show_mosters();
	}
	return 0;
}

int function_kill(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "���O�榡�Gkill [�Ǫ��W��]" << endl;
		cout << "�Ҧp�Gkill cat3 �� kill rabbit1" << endl;
		return 0;
	}

	string monster_engname = tokens[1];
	int city = CGlobalInfo::user->get_current_city();
	CPlace* cityptr = CGlobalInfo::map_data->get_place_by_id(city);
	assert(cityptr);

	// ���ˬd��e��m�����ǩǪ�
	cout << "��e��m���Ǫ��G" << endl;
	cityptr->show_mosters();

	CMonster* monster = cityptr->get_monster_by_engname(monster_engname);
	if (!monster) {
		cout << "�䤣����w���Ǫ��C�Шϥ� ls ���O�d�ݷ�e��m���Ǫ��C��C" << endl;
		cout << "�`�N�G�ШϥΩǪ����^��W�٥[�W�s���]�Ҧp�Gcat3�^�Ӷi��԰��C" << endl;
		return 0;
	}

	// �i�J�԰��Ҧ�
	CGlobalInfo::parser->enter_battle_mode(monster);
	return 0;
}
int function_move(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "���ʫ��O�榡�Gmove [�a�I�W��]" << endl;
		return 0;
	}

	string destination = tokens[1];

	// �����q�L�a�I�W�٧�����ID
	int dest_id = CGlobalInfo::map_data->get_place_id_by_name(destination);

	if (dest_id < 0) {
		cout << "�䤣��Ӧa�I�I" << endl;
		return 0;
	}

	// ������s���a��m�����
	CFighter* fighter = (CFighter*)CGlobalInfo::user->get_user();
	fighter->set_current_city(dest_id);
	CGlobalInfo::map_data->show_description(dest_id);

	return 0;
}
int function_check_bag(vector<string>& tokens) {
	if (tokens.size() != 1) {
		for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
			cerr << (*it) << " ";
		}
		cerr << " command error" << endl;
		return 0;
	}
	CLifeEntity* usr = CGlobalInfo::user->get_user();
	assert(usr);
	if (usr->isA() == efighter) {
		CFighter* f = (CFighter*)usr;
		cout << f->getname() << " ���}�I�]" << endl;
		
		cout << "\n=== �I�]���e ===" << endl ;
		
		int num = f->showAllBagItems();
		cout << endl;
		cout << "�ثe����: " << f->getMoney() << endl << endl;  // �s�W�o��A��ܪ����ƶq
		if (num == 0) {
			cout << "�I�]�ŪŦp�]" << endl;
			cout << f->getname() << " ���W�I�]" << endl;
			return 0;
		}

		int selection = 0;
		cout << "�n�ϥΤ��򪫫~ (0��ܤ��n)" << endl;
		cin >> selection;
		while (selection > 0) {
			if (!f->useBagItems(selection)) {
				cout << "���ﶵ���s�b" << endl;
			}
			selection = 0;
			cout << "�n�ϥΤ��򪫫~ (0��ܤ��n)" << endl;
			cin >> selection;
		}
		cout << f->getname() << " ���W�I�]" << endl;
	}
	cin.clear();
	cin.ignore(1024, '\n');
	return 0;
}

int function_status(vector<string>& tokens) {
	if (!CGlobalInfo::user) return 0;

	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) return 0;

	CFighter* fighter = (CFighter*)player;
	fighter->showStatus();

	// �p�G���a�������t�I��,�i�J�[�I�`��
	while (fighter->getAvailablePoints() > 0) {
		cout << "\n=== ���t�ݩ��I ===" << endl;
		cout << "�����t�I��: " << fighter->getAvailablePoints() << endl;
		cout << "1) �O�q (+2�����O)" << endl;
		cout << "2) ���O (+5�]�O)" << endl;
		cout << "3) ��� (+10��q)" << endl;
		cout << "4) ���B (+1%�g������)" << endl;
		cout << "0) ��^" << endl;

		cout << "\n�п�ܭn�[�I���ݩ� (0-4): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) break;  // ���a��ܰh�X�[�I

		if (choice < 1 || choice > 4) {
			cout << "�L�Ī����!" << endl;
			continue;
		}

		cout << "�n�[�h���I? (��e�i���I��: " << fighter->getAvailablePoints() << "): ";
		int points;
		cin >> points;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (points <= 0) {
			cout << "�L�Ī��I��" << endl;
			continue;
		}

		if (points > fighter->getAvailablePoints()) {
			cout << "�i���I�Ƥ���" << endl;
			continue;
		}

		// �ھڿ�ܤ��t�I��
		bool success = true;
		switch (choice) {
		case 1: // �O�q
			fighter->addStrength(points);
			cout << "�O�q�W�[ " << points << " �I" << endl;
			break;
		case 2: // ���O
			fighter->addIntelligence(points);
			cout << "���O�W�[ " << points << " �I" << endl;
			break;
		case 3: // ���
			fighter->addTenacity(points);
			cout << "��ݼW�[ " << points << " �I" << endl;
			break;
		case 4: // ���B
			// �ˬd�O�_�W�L100%�W��
			if (fighter->getLucky() + points > 100) {
				cout << "���B�Ȥ���W�L100" << endl;
				success = false;
				continue;
			}
			fighter->addLucky(points);
			cout << "���B�W�[ " << points << " �I" << endl;
			break;
		}

		if (success) {
			// �����w�ϥΪ��I��
			fighter->addAvailablePoints(-points);

			// ��s���A
			cout << "\n=== ��s�᪺���A ===" << endl;
			fighter->showStatus();
		}
	}

	if (fighter->getAvailablePoints() == 0) {
		cout << "\n�w�L�i���ݩ��I" << endl;
	}

	return 0;
}

int function_shop_weapon(vector<string>& tokens) {
	// �ˬd�O�_�b����
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "�����b�����~��ϥΪZ���ө��I" << endl;
		return 0;
	}

	// �ˬd��������a��H
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "�L�k�X�ݰө��I" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== �w����{�Z���ө� ===" << endl;
	cout << "�A������: " << fighter->getMoney() << endl;
	cout << "�s��  �Z���W��    �����[��    ����" << endl;
	cout << "--------------------------------" << endl;

	// Ū���Z�����
	ifstream fin("weapon.txt");
	if (!fin) {
		cout << "�L�k�}�ҪZ�������" << endl;
		return 0;
	}

	vector<pair<CWeapon*, int>> weapons;
	int id;
	string name;
	int attack;

	// Ū������ܪZ���C��
	while (fin >> id >> name >> attack) {
		CWeapon* weapon = new CWeapon(name, 0, 0, id, attack);
		int price = attack * 100;  // ���� = �����O * 100
		weapons.push_back(make_pair(weapon, price));
		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << attack << "  "
			<< setw(8) << price << " ����" << endl;
	}
	fin.close();

	// �ʶR�`��
	while (true) {
		cout << "\n�ثe����: " << fighter->getMoney() << endl;
		cout << "�п�ܭn�ʶR���Z���s�� (��J0�h�X�ө�): ";
		int choice;
		cin >> choice;
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// �M���ܪ��Z��
		CWeapon* selected_weapon = nullptr;
		int price = 0;
		for (const auto& weapon_pair : weapons) {
			if (weapon_pair.first->getID() == choice) {
				selected_weapon = weapon_pair.first;
				price = weapon_pair.second;
				break;
			}
		}

		if (selected_weapon) {
			cout << "\n=== �ʶR�T�{ ===" << endl;
			cout << "�Z���W��: " << selected_weapon->getName() << endl;
			cout << "�����[��: +" << selected_weapon->getattackbonus() << endl;
			cout << "��    ��: " << price << " ����" << endl;
			cout << "�A������: " << fighter->getMoney() << endl;

			if (fighter->getMoney() < price) {
				cout << "���������I�ٻݭn " << (price - fighter->getMoney()) << " ����" << endl;
				continue;
			}

			cout << "�T�w�ʶR�ܡH(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // ��������
					fighter->captureItem(new CWeapon(*selected_weapon));  // �K�[��I�]
					cout << "������\�I��o " << selected_weapon->getName() << endl;
					cout << "�Ѿl����: " << fighter->getMoney() << endl;
				}
			}
			else {
				cout << "�����ʶR�C" << endl;
			}
		}
		else {
			cout << "�L�Ī��Z���s���I�Э��s��ܡC" << endl;
		}
	}

	// �M�z�귽
	for (auto& weapon_pair : weapons) {
		delete weapon_pair.first;
	}
	weapons.clear();

	cout << "\n�w��U�����{�I" << endl;
	return 0;
}

int function_shop_armor(vector<string>& tokens) {
	// �ˬd�O�_�b����
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "�����b�����~��ϥΨ���ө��I" << endl;
		return 0;
	}

	// �ˬd��������a��H
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "�L�k�X�ݰө��I" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== �w����{����ө� ===" << endl;
	cout << "�A������: " << fighter->getMoney() << endl;
	cout << "�s��  ����W��    ���m�[��    ����        ����" << endl;
	cout << "------------------------------------------------" << endl;

	// Ū��������
	ifstream fin("armor.txt");
	if (!fin) {
		cout << "�L�k�}�Ҩ�������" << endl;
		return 0;
	}

	vector<pair<CArmor*, int>> armors;
	int id;
	string name;
	int defense;
	int type;

	// ����W�ٹ���
	const string position_names[] = { "�Y��", "����", "�ⳡ", "�}��" };

	// Ū������ܨ���C��
	while (fin >> id >> name >> defense >> type) {
		CArmor* armor = new CArmor(name, 0, 0, id, defense, static_cast<ArmorType>(type));
		int price = defense * 200;  // ���� = ���m�O * 200
		armors.push_back(make_pair(armor, price));

		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << defense << "  "
			<< setw(8) << position_names[static_cast<int>(armor->getArmorType())] << "  "  // �o�̶i�������ഫ
			<< setw(8) << price << " ����" << endl;
	}
	fin.close();

	// ��ܷ�e�˳�
	cout << "\n=== ��e�˳ƪ��A ===" << endl;
	fighter->showEquippedArmor();

	// �ʶR�`��
	while (true) {
		cout << "\n�ثe����: " << fighter->getMoney() << endl;
		cout << "�п�ܭn�ʶR������s�� (��J0�h�X�ө�): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// �M���ܪ�����
		CArmor* selected_armor = nullptr;
		int price = 0;
		for (const auto& armor_pair : armors) {
			if (armor_pair.first->getID() == choice) {
				selected_armor = armor_pair.first;
				price = armor_pair.second;
				break;
			}
		}

		if (selected_armor) {
			cout << "\n=== �ʶR�T�{ ===" << endl;
			cout << "����W��: " << selected_armor->getName() << endl;
			cout << "���m�[��: +" << selected_armor->getdefensebonus() << endl;
			cout << "�˳Ƴ���: " << position_names[static_cast<int>(selected_armor->getArmorType())] << endl;  // �o�̶i�������ഫ
			cout << "��    ��: " << price << " ����" << endl;
			cout << "�A������: " << fighter->getMoney() << endl;

			// �ˬd�O�_�w�˳ƦP��������
			CArmor* equipped = fighter->getArmor(selected_armor->getArmorType());
			if (equipped) {
				cout << "ĵ�i�G�ӳ���w�˳� " << equipped->getName()
					<< " (���m�O +" << equipped->getdefensebonus() << ")" << endl;
			}

			if (fighter->getMoney() < price) {
				cout << "���������I�ٻݭn " << (price - fighter->getMoney()) << " ����" << endl;
				continue;
			}

			cout << "�T�w�ʶR�ܡH(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // ��������
					fighter->captureItem(new CArmor(*selected_armor));  // �K�[��I�]
					cout << "������\�I��o " << selected_armor->getName() << endl;
					cout << "�Ѿl����: " << fighter->getMoney() << endl;
					cout << "���ܡG�i�H�ϥέI�](checkbag)�Ӹ˳Ʒs�ʶR������" << endl;
				}
			}
			else {
				cout << "�����ʶR�C" << endl;
			}
		}
		else {
			cout << "�L�Ī�����s���I�Э��s��ܡC" << endl;
		}
	}

	// �M�z�귽
	for (auto& armor_pair : armors) {
		delete armor_pair.first;
	}
	armors.clear();

	cout << "\n�w��U�����{�I" << endl;
	return 0;
}
int function_show_pets(vector<string>& tokens) {
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "���~: �L�k�����d�����O!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;

	// ��ܩҦ��d���C��
	fighter->showPets();
	return 0;
}
int function_pet_status(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "���O�榡�Gpetstatus [�d���s��]" << endl;
		return 0;
	}

	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "���~: �L�k�����d�����O!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;
	int pet_index = std::stoi(tokens[1]) - 1;  // �N��J���s���ഫ������

	const vector<CPet*>& pets = fighter->getPets();
	if (pet_index < 0 || pet_index >= pets.size()) {
		cout << "�L�Ī��d���s���I" << endl;
		return 0;
	}

	// ��ܫ��w�d�����ԲӪ��A
	pets[pet_index]->showStatus();
	return 0;
}
int function_switch_pet(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "���O�榡�Gswitchpet [�d���s��]" << endl;
		return 0;
	}

	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "���~: �L�k�����d�����O!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;
	int pet_index = std::stoi(tokens[1]) - 1;

	if (fighter->switchPet(pet_index)) {
		cout << "���\�����X���d���I" << endl;
		fighter->getActivePet()->showStatus();
	}
	else {
		cout << "�����d�����ѡI�нT�{�d���s�����T�B�d�����A���`�C" << endl;
	}
	return 0;
}

int function_shop_food(vector<string>& tokens) {
	// �ˬd�O�_�b����
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "�����b�����~��ϥέ���I" << endl;
		return 0;
	}

	// �ˬd��������a��H
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "�L�k�X�ݭ���I" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== �w����{���� ===" << endl;
	cout << "�A������: " << fighter->getMoney() << endl;
	cout << "�A���ͩR��: " << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
	cout << "�s��  �����W��    ��_HP    ����" << endl;
	cout << "--------------------------------" << endl;

	// Ū���������
	ifstream fin("food.txt");
	if (!fin) {
		cout << "�L�k�}�ҭ��������" << endl;
		return 0;
	}

	vector<pair<CFood*, int>> foods;
	int id;
	string name;
	int hp_bonus;

	// Ū������ܭ����C��
	while (fin >> id >> name >> hp_bonus) {
		CFood* food = new CFood(name, 0, 0, id, hp_bonus);
		int price = hp_bonus * 50;  // ���� = ��_�q * 50
		foods.push_back(make_pair(food, price));
		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << hp_bonus << "  "
			<< setw(8) << price << " ����" << endl;
	}
	fin.close();

	// �ʶR�`��
	while (true) {
		cout << "\n�ثe���A�G" << endl;
		cout << "����: " << fighter->getMoney() << endl;
		cout << "�ͩR��: " << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
		cout << "\n�п�ܭn�ʶR�������s�� (��J0�h�X����): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// �M���ܪ�����
		CFood* selected_food = nullptr;
		int price = 0;
		for (const auto& food_pair : foods) {
			if (food_pair.first->getID() == choice) {
				selected_food = food_pair.first;
				price = food_pair.second;
				break;
			}
		}

		if (selected_food) {
			cout << "\n=== �ʶR�T�{ ===" << endl;
			cout << "�����W��: " << selected_food->getName() << endl;
			cout << "��_�ĪG: +" << selected_food->gethpbonus() << " HP" << endl;
			cout << "��    ��: " << price << " ����" << endl;
			cout << "�A������: " << fighter->getMoney() << endl;

			// �ˬd��q�O�_�w��
			if (fighter->getHP() >= fighter->getMAXHP()) {
				cout << "�����G�A���ͩR�Ȥw�g�O�����F" << endl;
			}

			if (fighter->getMoney() < price) {
				cout << "���������I�ٻݭn " << (price - fighter->getMoney()) << " ����" << endl;
				continue;
			}

			cout << "�T�w�ʶR�ܡH(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // ��������
					fighter->captureItem(new CFood(*selected_food));  // �K�[��I�]
					cout << "������\�I��o " << selected_food->getName() << endl;
					cout << "�Ѿl����: " << fighter->getMoney() << endl;

					// �߰ݬO�_�ߧY�ϥ�
					cout << "�O�_�ߧY�ϥΡH(Y/N): ";
					cin >> confirm;
					cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

					if (toupper(confirm) == 'Y') {
						vector<string> bag_cmd = { "checkbag" };
						function_check_bag(bag_cmd);  // �������}�I�]
					}
					else {
						cout << "�A�i�H�y��ϥέI�](checkbag)�Өϥέ���" << endl;
					}
				}
			}
			else {
				cout << "�����ʶR�C" << endl;
			}
		}
		else {
			cout << "�L�Ī������s���I�Э��s��ܡC" << endl;
		}
	}

	// �M�z�귽
	for (auto& food_pair : foods) {
		delete food_pair.first;
	}
	foods.clear();

	cout << "\n�w��U�����{�I" << endl;
	return 0;
}


CCmdParser::CCmdParser() {
	// �즳�����O�M�g
	mappingfunc[string("exit")] = (void(*)(vector<string>&))function_exit;
	mappingfunc[string("east")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("eastnorth")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("eastsouth")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("west")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("westsouth")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("westnorth")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("north")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("south")] = (void(*)(vector<string>&))function_next_direction;
	mappingfunc[string("status")] = (void(*)(vector<string>&))function_status;
	mappingfunc[string("ls")] = (void(*)(vector<string>&))function_list;
	mappingfunc[string("kill")] = (void(*)(vector<string>&))function_kill;
	mappingfunc[string("checkbag")] = (void(*)(vector<string>&))function_check_bag;
	mappingfunc[string("shopA")] = (void(*)(vector<string>&))function_shop_weapon;
	mappingfunc[string("shopB")] = (void(*)(vector<string>&))function_shop_armor;
	mappingfunc[string("shopC")] = (void(*)(vector<string>&))function_shop_food;
	mappingfunc[string("move")] = (void(*)(vector<string>&))function_move;

	// �s�W��¾�M�ޯ�������O
	mappingfunc[string("job")] = (void(*)(vector<string>&))function_job_change;
	mappingfunc[string("skill")] = (void(*)(vector<string>&))function_skill_management;

	mappingfunc[string("showpets")] = (void(*)(vector<string>&))function_show_pets;
	mappingfunc[string("petstatus")] = (void(*)(vector<string>&))function_pet_status;
	mappingfunc[string("switchpet")] = (void(*)(vector<string>&))function_switch_pet;

	// �԰����O
	battle_commands[string("z")] = (void(*)(vector<string>&))battle_attack;
	battle_commands[string("x")] = (void(*)(vector<string>&))battle_skill;
	battle_commands[string("c")] = (void(*)(vector<string>&))battle_ultimate;
	// �K�[�n�X�R�O
	mappingfunc[string("logout")] = (void(*)(vector<string>&))function_logout;

	current_mode = MODE_NORMAL;
	current_enemy = nullptr;
}

void CCmdParser::splitstring(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find_first_of(c);
	pos1 = 0;
	while (string::npos != pos2){
		if(pos2-pos1)
			v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = s.find_first_of(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}
void CCmdParser::show_help() {
	cout << "\n=== ���O���� ===" << endl;
	cout << "���ʫ��O:" << endl;
	cout << "  W/north - ���_����" << endl;
	cout << "  S/south - ���n����" << endl;
	cout << "  A/west - ���貾��" << endl;
	cout << "  D/east - ���F����" << endl;
	cout << "  move [�a�I�W��] - �������ʨ���w�a�I" << endl;

	cout << "\n�԰�����:" << endl;
	cout << "  ls - �C�X��e��m���Ǫ�" << endl;
	cout << "  kill [�Ǫ��W��] - �������w�Ǫ�" << endl;

	cout << "\n�������:" << endl;
	cout << "  status - �d�ݪ��A" << endl;
	cout << "  checkbag - �d�ݭI�]" << endl;
	cout << "  job - ��¾�t�Ρ]�ݭn����15�^" << endl;
	cout << "  skill - �ޯ�޲z�]�ݭn����¾�^" << endl;

	cout << "\n�ө������]�Ȧb�ө��i�Ρ^:" << endl;
	cout << "  shopA - �}�ҪZ���ө�" << endl;
	cout << "  shopB - �}�Ҩ���ө�" << endl;
	cout << "  shopC - �}�ҭ����ө�" << endl;

	cout << "\n��L���O:" << endl;
	cout << "  help - ��ܦ�����" << endl;
	cout << "  exit - ���}�C��" << endl;

	cout << "\n�d������:" << endl;
	cout << "  showpets - ��ܩҦ��d���C��" << endl;
	cout << "  petstatus [�s��] - �d�ݫ��w�d�����ԲӪ��A" << endl;
	cout << "  switchpet [�s��] - �����X���d��" << endl;

	cout << "\n�b������:" << endl;
	cout << "  logout/quit - �O�s��ƨõn�X�^��n�J�e��" << endl;

	cout << "\n��e��m: " << CGlobalInfo::map_data->get_place_name(CGlobalInfo::user->get_current_city()) << endl;
}
int CCmdParser::process_wasd(const string& direction) {
	vector<string> tokens;
	tokens.resize(1);  // �T�O���@�Ӥ���

	if (direction == "w" || direction == "W") {
		tokens[0] = "north";
	}
	else if (direction == "s" || direction == "S") {
		tokens[0] = "south";
	}
	else if (direction == "a" || direction == "A") {
		tokens[0] = "west";
	}
	else if (direction == "d" || direction == "D") {
		tokens[0] = "east";
	}
	else {
		return 0;  // �L�Ī���J
	}

	return function_next_direction(tokens);
}


void CCmdParser::enter_battle_mode(CMonster* enemy) {
	current_mode = MODE_BATTLE;
	current_enemy = enemy;
	system("cls");  // �M��
	cout << "========== �԰��}�l ==========" << endl;
	cout << "���G" << enemy->get_basic_data() << endl;
	show_battle_help();
}
void CCmdParser::exit_battle_mode() {
	current_mode = MODE_NORMAL;
	current_enemy = nullptr;

	cout << "========== �԰����� ==========" << endl;
	Sleep(500);
	cout << "\n�����N��H��^�a��..." << endl;

	// ���ݨϥΪ̫��U���N��
	system("pause > nul");

	// �M���e������ܦa��
	
}
void CCmdParser::show_battle_help() {
	CFighter* player = (CFighter*)CGlobalInfo::user->get_user();

	cout << "\n=== �԰����O���� ===" << endl;
	cout << "Z - ���q����" << endl;

	if (player->getJobClass() == JobClass::NOVICE) {
		// ��ߪ���ܰ򥻧ޯ�W��
		cout << "X - �ޯ���� (SP: 15) - ���������A�����|�y���z��" << endl;
		cout << "C - ���ۧ��� (SP: 30) - ��O�z�o�A��_�ͩR�óy���ˮ`" << endl;
	}
	else {
		// �w��¾������
		CSkill* skillX = player->getEquippedSkill(1);
		if (skillX) {
			// �p�G���]�mX��ޯ�A��ܸӧޯ�
			cout << "X - " << skillX->getName() << " (SP: " << skillX->getSpCost() << ")" << endl;
		}
		else {
			// �S���]�m�h��ܹw�]�W��
			cout << "X - �ޯ����" << endl;
		}

		CSkill* skillC = player->getEquippedSkill(2);
		if (skillC) {
			// �p�G���]�mC��ޯ�A��ܸӧޯ�
			cout << "C - " << skillC->getName() << " (SP: " << skillC->getSpCost() << ")" << endl;
		}
		else {
			// �S���]�m�h��ܹw�]�W��
			cout << "C - ���ۧ���" << endl;
		}
	}

	cout << "help - ��ܦ�����" << endl;
}

int CCmdParser::process_battle_command(const string& cmd) {
	// �N�R�O�ഫ���p�g�i��B�z
	string lower_cmd = cmd;
	transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::tolower);

	if (lower_cmd == "help") {
		show_battle_help();
		return 0;
	}

	map<string, void(*)>::iterator it = battle_commands.find(lower_cmd);
	if (it == battle_commands.end()) {
		cout << "�L�Ī��԰����O�C��J help �d�ݥi�Ϋ��O�C" << endl;
		return 0;
	}

	vector<string> tokens;
	tokens.push_back(lower_cmd);
	return (reinterpret_cast<int(*)(vector<string>&)>(it->second)(tokens));
}


// �ק� query ��ƨӤ䴩�԰��Ҧ�
int CCmdParser::query() {
	string sentence;
	getline(cin, sentence);
	vector<string> tokens;
	splitstring(sentence, tokens, string(" "));
	if (tokens.size() == 0) {
		return 0;
	}

	// �ھڷ�e�Ҧ��B�z���O
	if (current_mode == MODE_BATTLE) {
		return process_battle_command(tokens[0]);
	}

	// �@��Ҧ������O�B�z
	if (tokens[0].length() == 1 &&
		(tokens[0] == "w" || tokens[0] == "W" ||
			tokens[0] == "a" || tokens[0] == "A" ||
			tokens[0] == "s" || tokens[0] == "S" ||
			tokens[0] == "d" || tokens[0] == "D")) {
		return process_wasd(tokens[0]);
	}
	if (tokens[0] == "help") {
		show_help();
		return 0;
	}

	map<string, void(*)>::iterator it = mappingfunc.find(tokens[0]);
	if (it == mappingfunc.end()) {
		cout << "�L�Ī����O�C��J help �d�ݥi�Ϋ��O�C" << endl;
		return 0;
	}
	return (reinterpret_cast<int(*)(const vector<string>&)>(it->second)(tokens));
}
void handle_game_over() {
	system("cls");  // �M���e��

	cout << "\n\n";
	cout << "==================== �iGame Over�j ====================\n\n";
	Sleep(1000);

	cout << "�i�̥H�L�Ȫ��ߪﱵ�R�B���D�ԡA�P�j�j���ĤH�i�}���i���C\n";
	Sleep(1000);
	cout << "�M�ӡA�R�B�L���A�L���ͩR�����רs�����b�Գ��W�C\n";
	Sleep(1000);
	cout << "\n���M�L���ȵ{�N�������A���L���i��P�믫�N�û��ʨ�b�o���j�a���W�C\n";
	Sleep(1000);
	cout << "�]�\�Y�@�ѡA�s���_�I�̱N�B�_�L����ӡA�~��Ѽg�������g���C\n\n";
	Sleep(1000);

	cout << "�����N�䵲���C��..." << endl;
	system("pause > nul");  // ���ݫ����J
	exit(0);  // �����{��
}
bool CCmdParser::executePetAttack(CMonster* enemy, CFighter* player) {
	// ��¦�ˬd
	if (!enemy || enemy->isdead() || !player) {
		return false;
	}

	// �N�Ҧ��d�������ާ@�]�b try block ��
	try {
		// ���o�d������
		CPet* active_pet = nullptr;
		try {
			active_pet = player->getActivePet();
			if (!active_pet) {
				return false;
			}

			// �ϥΥi���Ҫ��O�����m���ˬd���Ъ����ĩ�
			volatile void* ptr = active_pet;
			if (ptr == nullptr ||
				reinterpret_cast<uintptr_t>(ptr) == 0xCDCDCDCD ||
				reinterpret_cast<uintptr_t>(ptr) < 0x1000) {
				return false;
			}

		}
		catch (...) {
			return false;
		}

		// �i���d���l��
		cout << "\n=== �d���l�� ===" << endl;
		Sleep(500);

		// �ϥΩT�w����¦�ˮ`
		int pet_damage = 3;

		cout << "�d���o�ʰl���I" << endl;
		Sleep(800);

		enemy->gethurt(pet_damage);
		cout << "�y�� " << pet_damage << " �I�ˮ`�I" << endl;
		Sleep(1000);

		return true;
	}
	catch (...) {
		return false;
	}
}

void handle_battle_end(CFighter* player, CMonster* enemy) {
	if (enemy->isdead()) {
		cout << "\n" << enemy->getname() << " �Q���ˤF�I" << endl;
		Sleep(1000);
		cout << "��o�԰��ӧQ�I" << endl;
		Sleep(1000);
		// ���ծ���
		cout << "\n���ծ����Ǫ�..." << endl;
		Sleep(800);

		// �p�⮷����v�]�ثe���ն��q�]��100%�^
		bool can_capture = true;  // ����i�H�אּ�ھڵ��Ůt�BHP��ҵ��p��

		if (can_capture && player->capturePet(enemy)) {
			cout << "���\����F " << enemy->getname() << "�I" << endl;
			Sleep(500);
			player->showPets();
		}
		// �p��g��ȼ��y
		int exp_reward = enemy->getLevel() * 5;  // �C�ŵ��� 5 �I�g���
		if (exp_reward <= 0) exp_reward = 1;
		if (exp_reward > 300) exp_reward = 300;  // �]�w�W���קK�L�q

		// �����g���
		player->addExp(exp_reward);
		Sleep(1000);

		// �ˬd�O�_���X���d���A�p�G���]�����g���
		if (CPet* active_pet = player->getActivePet()) {
			cout << "\n�X���d�� " << active_pet->getNickname() << " ��o�g��ȡI" << endl;
			Sleep(500);
			active_pet->addExp(exp_reward);
			Sleep(1000);
		}

		// �p��������y
		int base_gold = 20;                          // ��¦����
		int level_bonus = enemy->getLevel() * 5;     // ���ť[��
		int random_bonus = rand() % (enemy->getLevel() * 2); // �H���i��
		int total_gold = base_gold + level_bonus + random_bonus;

		cout << "\n�M��ԧQ�~..." << endl;
		Sleep(1000);
		cout << "��o " << total_gold << " �����I" << endl;
		player->addMoney(total_gold);
		Sleep(1500);

		// �����԰�
		CGlobalInfo::parser->exit_battle_mode();
		system("cls");
		int current_city = CGlobalInfo::user->get_current_city();
		CPlace* current_place = CGlobalInfo::map_data->get_place_by_id(current_city);
		if (current_place) {
			current_place->remove_moster_by_engname(enemy->get_engname());
			CGlobalInfo::map_data->increment_kill_count();
		}
		CGlobalInfo::map_data->show_description(current_city);
	}
}

int battle_attack(vector<string>& tokens) {
	CMonster* enemy = CGlobalInfo::parser->get_current_enemy();
	CFighter* player = (CFighter*)CGlobalInfo::user->get_user();

	if (!enemy || !player) {
		cout << "�԰���H���s�b�I" << endl;
		return 0;
	}

	cout << "\n=== ���q���� ===" << endl;
	Sleep(500);

	// ���a����
	int player_base_damage = 10 + (rand() % 6);
	if (player->getWeapon()) {
		player_base_damage += player->getWeapon()->getattackbonus();
		cout << "�Z�� " << player->getWeapon()->getName() << " �o�ʮĪG�I" << endl;
		Sleep(800);
	}

	int player_final_damage = enemy->gethurt(player_base_damage);
	cout << "�� " << enemy->getname() << " �y�� " << player_base_damage << " �I�ˮ`!" << endl;
	Sleep(1000);
	// �d���l������
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// �Ǫ�����
	Sleep(1000);
	cout << "\n" << enemy->getname() << " �o�ʤ����I" << endl;
	Sleep(800);

	// �p���¦�ˮ`
	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " ��A�y�� " << monster_final_damage << " �I�ˮ`�I" << endl;
	Sleep(1000);

	// ��ܾ԰����A
	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n�A�Q���ѤF�I" << endl;
		Sleep(1000);
		cout << "�����N���~��..." << endl;
		system("pause > nul");  // ���ݫ����J
		handle_game_over();  // �I�s�C�������B�z
		return 0;
	}

	return 0;
}

int battle_skill(vector<string>& tokens) {
	CMonster* enemy = CGlobalInfo::parser->get_current_enemy();
	CFighter* player = static_cast<CFighter*>(CGlobalInfo::user->get_user());

	if (!enemy || !player) {
		cout << "�԰���H���s�b�I" << endl;
		return 0;
	}

	// �ϥΪ�ߪ̧ޯ઺����G�O��ߪ̡A�Υ��]�m�ޯ�
	bool useNoviceSkill = (player->getJobClass() == JobClass::NOVICE ||
		player->getEquippedSkill(1) == nullptr);

	if (useNoviceSkill) {
		// ��ߪ̪���������
		int sp_cost = 15;
		if (player->getSP() < sp_cost) {
			cout << "SP�����I�ݭn " << sp_cost << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== �������� ===" << endl;
		Sleep(500);

		cout << player->getname() << " �����믫..." << endl;
		Sleep(1200);

		player->delSP(sp_cost);
		cout << "���� " << sp_cost << " SP!" << endl;
		Sleep(800);

		// �p��ˮ`�G��¦���� * 1.5 + �Z���[��
		int damage = (player->getBaseAttack() * 3 / 2);
		if (player->getWeapon()) {
			damage += player->getWeapon()->getattackbonus();
		}

		// ��20%���v�y���z��(2���ˮ`)
		bool is_critical = (rand() % 100) < 20;
		if (is_critical) {
			damage *= 2;
			cout << "�|�ߤ@���I" << endl;
			Sleep(800);
		}

		enemy->gethurt(damage);
		cout << "�� " << enemy->getname() << " �y�� " << damage << " �I�ˮ`�I" << endl;
	}
	else {
		CSkill* skill = player->getEquippedSkill(1);
		// �ˬdSP�O�_����
		if (player->getSP() < skill->getSpCost()) {
			cout << "SP�����I�ݭn " << skill->getSpCost() << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== " << skill->getName() << " ===" << endl;
		Sleep(500);

		// �ھڧޯ��������椣�P�ĪG
		switch (skill->getType()) {
		case SkillType::PHYSICAL: {
			// ���z�ޯ�G��¦�ˮ` + �O�q�[�� + �Z���[��
			int damage = skill->getPower() * (1 + player->getStrength() * 0.1);
			if (player->getWeapon()) {
				damage += player->getWeapon()->getattackbonus();
			}
			enemy->gethurt(damage);
			cout << player->getname() << " �ϥ� " << skill->getName()
				<< "�A�� " << enemy->getname() << " �y�� " << damage << " �I�ˮ`�I" << endl;
			break;
		}

		case SkillType::MAGICAL: {
			// �]�k�ޯ�G��¦�ˮ` + ���O�[��
			int damage = skill->getPower() * (1 + player->getIntelligence() * 0.15);
			enemy->gethurt(damage);
			cout << player->getname() << " �I�� " << skill->getName()
				<< "�A�� " << enemy->getname() << " �y�� " << damage << " �I�]�k�ˮ`�I" << endl;
			break;
		}

		case SkillType::HEAL: {
			// �v¡�ޯ�G��¦�^�_ + ���O�[��
			int heal = skill->getPower() * (1 + player->getIntelligence() * 0.2);
			player->addHP(heal);
			cout << player->getname() << " �ϥ� " << skill->getName()
				<< "�A��_�F " << heal << " �I�ͩR�ȡI" << endl;
			break;
		}

		case SkillType::BUFF: {
			// �W�q�ޯ�G�Ȯɴ��ɧ����O
			int bonus = skill->getPower() * (1 + player->getStrength() * 0.1);
			player->setBaseAttack(player->getBaseAttack() + bonus);
			cout << player->getname() << " �ϥ� " << skill->getName()
				<< "�A�����O���ɤF " << bonus << " �I�I" << endl;
			break;
		}
		}

		// ����SP
		player->delSP(skill->getSpCost());
	}

	Sleep(1000);
	// �d���l������
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// �Ǫ�����
	Sleep(1000);
	cout << "\n" << enemy->getname() << " �o�ʤ����I" << endl;
	Sleep(800);

	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " ��A�y�� " << monster_final_damage << " �I�ˮ`�I" << endl;
	Sleep(1000);

	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n�A�Q���ѤF�I" << endl;
		Sleep(1000);
		cout << "�����N���~��..." << endl;
		system("pause > nul");
		handle_game_over();
		return 0;
	}

	return 0;
}

int battle_ultimate(vector<string>& tokens) {
	CMonster* enemy = CGlobalInfo::parser->get_current_enemy();
	CFighter* player = static_cast<CFighter*>(CGlobalInfo::user->get_user());

	if (!enemy || !player) {
		cout << "�԰���H���s�b�I" << endl;
		return 0;
	}

	// �ϥΪ�ߪ̧ޯ઺����G�O��ߪ̡A�Υ��]�m�ޯ�
	bool useNoviceSkill = (player->getJobClass() == JobClass::NOVICE ||
		player->getEquippedSkill(2) == nullptr);

	if (useNoviceSkill) {
		// ��ߪ̪���O�z�o
		int sp_cost = 30;
		if (player->getSP() < sp_cost) {
			cout << "SP�����I�ݭn " << sp_cost << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== ��O�z�o ===" << endl;
		Sleep(800);

		cout << player->getname() << " �z�o�X��b�O�q�I" << endl;
		Sleep(1500);

		player->delSP(sp_cost);
		cout << "���� " << sp_cost << " SP!" << endl;
		Sleep(800);

		// �^�_HP
		int heal = player->getMAXHP() / 4;  // �^�_25%�̤j�ͩR��
		player->addHP(heal);
		cout << "��_�F " << heal << " �I�ͩR�ȡI" << endl;
		Sleep(800);

		// �y���ˮ`
		int damage = player->getBaseAttack() * 2;  // 2����¦�����O
		if (player->getWeapon()) {
			damage += player->getWeapon()->getattackbonus() * 2;
		}

		enemy->gethurt(damage);
		cout << "�� " << enemy->getname() << " �y�� " << damage << " �I�ˮ`�I" << endl;
	}
	else {
		CSkill* skill = player->getEquippedSkill(2);
		// �ˬdSP�O�_����
		if (player->getSP() < skill->getSpCost()) {
			cout << "SP�����I�ݭn " << skill->getSpCost() << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== " << skill->getName() << " ===" << endl;
		Sleep(800);

		// �ھڧޯ��������椣�P�ĪG�A�¤O����50%
		switch (skill->getType()) {
		case SkillType::PHYSICAL: {
			int damage = skill->getPower() * (1 + player->getStrength() * 0.1) * 1.5;
			if (player->getWeapon()) {
				damage += player->getWeapon()->getattackbonus() * 2;
			}
			enemy->gethurt(damage);
			cout << player->getname() << " �ϥβ׷��� " << skill->getName()
				<< "�A�� " << enemy->getname() << " �y����H�� " << damage << " �I�ˮ`�I" << endl;
			break;
		}

		case SkillType::MAGICAL: {
			int damage = skill->getPower() * (1 + player->getIntelligence() * 0.15) * 1.5;
			enemy->gethurt(damage);
			cout << player->getname() << " �I��׷��]�k " << skill->getName()
				<< "�A�� " << enemy->getname() << " �y����H�� " << damage << " �I�]�k�ˮ`�I" << endl;
			break;
		}

		case SkillType::HEAL: {
			int heal = skill->getPower() * (1 + player->getIntelligence() * 0.2) * 1.5;
			player->addHP(heal);
			cout << player->getname() << " �ϥαj�� " << skill->getName()
				<< "�A��_�F " << heal << " �I�ͩR�ȡI" << endl;
			break;
		}

		case SkillType::BUFF: {
			int bonus = skill->getPower() * (1 + player->getStrength() * 0.1) * 1.5;
			player->setBaseAttack(player->getBaseAttack() + bonus);
			cout << player->getname() << " �ϥαj�� " << skill->getName()
				<< "�A�����O�j�T���ɤF " << bonus << " �I�I" << endl;
			break;
		}
		}

		// ����SP
		player->delSP(skill->getSpCost());
	}

	Sleep(1000);
	// �d���l������
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// �Ǫ�����
	Sleep(1000);
	cout << "\n" << enemy->getname() << " �o�ʤ����I" << endl;
	Sleep(800);

	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " ��A�y�� " << monster_final_damage << " �I�ˮ`�I" << endl;
	Sleep(1000);

	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n�A�Q���ѤF�I" << endl;
		Sleep(1000);
		cout << "�����N���~��..." << endl;
		system("pause > nul");
		handle_game_over();
		return 0;
	}

	return 0;
}

int function_job_change(vector<string>& tokens) {
	// �ˬd��������a
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "���~: �L�k������¾�R�O!" << endl;
		return 0;
	}

	CFighter* player_fighter = (CFighter*)player;

	// ���¾�~�ﶵ
	cout << "\n=== ¾�~�ഫ ===" << endl;
	if (player_fighter->getLevel() < JOB_CHANGE_LEVEL) {
		cout << "�A�����Ť����H��¾�I�ݭn�F�� " << JOB_CHANGE_LEVEL << " �šC" << endl;
		cout << "��e����: " << player_fighter->getLevel() << endl;
		return 0;
	}

	if (player_fighter->getJobClass() != JobClass::NOVICE) {
		cout << "�A�w�g��¾�L�F�I�L�k�A����¾�C" << endl;
		return 0;
	}

	cout << "\n�п�ܭn��¾��¾�~:" << endl;
	cout << "1. �Ԥh - �M����ԧ����P���m" << endl;
	cout << "  - ��o�B�~�ͩR�ȻP���m�O" << endl;
	cout << "  - �S��ޯ�: �r�O�@���B�ԧq" << endl;
	cout << "\n2. �k�v - �ժ��]�k�����P���U" << endl;
	cout << "  - ��o�B�~�]�O�ȻP���O" << endl;
	cout << "  - �S��ޯ�: ���y�N�B�v¡�N" << endl;
	cout << "\n�п�� (1-2, 0���): ";

	int choice;
	cin >> choice;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	switch (choice) {
	case 0:
		cout << "�A�M�w�Ȯɤ���¾�C" << endl;
		break;

	case 1:
		if (player_fighter->changeJob(JobClass::WARRIOR)) {
			cout << "\n��¾���\�I�ϥ� skill ���O�Ӭd�ݩM�޲z�ޯ�C" << endl;
		}
		break;

	case 2:
		if (player_fighter->changeJob(JobClass::MAGE)) {
			cout << "\n��¾���\�I�ϥ� skill ���O�Ӭd�ݩM�޲z�ޯ�C" << endl;
		}
		break;

	default:
		cout << "�L�Ī���ܡI" << endl;
	}

	return 0;
}
int function_skill_management(vector<string>& tokens) {
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "���~: �L�k����ޯ�޲z�R�O!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;

	// �p�G�٨S��¾
	if (fighter->getJobClass() == JobClass::NOVICE) {
		cout << "�A�٨S����¾�A�L�k�ϥΧޯ�t�ΡI" << endl;
		cout << "�F��15�ū�i�H�ϥ� job ���O�i����¾�C" << endl;
		return 0;
	}

	while (true) {
		cout << "\n=== �ޯ�޲z ===" << endl;
		fighter->showSkills();  // ��ܧޯ�C��M�ֱ���]�m

		cout << "\n�п�ܾާ@:" << endl;
		cout << "1. �ɯŧޯ�" << endl;
		cout << "2. �]�m�ֱ���" << endl;
		cout << "0. ���}�ޯ�޲z" << endl;
		cout << "���: ";

		int choice;
		if (!(cin >> choice)) {
			cin.clear();  // �M�����~���A
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "�L�Ī���J�I�Э��աC" << endl;
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) break;

		switch (choice) {
		case 1: {
			if (fighter->getSkillPoints() <= 0) {
				cout << "�A�S���i�Ϊ��ޯ��I�ơI" << endl;
				continue;
			}

			cout << "��ܭn�ɯŪ��ޯ� (1-" << fighter->getSkills().size() << "): ";
			int skillChoice;
			if (!(cin >> skillChoice)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "�L�Ī���J�I" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (skillChoice > 0 && skillChoice <= fighter->getSkills().size()) {
				fighter->upgradeSkill(skillChoice - 1);
			}
			else {
				cout << "�L�Ī���ܡI" << endl;
			}
			break;
		}

		case 2: {
			cout << "��ܭn�]�m���ֱ����m (1:X, 2:C): ";
			int slot;
			if (!(cin >> slot)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "�L�Ī���J�I" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			// �վ�slot���ȥH�ǰt��ڪ��}�C���ޡ]1->1, 2->2�^
			if (slot < 1 || slot > 2) {
				cout << "�L�Ī��ֱ����m�I�����O1��2�C" << endl;
				continue;
			}

			cout << "��ܭn�j�w���ޯ� (1-" << fighter->getSkills().size() << "): ";
			int skillChoice;
			if (!(cin >> skillChoice)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "�L�Ī���J�I" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (skillChoice > 0 && skillChoice <= fighter->getSkills().size()) {
				fighter->setSkillHotkey(slot, skillChoice - 1);  // �����ϥ�slot�A���ݭn�B�~�ഫ
				cout << "�ֱ���]�m���\�I" << endl;
			}
			else {
				cout << "�L�Ī��ޯ��ܡI" << endl;
			}
			break;
		}

		default:
			cout << "�L�Ī���ܡI�п�J0-2�������Ʀr�C" << endl;
			continue;
		}
	}

	return 0;
}
int function_logout(vector<string>& tokens) {
	// �ˬd���a�O�_�w�n�J
	if (!CGlobalInfo::user || !CGlobalInfo::user->get_user()) {
		cout << "�A�|���n�J!" << endl;
		return 0;
	}

	cout << "\n=== �n�X�T�{ ===" << endl;
	cout << "�T�w�n�n�X��? �t�αN�O�s�Ҧ������� (Y/N): ";

	char confirm;
	cin >> confirm;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (toupper(confirm) != 'Y') {
		cout << "�����n�X�C" << endl;
		return 0;
	}

	// �O�s������
	string username = AccountManager::getInstance().getCurrentUsername();
	CFighter* fighter = (CFighter*)CGlobalInfo::user->get_user();

	if (!fighter) {
		cout << "���~: �L�k�������ƾ�!" << endl;
		return 0;
	}

	cout << "\n���b�O�s������..." << endl;
	Sleep(500);

	// �T�O�s�ɥؿ��s�b
	if (!CreateDirectoryA("saves", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			cout << "���~: �L�k�Ыئs�ɥؿ�!" << endl;
			return 0;
		}
	}

	// �Ыسƥ��ؿ�
	if (!CreateDirectoryA("saves\\backups", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			cout << "ĵ�i: �L�k�Ыسƥ��ؿ�!" << endl;
			// �~�����A�]���o���O�P�R���~
		}
	}

	// �������S�w���s�ɸ��|
	auto& saveManager = SaveDirectoryManager::getInstance();
	string savePath = saveManager.getCharacterSavePath(username, fighter->getname());

	// �p�G�s�ɤw�s�b�A�Ыسƥ�
	if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
		string backupPath = saveManager.getCharacterBackupPath(username, fighter->getname());

		if (CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE)) {
			cout << "�w�Ыب����Ƴƥ�" << endl;
		}
		else {
			cout << "ĵ�i: �L�k�Ыسƥ����" << endl;
			// �~�����A�]���o���O�P�R���~
		}
	}

	// �O�s��e������
	bool saveSuccess = SaveSystem::saveCharacterData(fighter, savePath);
	if (!saveSuccess) {
		cout << "ĵ�i: �����ƫO�s����!" << endl;
		cout << "�O�_���n�~��n�X? (Y/N): ";
		cin >> confirm;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (toupper(confirm) != 'Y') {
			cout << "�����n�X�C" << endl;
			return 0;
		}
	}
	else {
		cout << "�����ƫO�s���\!" << endl;
	}

	// �n�X���a
	AccountManager::getInstance().logout();
	cout << "�n�X���\�I" << endl;
	Sleep(1000);

	// �M�z��e�C�����A
	CGlobalInfo::user->set_user(nullptr);
	system("cls");  // �M�ŵe��

	// ��^�S���1��ܻݭn��^�n�J�e��
	return 1;
}