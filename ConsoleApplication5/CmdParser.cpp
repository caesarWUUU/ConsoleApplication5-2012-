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
#define NOMINMAX  // 防止 windows.h 定義 min 和 max 
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
		cout << "移動指令格式錯誤" << endl;
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
		cout << "無效的移動方向" << endl;
		return 0;
	}

	int next_city = CGlobalInfo::user->goto_next_city(next_dir);
	if (next_city > 0) {
		system("cls");
		CGlobalInfo::map_data->show_description(next_city);

		cout << "\n可移動方向：" << endl;
		CPlace* current_place = CGlobalInfo::map_data->get_place_by_id(next_city);
		if (current_place) {
			vector<int> available_directions = current_place->get_available_directions();
			for (int dir : available_directions) {
				switch (dir) {
				case eNorth: cout << "北方(W) "; break;
				case eEast: cout << "東方(D) "; break;
				case eSouth: cout << "南方(S) "; break;
				case eWest: cout << "西方(A) "; break;
				}
			}
			cout << endl;
		}
	}
	else {
		cout << "無法往該方向移動！請確認路徑是否存在。" << endl;
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
		cout << "指令格式：kill [怪物名稱]" << endl;
		cout << "例如：kill cat3 或 kill rabbit1" << endl;
		return 0;
	}

	string monster_engname = tokens[1];
	int city = CGlobalInfo::user->get_current_city();
	CPlace* cityptr = CGlobalInfo::map_data->get_place_by_id(city);
	assert(cityptr);

	// 先檢查當前位置有哪些怪物
	cout << "當前位置的怪物：" << endl;
	cityptr->show_mosters();

	CMonster* monster = cityptr->get_monster_by_engname(monster_engname);
	if (!monster) {
		cout << "找不到指定的怪物。請使用 ls 指令查看當前位置的怪物列表。" << endl;
		cout << "注意：請使用怪物的英文名稱加上編號（例如：cat3）來進行戰鬥。" << endl;
		return 0;
	}

	// 進入戰鬥模式
	CGlobalInfo::parser->enter_battle_mode(monster);
	return 0;
}
int function_move(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "移動指令格式：move [地點名稱]" << endl;
		return 0;
	}

	string destination = tokens[1];

	// 直接通過地點名稱找到對應ID
	int dest_id = CGlobalInfo::map_data->get_place_id_by_name(destination);

	if (dest_id < 0) {
		cout << "找不到該地點！" << endl;
		return 0;
	}

	// 直接更新玩家位置並顯示
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
		cout << f->getname() << " 打開背包" << endl;
		
		cout << "\n=== 背包內容 ===" << endl ;
		
		int num = f->showAllBagItems();
		cout << endl;
		cout << "目前金幣: " << f->getMoney() << endl << endl;  // 新增這行，顯示金幣數量
		if (num == 0) {
			cout << "背包空空如也" << endl;
			cout << f->getname() << " 關上背包" << endl;
			return 0;
		}

		int selection = 0;
		cout << "要使用什麼物品 (0表示不要)" << endl;
		cin >> selection;
		while (selection > 0) {
			if (!f->useBagItems(selection)) {
				cout << "此選項不存在" << endl;
			}
			selection = 0;
			cout << "要使用什麼物品 (0表示不要)" << endl;
			cin >> selection;
		}
		cout << f->getname() << " 關上背包" << endl;
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

	// 如果玩家有未分配點數,進入加點循環
	while (fighter->getAvailablePoints() > 0) {
		cout << "\n=== 分配屬性點 ===" << endl;
		cout << "未分配點數: " << fighter->getAvailablePoints() << endl;
		cout << "1) 力量 (+2攻擊力)" << endl;
		cout << "2) 智力 (+5魔力)" << endl;
		cout << "3) 堅毅 (+10血量)" << endl;
		cout << "4) 幸運 (+1%經驗值獲取)" << endl;
		cout << "0) 返回" << endl;

		cout << "\n請選擇要加點的屬性 (0-4): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) break;  // 玩家選擇退出加點

		if (choice < 1 || choice > 4) {
			cout << "無效的選擇!" << endl;
			continue;
		}

		cout << "要加多少點? (當前可用點數: " << fighter->getAvailablePoints() << "): ";
		int points;
		cin >> points;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (points <= 0) {
			cout << "無效的點數" << endl;
			continue;
		}

		if (points > fighter->getAvailablePoints()) {
			cout << "可用點數不足" << endl;
			continue;
		}

		// 根據選擇分配點數
		bool success = true;
		switch (choice) {
		case 1: // 力量
			fighter->addStrength(points);
			cout << "力量增加 " << points << " 點" << endl;
			break;
		case 2: // 智力
			fighter->addIntelligence(points);
			cout << "智力增加 " << points << " 點" << endl;
			break;
		case 3: // 堅毅
			fighter->addTenacity(points);
			cout << "堅毅增加 " << points << " 點" << endl;
			break;
		case 4: // 幸運
			// 檢查是否超過100%上限
			if (fighter->getLucky() + points > 100) {
				cout << "幸運值不能超過100" << endl;
				success = false;
				continue;
			}
			fighter->addLucky(points);
			cout << "幸運增加 " << points << " 點" << endl;
			break;
		}

		if (success) {
			// 扣除已使用的點數
			fighter->addAvailablePoints(-points);

			// 更新狀態
			cout << "\n=== 更新後的狀態 ===" << endl;
			fighter->showStatus();
		}
	}

	if (fighter->getAvailablePoints() == 0) {
		cout << "\n已無可用屬性點" << endl;
	}

	return 0;
}

