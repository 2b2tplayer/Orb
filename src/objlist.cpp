#include "pch.h"

Object* ObjList::GetBestTargetFor(const Object* obj) const {
	extern std::unique_ptr<Functions> f;
	Object* ret = nullptr;
	for(int i = 0; i < size; i++) {
		const auto cur = list[i];
		if(f->IsAlive(cur)
			&& cur->team != obj->team
			&& cur->visible
			&& cur->targetable
			&& cur->DistanceTo(obj) <= obj->attack_range + f->GetRadius(obj)
			&& (!ret || cur->health < ret->health)) {
			ret = cur;
		}
	}
	return ret;
}
