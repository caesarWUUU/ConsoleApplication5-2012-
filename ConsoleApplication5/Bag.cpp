#include <iostream>
#include "Bag.h"
#include "Item.h"

using namespace std;

CBagEntry::CBagEntry(CItem *in_item, int in_num){
	itm = in_item; 
	number = in_num;
}

CBag::CBag (){
	LIST_INIT(&listhead);	
}

CBag::~CBag (){
	CBagEntry* ne = listhead.lh_first;
	CBagEntry* tmp = NULL;
	while (ne){
		tmp = ne->next_link.le_next;
		LIST_REMOVE (ne, next_link);
		ne = tmp;
	}
}
void CBag::getAllItems(std::vector<CBagEntry*>& entries) {
	entries.clear();
	CBagEntry* ne = listhead.lh_first;
	while (ne) {
		entries.push_back(ne);
		ne = ne->next_link.le_next;
	}
}
void CBag::getAllItems(std::vector<CBagEntry*>& entries) const {
	entries.clear();
	const CBagEntry* ne = listhead.lh_first;
	while (ne) {
		// 因為我們需要保持向後兼容性，這裡使用 const_cast
		entries.push_back(const_cast<CBagEntry*>(ne));
		ne = ne->next_link.le_next;
	}
}
void CBag::item_insert(CItem* in_item, int num, bool replace) {
	if (!in_item) return;

	CBagEntry* existing = item_lookup(in_item->isA(), in_item->getID());
	if (existing) {
		if (replace) {
			existing->setNum(num);  // 如果是 replace 模式，直接設置數量
		}
		else {
			existing->addNum(num);  // 否則增加數量
		}
		delete in_item;  // 刪除新建的物品，因為已經有一個了
	}
	else {
		// 創建新的物品條目
		CBagEntry* new_entry = new CBagEntry(in_item, num);
		LIST_INSERT_HEAD(&listhead, new_entry, next_link);
	}
}

CBagEntry* CBag::item_lookup (int type, int id){
	CBagEntry* ne = listhead.lh_first;
	for (; ne; ne = ne->next_link.le_next){
		if (ne->itm->isA() == type && ne->itm->getID () == id)
			break;
	}
	return ne;
}

CBagEntry* CBag::item_lookup (int no){
	CBagEntry* ne = listhead.lh_first;
	int countnum = 1;
	for (; ne; ne = ne->next_link.le_next){
		if (no == countnum){						
			break;
		}
		countnum++;
	}
	return ne;
}

void CBag::item_delete (CBagEntry* be){
	LIST_REMOVE (be, next_link);
}

int CBag::showAllItems (){
	CBagEntry* ne = listhead.lh_first;
	CItem *tmp;
	int countnum = 0;
	if (ne)
		cout << "背包內容如下所示：" << endl;
	for (; ne; ne = ne->next_link.le_next){
		tmp = ne->itm;
		cout << ++countnum << ". " << tmp->getName () << ", 數量-> " << ne->number << endl;
	}	
	return countnum;
}