int function_shop_weapon(vector<string>& tokens) {
	// 檢查是否在市集
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "必須在市集才能使用武器商店！" << endl;
		return 0;
	}

	// 檢查並獲取玩家對象
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "無法訪問商店！" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== 歡迎光臨武器商店 ===" << endl;
	cout << "你的金幣: " << fighter->getMoney() << endl;
	cout << "編號  武器名稱    攻擊加成    價格" << endl;
	cout << "--------------------------------" << endl;

	// 讀取武器資料
	ifstream fin("weapon.txt");
	if (!fin) {
		cout << "無法開啟武器資料檔" << endl;
		return 0;
	}

	vector<pair<CWeapon*, int>> weapons;
	int id;
	string name;
	int attack;

	// 讀取並顯示武器列表
	while (fin >> id >> name >> attack) {
		CWeapon* weapon = new CWeapon(name, 0, 0, id, attack);
		int price = attack * 100;  // 價格 = 攻擊力 * 100
		weapons.push_back(make_pair(weapon, price));
		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << attack << "  "
			<< setw(8) << price << " 金幣" << endl;
	}
	fin.close();

	// 購買循環
	while (true) {
		cout << "\n目前金幣: " << fighter->getMoney() << endl;
		cout << "請選擇要購買的武器編號 (輸入0退出商店): ";
		int choice;
		cin >> choice;
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// 尋找選擇的武器
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
			cout << "\n=== 購買確認 ===" << endl;
			cout << "武器名稱: " << selected_weapon->getName() << endl;
			cout << "攻擊加成: +" << selected_weapon->getattackbonus() << endl;
			cout << "價    格: " << price << " 金幣" << endl;
			cout << "你的金幣: " << fighter->getMoney() << endl;

			if (fighter->getMoney() < price) {
				cout << "金幣不足！還需要 " << (price - fighter->getMoney()) << " 金幣" << endl;
				continue;
			}

			cout << "確定購買嗎？(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // 扣除金幣
					fighter->captureItem(new CWeapon(*selected_weapon));  // 添加到背包
					cout << "交易成功！獲得 " << selected_weapon->getName() << endl;
					cout << "剩餘金幣: " << fighter->getMoney() << endl;
				}
			}
			else {
				cout << "取消購買。" << endl;
			}
		}
		else {
			cout << "無效的武器編號！請重新選擇。" << endl;
		}
	}

	// 清理資源
	for (auto& weapon_pair : weapons) {
		delete weapon_pair.first;
	}
	weapons.clear();

	cout << "\n歡迎下次光臨！" << endl;
	return 0;
}

int function_shop_armor(vector<string>& tokens) {
	// 檢查是否在市集
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "必須在市集才能使用防具商店！" << endl;
		return 0;
	}

	// 檢查並獲取玩家對象
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "無法訪問商店！" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== 歡迎光臨防具商店 ===" << endl;
	cout << "你的金幣: " << fighter->getMoney() << endl;
	cout << "編號  防具名稱    防禦加成    部位        價格" << endl;
	cout << "------------------------------------------------" << endl;

	// 讀取防具資料
	ifstream fin("armor.txt");
	if (!fin) {
		cout << "無法開啟防具資料檔" << endl;
		return 0;
	}

	vector<pair<CArmor*, int>> armors;
	int id;
	string name;
	int defense;
	int type;

	// 部位名稱對應
	const string position_names[] = { "頭部", "身體", "手部", "腳部" };

	// 讀取並顯示防具列表
	while (fin >> id >> name >> defense >> type) {
		CArmor* armor = new CArmor(name, 0, 0, id, defense, static_cast<ArmorType>(type));
		int price = defense * 200;  // 價格 = 防禦力 * 200
		armors.push_back(make_pair(armor, price));

		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << defense << "  "
			<< setw(8) << position_names[static_cast<int>(armor->getArmorType())] << "  "  // 這裡進行類型轉換
			<< setw(8) << price << " 金幣" << endl;
	}
	fin.close();

	// 顯示當前裝備
	cout << "\n=== 當前裝備狀態 ===" << endl;
	fighter->showEquippedArmor();

	// 購買循環
	while (true) {
		cout << "\n目前金幣: " << fighter->getMoney() << endl;
		cout << "請選擇要購買的防具編號 (輸入0退出商店): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// 尋找選擇的防具
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
			cout << "\n=== 購買確認 ===" << endl;
			cout << "防具名稱: " << selected_armor->getName() << endl;
			cout << "防禦加成: +" << selected_armor->getdefensebonus() << endl;
			cout << "裝備部位: " << position_names[static_cast<int>(selected_armor->getArmorType())] << endl;  // 這裡進行類型轉換
			cout << "價    格: " << price << " 金幣" << endl;
			cout << "你的金幣: " << fighter->getMoney() << endl;

			// 檢查是否已裝備同類型防具
			CArmor* equipped = fighter->getArmor(selected_armor->getArmorType());
			if (equipped) {
				cout << "警告：該部位已裝備 " << equipped->getName()
					<< " (防禦力 +" << equipped->getdefensebonus() << ")" << endl;
			}

			if (fighter->getMoney() < price) {
				cout << "金幣不足！還需要 " << (price - fighter->getMoney()) << " 金幣" << endl;
				continue;
			}

			cout << "確定購買嗎？(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // 扣除金幣
					fighter->captureItem(new CArmor(*selected_armor));  // 添加到背包
					cout << "交易成功！獲得 " << selected_armor->getName() << endl;
					cout << "剩餘金幣: " << fighter->getMoney() << endl;
					cout << "提示：可以使用背包(checkbag)來裝備新購買的防具" << endl;
				}
			}
			else {
				cout << "取消購買。" << endl;
			}
		}
		else {
			cout << "無效的防具編號！請重新選擇。" << endl;
		}
	}

	// 清理資源
	for (auto& armor_pair : armors) {
		delete armor_pair.first;
	}
	armors.clear();

	cout << "\n歡迎下次光臨！" << endl;
	return 0;
}
int function_show_pets(vector<string>& tokens) {
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "錯誤: 無法執行寵物指令!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;

	// 顯示所有寵物列表
	fighter->showPets();
	return 0;
}
int function_pet_status(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "指令格式：petstatus [寵物編號]" << endl;
		return 0;
	}

	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "錯誤: 無法執行寵物指令!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;
	int pet_index = std::stoi(tokens[1]) - 1;  // 將輸入的編號轉換為索引

	const vector<CPet*>& pets = fighter->getPets();
	if (pet_index < 0 || pet_index >= pets.size()) {
		cout << "無效的寵物編號！" << endl;
		return 0;
	}

	// 顯示指定寵物的詳細狀態
	pets[pet_index]->showStatus();
	return 0;
}
int function_switch_pet(vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "指令格式：switchpet [寵物編號]" << endl;
		return 0;
	}

	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "錯誤: 無法執行寵物指令!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;
	int pet_index = std::stoi(tokens[1]) - 1;

	if (fighter->switchPet(pet_index)) {
		cout << "成功切換出戰寵物！" << endl;
		fighter->getActivePet()->showStatus();
	}
	else {
		cout << "切換寵物失敗！請確認寵物編號正確且寵物狀態正常。" << endl;
	}
	return 0;
}

