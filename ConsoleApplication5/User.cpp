#include <iostream>
#include "LifeEntity.h"
#include "User.h"
#include "def.h"
#include "Fighter.h"

CUser::CUser (){
	user = NULL;
};	

void CUser::set_user (CLifeEntity *in_usr){
	user = in_usr;
}

int CUser::goto_next_city (int next){
	if (user->isA () == efighter){
		return ((CFighter *) user)->goto_next_city (next);
	}
	cout << "不是一般戰士" << endl;
	return -1;
}

int CUser::get_current_city (){
	if (user->isA () == efighter){
		return ((CFighter *) user)->get_current_city ();
	}
}