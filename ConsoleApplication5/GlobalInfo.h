#ifndef GLOBALINFO_H
#define GLOBALINFO_H

// �e�V�n��
class CItemData;
class CMapData;
class CCmdParser;
class CUser;
class CMonsterData;

class CGlobalInfo {
public:
    static CMapData* map_data;
    static CMonsterData* monster_data;
    static CUser* user;
    static CItemData* itm_data;
    static CCmdParser* parser;
};

#endif