int function_shop_food(vector<string>& tokens) {
	// 檢查是否在市集
	if (CGlobalInfo::user->get_current_city() != 2) {
		cout << "必須在市集才能使用食堂！" << endl;
		return 0;
	}

	// 檢查並獲取玩家對象
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "無法訪問食堂！" << endl;
		return 0;
	}
	CFighter* fighter = (CFighter*)player;

	cout << "\n=== 歡迎光臨食堂 ===" << endl;
	cout << "你的金幣: " << fighter->getMoney() << endl;
	cout << "你的生命值: " << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
	cout << "編號  食物名稱    恢復HP    價格" << endl;
	cout << "--------------------------------" << endl;

	// 讀取食物資料
	ifstream fin("food.txt");
	if (!fin) {
		cout << "無法開啟食物資料檔" << endl;
		return 0;
	}

	vector<pair<CFood*, int>> foods;
	int id;
	string name;
	int hp_bonus;

	// 讀取並顯示食物列表
	while (fin >> id >> name >> hp_bonus) {
		CFood* food = new CFood(name, 0, 0, id, hp_bonus);
		int price = hp_bonus * 50;  // 價格 = 恢復量 * 50
		foods.push_back(make_pair(food, price));
		cout << setw(4) << id << "  "
			<< setw(10) << name << "  "
			<< setw(8) << hp_bonus << "  "
			<< setw(8) << price << " 金幣" << endl;
	}
	fin.close();

	// 購買循環
	while (true) {
		cout << "\n目前狀態：" << endl;
		cout << "金幣: " << fighter->getMoney() << endl;
		cout << "生命值: " << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
		cout << "\n請選擇要購買的食物編號 (輸入0退出食堂): ";
		int choice;
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) {
			break;
		}

		// 尋找選擇的食物
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
			cout << "\n=== 購買確認 ===" << endl;
			cout << "食物名稱: " << selected_food->getName() << endl;
			cout << "恢復效果: +" << selected_food->gethpbonus() << " HP" << endl;
			cout << "價    格: " << price << " 金幣" << endl;
			cout << "你的金幣: " << fighter->getMoney() << endl;

			// 檢查血量是否已滿
			if (fighter->getHP() >= fighter->getMAXHP()) {
				cout << "提醒：你的生命值已經是滿的了" << endl;
			}

			if (fighter->getMoney() < price) {
				cout << "金幣不足！還需要 " << (price - fighter->getMoney()) << " 金幣" << endl;
				continue;
			}

			cout << "確定購買嗎？(Y/N): ";
			char confirm;
			cin >> confirm;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (toupper(confirm) == 'Y') {
				if (fighter->spendMoney(price)) {  // 扣除金幣
					fighter->captureItem(new CFood(*selected_food));  // 添加到背包
					cout << "交易成功！獲得 " << selected_food->getName() << endl;
					cout << "剩餘金幣: " << fighter->getMoney() << endl;

					// 詢問是否立即使用
					cout << "是否立即使用？(Y/N): ";
					cin >> confirm;
					cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

					if (toupper(confirm) == 'Y') {
						vector<string> bag_cmd = { "checkbag" };
						function_check_bag(bag_cmd);  // 直接打開背包
					}
					else {
						cout << "你可以稍後使用背包(checkbag)來使用食物" << endl;
					}
				}
			}
			else {
				cout << "取消購買。" << endl;
			}
		}
		else {
			cout << "無效的食物編號！請重新選擇。" << endl;
		}
	}

	// 清理資源
	for (auto& food_pair : foods) {
		delete food_pair.first;
	}
	foods.clear();

	cout << "\n歡迎下次光臨！" << endl;
	return 0;
}


