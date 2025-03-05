#ifndef BAG_H
#define BAG_H

#include <iostream>
#include <vector>
#include "bsd-list.h"
class CItem;
class SaveSystem;

class CBagEntry {
    friend class CBag;
    friend class CFighter;
    friend class SaveSystem;
public:
    CBagEntry(CItem* in_item = NULL, int in_num = 1);
    void addNum(int adds = 1) { number += adds; }
    void deleteNum(int dels = 1) { number -= dels; }
    int getNum() const { return number; }
    void setNum(int num) { number = num; }
    const CItem* getItem() const { return itm; }

private:
    LIST_ENTRY(CBagEntry) next_link;
    CItem* itm;
    int number;
};

LIST_HEAD(bag_itemlist, CBagEntry);

class CBag {
public:
    CBag();
    ~CBag();
    void item_insert(CItem* in_item, int num = 1, bool replace = false);
    CBagEntry* item_lookup(int type, int id);
    CBagEntry* item_lookup(int no);
    void item_delete(CBagEntry*);
    int showAllItems();

    // 添加 const 和非 const 版本的 getAllItems
    void getAllItems(std::vector<CBagEntry*>& entries);
    void getAllItems(std::vector<CBagEntry*>& entries) const;

private:
    bag_itemlist listhead;
    friend class SaveSystem;
};

#endif