// GlobalInfo.cpp
#include "GlobalInfo.h"
#include "MapData.h"
#include "MonsterData.h"
#include "User.h"
#include "ItemData.h"
#include "CmdParser.h"

// 靜態成員定義
CMapData* CGlobalInfo::map_data = NULL;
CMonsterData* CGlobalInfo::monster_data = NULL;
CUser* CGlobalInfo::user = NULL;
CItemData* CGlobalInfo::itm_data = NULL;
CCmdParser* CGlobalInfo::parser = NULL;