CCmdParser::CCmdParser() {
	// 原有的指令映射
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

	// 新增轉職和技能相關指令
	mappingfunc[string("job")] = (void(*)(vector<string>&))function_job_change;
	mappingfunc[string("skill")] = (void(*)(vector<string>&))function_skill_management;

	mappingfunc[string("showpets")] = (void(*)(vector<string>&))function_show_pets;
	mappingfunc[string("petstatus")] = (void(*)(vector<string>&))function_pet_status;
	mappingfunc[string("switchpet")] = (void(*)(vector<string>&))function_switch_pet;

	// 戰鬥指令
	battle_commands[string("z")] = (void(*)(vector<string>&))battle_attack;
	battle_commands[string("x")] = (void(*)(vector<string>&))battle_skill;
	battle_commands[string("c")] = (void(*)(vector<string>&))battle_ultimate;
	// 添加登出命令
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
	cout << "\n=== 指令說明 ===" << endl;
	cout << "移動指令:" << endl;
	cout << "  W/north - 往北移動" << endl;
	cout << "  S/south - 往南移動" << endl;
	cout << "  A/west - 往西移動" << endl;
	cout << "  D/east - 往東移動" << endl;
	cout << "  move [地點名稱] - 直接移動到指定地點" << endl;

	cout << "\n戰鬥相關:" << endl;
	cout << "  ls - 列出當前位置的怪物" << endl;
	cout << "  kill [怪物名稱] - 攻擊指定怪物" << endl;

	cout << "\n角色相關:" << endl;
	cout << "  status - 查看狀態" << endl;
	cout << "  checkbag - 查看背包" << endl;
	cout << "  job - 轉職系統（需要等級15）" << endl;
	cout << "  skill - 技能管理（需要先轉職）" << endl;

	cout << "\n商店相關（僅在商店可用）:" << endl;
	cout << "  shopA - 開啟武器商店" << endl;
	cout << "  shopB - 開啟防具商店" << endl;
	cout << "  shopC - 開啟食物商店" << endl;

	cout << "\n其他指令:" << endl;
	cout << "  help - 顯示此說明" << endl;
	cout << "  exit - 離開遊戲" << endl;

	cout << "\n寵物相關:" << endl;
	cout << "  showpets - 顯示所有寵物列表" << endl;
	cout << "  petstatus [編號] - 查看指定寵物的詳細狀態" << endl;
	cout << "  switchpet [編號] - 切換出戰寵物" << endl;

	cout << "\n帳號相關:" << endl;
	cout << "  logout/quit - 保存資料並登出回到登入畫面" << endl;

	cout << "\n當前位置: " << CGlobalInfo::map_data->get_place_name(CGlobalInfo::user->get_current_city()) << endl;
}
int CCmdParser::process_wasd(const string& direction) {
	vector<string> tokens;
	tokens.resize(1);  // 確保有一個元素

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
		return 0;  // 無效的輸入
	}

	return function_next_direction(tokens);
}


void CCmdParser::enter_battle_mode(CMonster* enemy) {
	current_mode = MODE_BATTLE;
	current_enemy = enemy;
	system("cls");  // 清屏
	cout << "========== 戰鬥開始 ==========" << endl;
	cout << "對手：" << enemy->get_basic_data() << endl;
	show_battle_help();
}
void CCmdParser::exit_battle_mode() {
	current_mode = MODE_NORMAL;
	current_enemy = nullptr;

	cout << "========== 戰鬥結束 ==========" << endl;
	Sleep(500);
	cout << "\n按任意鍵以返回地圖..." << endl;

	// 等待使用者按下任意鍵
	system("pause > nul");

	// 清除畫面並顯示地圖
	
}
void CCmdParser::show_battle_help() {
	CFighter* player = (CFighter*)CGlobalInfo::user->get_user();

	cout << "\n=== 戰鬥指令說明 ===" << endl;
	cout << "Z - 普通攻擊" << endl;

	if (player->getJobClass() == JobClass::NOVICE) {
		// 初心者顯示基本技能名稱
		cout << "X - 技能攻擊 (SP: 15) - 集中攻擊，有機會造成爆擊" << endl;
		cout << "C - 絕招攻擊 (SP: 30) - 潛力爆發，恢復生命並造成傷害" << endl;
	}
	else {
		// 已轉職的角色
		CSkill* skillX = player->getEquippedSkill(1);
		if (skillX) {
			// 如果有設置X鍵技能，顯示該技能
			cout << "X - " << skillX->getName() << " (SP: " << skillX->getSpCost() << ")" << endl;
		}
		else {
			// 沒有設置則顯示預設名稱
			cout << "X - 技能攻擊" << endl;
		}

		CSkill* skillC = player->getEquippedSkill(2);
		if (skillC) {
			// 如果有設置C鍵技能，顯示該技能
			cout << "C - " << skillC->getName() << " (SP: " << skillC->getSpCost() << ")" << endl;
		}
		else {
			// 沒有設置則顯示預設名稱
			cout << "C - 絕招攻擊" << endl;
		}
	}

	cout << "help - 顯示此說明" << endl;
}

int CCmdParser::process_battle_command(const string& cmd) {
	// 將命令轉換為小寫進行處理
	string lower_cmd = cmd;
	transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::tolower);

	if (lower_cmd == "help") {
		show_battle_help();
		return 0;
	}

	map<string, void(*)>::iterator it = battle_commands.find(lower_cmd);
	if (it == battle_commands.end()) {
		cout << "無效的戰鬥指令。輸入 help 查看可用指令。" << endl;
		return 0;
	}

	vector<string> tokens;
	tokens.push_back(lower_cmd);
	return (reinterpret_cast<int(*)(vector<string>&)>(it->second)(tokens));
}


// 修改 query 函數來支援戰鬥模式
int CCmdParser::query() {
	string sentence;
	getline(cin, sentence);
	vector<string> tokens;
	splitstring(sentence, tokens, string(" "));
	if (tokens.size() == 0) {
		return 0;
	}

	// 根據當前模式處理指令
	if (current_mode == MODE_BATTLE) {
		return process_battle_command(tokens[0]);
	}

	// 一般模式的指令處理
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
		cout << "無效的指令。輸入 help 查看可用指令。" << endl;
		return 0;
	}
	return (reinterpret_cast<int(*)(const vector<string>&)>(it->second)(tokens));
}
void handle_game_over() {
	system("cls");  // 清除畫面

	cout << "\n\n";
	cout << "==================== 【Game Over】 ====================\n\n";
	Sleep(1000);

	cout << "勇者以無畏的心迎接命運的挑戰，與強大的敵人展開殊死搏鬥。\n";
	Sleep(1000);
	cout << "然而，命運無情，他的生命之火終究熄滅在戰場上。\n";
	Sleep(1000);
	cout << "\n雖然他的旅程就此結束，但他的勇氣與精神將永遠銘刻在這片大地之上。\n";
	Sleep(1000);
	cout << "也許某一天，新的冒險者將拾起他的遺志，繼續書寫未完的篇章。\n\n";
	Sleep(1000);

	cout << "按任意鍵結束遊戲..." << endl;
	system("pause > nul");  // 等待按鍵輸入
	exit(0);  // 結束程式
}
bool CCmdParser::executePetAttack(CMonster* enemy, CFighter* player) {
	// 基礎檢查
	if (!enemy || enemy->isdead() || !player) {
		return false;
	}

	// 將所有寵物相關操作包在 try block 中
	try {
		// 取得寵物指標
		CPet* active_pet = nullptr;
		try {
			active_pet = player->getActivePet();
			if (!active_pet) {
				return false;
			}

			// 使用可驗證的記憶體位置來檢查指標的有效性
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

		// 進行寵物追擊
		cout << "\n=== 寵物追擊 ===" << endl;
		Sleep(500);

		// 使用固定的基礎傷害
		int pet_damage = 3;

		cout << "寵物發動追擊！" << endl;
		Sleep(800);

		enemy->gethurt(pet_damage);
		cout << "造成 " << pet_damage << " 點傷害！" << endl;
		Sleep(1000);

		return true;
	}
	catch (...) {
		return false;
	}
}

void handle_battle_end(CFighter* player, CMonster* enemy) {
	if (enemy->isdead()) {
		cout << "\n" << enemy->getname() << " 被打倒了！" << endl;
		Sleep(1000);
		cout << "獲得戰鬥勝利！" << endl;
		Sleep(1000);
		// 嘗試捕獲
		cout << "\n嘗試捕捉怪物..." << endl;
		Sleep(800);

		// 計算捕獲機率（目前測試階段設為100%）
		bool can_capture = true;  // 之後可以改為根據等級差、HP比例等計算

		if (can_capture && player->capturePet(enemy)) {
			cout << "成功捕獲了 " << enemy->getname() << "！" << endl;
			Sleep(500);
			player->showPets();
		}
		// 計算經驗值獎勵
		int exp_reward = enemy->getLevel() * 5;  // 每級給予 5 點經驗值
		if (exp_reward <= 0) exp_reward = 1;
		if (exp_reward > 300) exp_reward = 300;  // 設定上限避免過量

		// 給予經驗值
		player->addExp(exp_reward);
		Sleep(1000);

		// 檢查是否有出戰寵物，如果有也給予經驗值
		if (CPet* active_pet = player->getActivePet()) {
			cout << "\n出戰寵物 " << active_pet->getNickname() << " 獲得經驗值！" << endl;
			Sleep(500);
			active_pet->addExp(exp_reward);
			Sleep(1000);
		}

		// 計算金幣獎勵
		int base_gold = 20;                          // 基礎金幣
		int level_bonus = enemy->getLevel() * 5;     // 等級加成
		int random_bonus = rand() % (enemy->getLevel() * 2); // 隨機波動
		int total_gold = base_gold + level_bonus + random_bonus;

		cout << "\n尋找戰利品..." << endl;
		Sleep(1000);
		cout << "獲得 " << total_gold << " 金幣！" << endl;
		player->addMoney(total_gold);
		Sleep(1500);

		// 結束戰鬥
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
		cout << "戰鬥對象不存在！" << endl;
		return 0;
	}

	cout << "\n=== 普通攻擊 ===" << endl;
	Sleep(500);

	// 玩家攻擊
	int player_base_damage = 10 + (rand() % 6);
	if (player->getWeapon()) {
		player_base_damage += player->getWeapon()->getattackbonus();
		cout << "武器 " << player->getWeapon()->getName() << " 發動效果！" << endl;
		Sleep(800);
	}

	int player_final_damage = enemy->gethurt(player_base_damage);
	cout << "對 " << enemy->getname() << " 造成 " << player_base_damage << " 點傷害!" << endl;
	Sleep(1000);
	// 寵物追擊部分
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// 怪物反擊
	Sleep(1000);
	cout << "\n" << enemy->getname() << " 發動反擊！" << endl;
	Sleep(800);

	// 計算基礎傷害
	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " 對你造成 " << monster_final_damage << " 點傷害！" << endl;
	Sleep(1000);

	// 顯示戰鬥狀態
	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n你被打敗了！" << endl;
		Sleep(1000);
		cout << "按任意鍵繼續..." << endl;
		system("pause > nul");  // 等待按鍵輸入
		handle_game_over();  // 呼叫遊戲結束處理
		return 0;
	}

	return 0;
}

int battle_skill(vector<string>& tokens) {
	CMonster* enemy = CGlobalInfo::parser->get_current_enemy();
	CFighter* player = static_cast<CFighter*>(CGlobalInfo::user->get_user());

	if (!enemy || !player) {
		cout << "戰鬥對象不存在！" << endl;
		return 0;
	}

	// 使用初心者技能的條件：是初心者，或未設置技能
	bool useNoviceSkill = (player->getJobClass() == JobClass::NOVICE ||
		player->getEquippedSkill(1) == nullptr);

	if (useNoviceSkill) {
		// 初心者的集中攻擊
		int sp_cost = 15;
		if (player->getSP() < sp_cost) {
			cout << "SP不足！需要 " << sp_cost << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== 集中攻擊 ===" << endl;
		Sleep(500);

		cout << player->getname() << " 集中精神..." << endl;
		Sleep(1200);

		player->delSP(sp_cost);
		cout << "消耗 " << sp_cost << " SP!" << endl;
		Sleep(800);

		// 計算傷害：基礎攻擊 * 1.5 + 武器加成
		int damage = (player->getBaseAttack() * 3 / 2);
		if (player->getWeapon()) {
			damage += player->getWeapon()->getattackbonus();
		}

		// 有20%機率造成爆擊(2倍傷害)
		bool is_critical = (rand() % 100) < 20;
		if (is_critical) {
			damage *= 2;
			cout << "會心一擊！" << endl;
			Sleep(800);
		}

		enemy->gethurt(damage);
		cout << "對 " << enemy->getname() << " 造成 " << damage << " 點傷害！" << endl;
	}
	else {
		CSkill* skill = player->getEquippedSkill(1);
		// 檢查SP是否足夠
		if (player->getSP() < skill->getSpCost()) {
			cout << "SP不足！需要 " << skill->getSpCost() << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== " << skill->getName() << " ===" << endl;
		Sleep(500);

		// 根據技能類型執行不同效果
		switch (skill->getType()) {
		case SkillType::PHYSICAL: {
			// 物理技能：基礎傷害 + 力量加成 + 武器加成
			int damage = skill->getPower() * (1 + player->getStrength() * 0.1);
			if (player->getWeapon()) {
				damage += player->getWeapon()->getattackbonus();
			}
			enemy->gethurt(damage);
			cout << player->getname() << " 使用 " << skill->getName()
				<< "，對 " << enemy->getname() << " 造成 " << damage << " 點傷害！" << endl;
			break;
		}

		case SkillType::MAGICAL: {
			// 魔法技能：基礎傷害 + 智力加成
			int damage = skill->getPower() * (1 + player->getIntelligence() * 0.15);
			enemy->gethurt(damage);
			cout << player->getname() << " 施放 " << skill->getName()
				<< "，對 " << enemy->getname() << " 造成 " << damage << " 點魔法傷害！" << endl;
			break;
		}

		case SkillType::HEAL: {
			// 治癒技能：基礎回復 + 智力加成
			int heal = skill->getPower() * (1 + player->getIntelligence() * 0.2);
			player->addHP(heal);
			cout << player->getname() << " 使用 " << skill->getName()
				<< "，恢復了 " << heal << " 點生命值！" << endl;
			break;
		}

		case SkillType::BUFF: {
			// 增益技能：暫時提升攻擊力
			int bonus = skill->getPower() * (1 + player->getStrength() * 0.1);
			player->setBaseAttack(player->getBaseAttack() + bonus);
			cout << player->getname() << " 使用 " << skill->getName()
				<< "，攻擊力提升了 " << bonus << " 點！" << endl;
			break;
		}
		}

		// 消耗SP
		player->delSP(skill->getSpCost());
	}

	Sleep(1000);
	// 寵物追擊部分
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// 怪物反擊
	Sleep(1000);
	cout << "\n" << enemy->getname() << " 發動反擊！" << endl;
	Sleep(800);

	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " 對你造成 " << monster_final_damage << " 點傷害！" << endl;
	Sleep(1000);

	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n你被打敗了！" << endl;
		Sleep(1000);
		cout << "按任意鍵繼續..." << endl;
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
		cout << "戰鬥對象不存在！" << endl;
		return 0;
	}

	// 使用初心者技能的條件：是初心者，或未設置技能
	bool useNoviceSkill = (player->getJobClass() == JobClass::NOVICE ||
		player->getEquippedSkill(2) == nullptr);

	if (useNoviceSkill) {
		// 初心者的潛力爆發
		int sp_cost = 30;
		if (player->getSP() < sp_cost) {
			cout << "SP不足！需要 " << sp_cost << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== 潛力爆發 ===" << endl;
		Sleep(800);

		cout << player->getname() << " 爆發出潛在力量！" << endl;
		Sleep(1500);

		player->delSP(sp_cost);
		cout << "消耗 " << sp_cost << " SP!" << endl;
		Sleep(800);

		// 回復HP
		int heal = player->getMAXHP() / 4;  // 回復25%最大生命值
		player->addHP(heal);
		cout << "恢復了 " << heal << " 點生命值！" << endl;
		Sleep(800);

		// 造成傷害
		int damage = player->getBaseAttack() * 2;  // 2倍基礎攻擊力
		if (player->getWeapon()) {
			damage += player->getWeapon()->getattackbonus() * 2;
		}

		enemy->gethurt(damage);
		cout << "對 " << enemy->getname() << " 造成 " << damage << " 點傷害！" << endl;
	}
	else {
		CSkill* skill = player->getEquippedSkill(2);
		// 檢查SP是否足夠
		if (player->getSP() < skill->getSpCost()) {
			cout << "SP不足！需要 " << skill->getSpCost() << " SP!" << endl;
			Sleep(1000);
			return 0;
		}

		cout << "\n=== " << skill->getName() << " ===" << endl;
		Sleep(800);

		// 根據技能類型執行不同效果，威力提升50%
		switch (skill->getType()) {
		case SkillType::PHYSICAL: {
			int damage = skill->getPower() * (1 + player->getStrength() * 0.1) * 1.5;
			if (player->getWeapon()) {
				damage += player->getWeapon()->getattackbonus() * 2;
			}
			enemy->gethurt(damage);
			cout << player->getname() << " 使用終極技 " << skill->getName()
				<< "，對 " << enemy->getname() << " 造成驚人的 " << damage << " 點傷害！" << endl;
			break;
		}

		case SkillType::MAGICAL: {
			int damage = skill->getPower() * (1 + player->getIntelligence() * 0.15) * 1.5;
			enemy->gethurt(damage);
			cout << player->getname() << " 施放終極魔法 " << skill->getName()
				<< "，對 " << enemy->getname() << " 造成驚人的 " << damage << " 點魔法傷害！" << endl;
			break;
		}

		case SkillType::HEAL: {
			int heal = skill->getPower() * (1 + player->getIntelligence() * 0.2) * 1.5;
			player->addHP(heal);
			cout << player->getname() << " 使用強效 " << skill->getName()
				<< "，恢復了 " << heal << " 點生命值！" << endl;
			break;
		}

		case SkillType::BUFF: {
			int bonus = skill->getPower() * (1 + player->getStrength() * 0.1) * 1.5;
			player->setBaseAttack(player->getBaseAttack() + bonus);
			cout << player->getname() << " 使用強化 " << skill->getName()
				<< "，攻擊力大幅提升了 " << bonus << " 點！" << endl;
			break;
		}
		}

		// 消耗SP
		player->delSP(skill->getSpCost());
	}

	Sleep(1000);
	// 寵物追擊部分
	CGlobalInfo::parser->executePetAttack(enemy, player);

	if (enemy->isdead()) {
		handle_battle_end(player, enemy);
		return 0;
	}

	// 怪物反擊
	Sleep(1000);
	cout << "\n" << enemy->getname() << " 發動反擊！" << endl;
	Sleep(800);

	int monster_base_damage = enemy->physicaldamage();
	int defense = player->getDefense();
	double damage_reduction = defense / 100.0;
	if (damage_reduction > 1.0) damage_reduction = 1.0;

	int monster_final_damage = static_cast<int>(monster_base_damage * (1.0 - damage_reduction));
	if (monster_final_damage < 0) monster_final_damage = 0;

	player->gethurt(monster_final_damage);
	cout << enemy->getname() << " 對你造成 " << monster_final_damage << " 點傷害！" << endl;
	Sleep(1000);

	fightstatus(enemy, player);

	if (player->isdead()) {
		cout << "\n你被打敗了！" << endl;
		Sleep(1000);
		cout << "按任意鍵繼續..." << endl;
		system("pause > nul");
		handle_game_over();
		return 0;
	}

	return 0;
}

int function_job_change(vector<string>& tokens) {
	// 檢查並獲取玩家
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "錯誤: 無法執行轉職命令!" << endl;
		return 0;
	}

	CFighter* player_fighter = (CFighter*)player;

	// 顯示職業選項
	cout << "\n=== 職業轉換 ===" << endl;
	if (player_fighter->getLevel() < JOB_CHANGE_LEVEL) {
		cout << "你的等級不足以轉職！需要達到 " << JOB_CHANGE_LEVEL << " 級。" << endl;
		cout << "當前等級: " << player_fighter->getLevel() << endl;
		return 0;
	}

	if (player_fighter->getJobClass() != JobClass::NOVICE) {
		cout << "你已經轉職過了！無法再次轉職。" << endl;
		return 0;
	}

	cout << "\n請選擇要轉職的職業:" << endl;
	cout << "1. 戰士 - 專精於近戰攻擊與防禦" << endl;
	cout << "  - 獲得額外生命值與防禦力" << endl;
	cout << "  - 特殊技能: 猛力一擊、戰吼" << endl;
	cout << "\n2. 法師 - 擅長魔法攻擊與輔助" << endl;
	cout << "  - 獲得額外魔力值與智力" << endl;
	cout << "  - 特殊技能: 火球術、治癒術" << endl;
	cout << "\n請選擇 (1-2, 0放棄): ";

	int choice;
	cin >> choice;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	switch (choice) {
	case 0:
		cout << "你決定暫時不轉職。" << endl;
		break;

	case 1:
		if (player_fighter->changeJob(JobClass::WARRIOR)) {
			cout << "\n轉職成功！使用 skill 指令來查看和管理技能。" << endl;
		}
		break;

	case 2:
		if (player_fighter->changeJob(JobClass::MAGE)) {
			cout << "\n轉職成功！使用 skill 指令來查看和管理技能。" << endl;
		}
		break;

	default:
		cout << "無效的選擇！" << endl;
	}

	return 0;
}
int function_skill_management(vector<string>& tokens) {
	CLifeEntity* player = CGlobalInfo::user->get_user();
	if (!player || player->isA() != efighter) {
		cout << "錯誤: 無法執行技能管理命令!" << endl;
		return 0;
	}

	CFighter* fighter = (CFighter*)player;

	// 如果還沒轉職
	if (fighter->getJobClass() == JobClass::NOVICE) {
		cout << "你還沒有轉職，無法使用技能系統！" << endl;
		cout << "達到15級後可以使用 job 指令進行轉職。" << endl;
		return 0;
	}

	while (true) {
		cout << "\n=== 技能管理 ===" << endl;
		fighter->showSkills();  // 顯示技能列表和快捷鍵設置

		cout << "\n請選擇操作:" << endl;
		cout << "1. 升級技能" << endl;
		cout << "2. 設置快捷鍵" << endl;
		cout << "0. 離開技能管理" << endl;
		cout << "選擇: ";

		int choice;
		if (!(cin >> choice)) {
			cin.clear();  // 清除錯誤狀態
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "無效的輸入！請重試。" << endl;
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 0) break;

		switch (choice) {
		case 1: {
			if (fighter->getSkillPoints() <= 0) {
				cout << "你沒有可用的技能點數！" << endl;
				continue;
			}

			cout << "選擇要升級的技能 (1-" << fighter->getSkills().size() << "): ";
			int skillChoice;
			if (!(cin >> skillChoice)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "無效的輸入！" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (skillChoice > 0 && skillChoice <= fighter->getSkills().size()) {
				fighter->upgradeSkill(skillChoice - 1);
			}
			else {
				cout << "無效的選擇！" << endl;
			}
			break;
		}

		case 2: {
			cout << "選擇要設置的快捷鍵位置 (1:X, 2:C): ";
			int slot;
			if (!(cin >> slot)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "無效的輸入！" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			// 調整slot的值以匹配實際的陣列索引（1->1, 2->2）
			if (slot < 1 || slot > 2) {
				cout << "無效的快捷鍵位置！必須是1或2。" << endl;
				continue;
			}

			cout << "選擇要綁定的技能 (1-" << fighter->getSkills().size() << "): ";
			int skillChoice;
			if (!(cin >> skillChoice)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "無效的輸入！" << endl;
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (skillChoice > 0 && skillChoice <= fighter->getSkills().size()) {
				fighter->setSkillHotkey(slot, skillChoice - 1);  // 直接使用slot，不需要額外轉換
				cout << "快捷鍵設置成功！" << endl;
			}
			else {
				cout << "無效的技能選擇！" << endl;
			}
			break;
		}

		default:
			cout << "無效的選擇！請輸入0-2之間的數字。" << endl;
			continue;
		}
	}

	return 0;
}
int function_logout(vector<string>& tokens) {
	// 檢查玩家是否已登入
	if (!CGlobalInfo::user || !CGlobalInfo::user->get_user()) {
		cout << "你尚未登入!" << endl;
		return 0;
	}

	cout << "\n=== 登出確認 ===" << endl;
	cout << "確定要登出嗎? 系統將保存所有角色資料 (Y/N): ";

	char confirm;
	cin >> confirm;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (toupper(confirm) != 'Y') {
		cout << "取消登出。" << endl;
		return 0;
	}

	// 保存角色資料
	string username = AccountManager::getInstance().getCurrentUsername();
	CFighter* fighter = (CFighter*)CGlobalInfo::user->get_user();

	if (!fighter) {
		cout << "錯誤: 無法獲取角色數據!" << endl;
		return 0;
	}

	cout << "\n正在保存角色資料..." << endl;
	Sleep(500);

	// 確保存檔目錄存在
	if (!CreateDirectoryA("saves", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			cout << "錯誤: 無法創建存檔目錄!" << endl;
			return 0;
		}
	}

	// 創建備份目錄
	if (!CreateDirectoryA("saves\\backups", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			cout << "警告: 無法創建備份目錄!" << endl;
			// 繼續執行，因為這不是致命錯誤
		}
	}

	// 獲取角色特定的存檔路徑
	auto& saveManager = SaveDirectoryManager::getInstance();
	string savePath = saveManager.getCharacterSavePath(username, fighter->getname());

	// 如果存檔已存在，創建備份
	if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
		string backupPath = saveManager.getCharacterBackupPath(username, fighter->getname());

		if (CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE)) {
			cout << "已創建角色資料備份" << endl;
		}
		else {
			cout << "警告: 無法創建備份文件" << endl;
			// 繼續執行，因為這不是致命錯誤
		}
	}

	// 保存當前角色資料
	bool saveSuccess = SaveSystem::saveCharacterData(fighter, savePath);
	if (!saveSuccess) {
		cout << "警告: 角色資料保存失敗!" << endl;
		cout << "是否仍要繼續登出? (Y/N): ";
		cin >> confirm;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (toupper(confirm) != 'Y') {
			cout << "取消登出。" << endl;
			return 0;
		}
	}
	else {
		cout << "角色資料保存成功!" << endl;
	}

	// 登出玩家
	AccountManager::getInstance().logout();
	cout << "登出成功！" << endl;
	Sleep(1000);

	// 清理當前遊戲狀態
	CGlobalInfo::user->set_user(nullptr);
	system("cls");  // 清空畫面

	// 返回特殊值1表示需要返回登入畫面
	return 1;
}