#include "stdafx.hpp"

using namespace offset;

int Object::team() {
  return prop<int>(0x3C);
}

bool Object::visible() {
  return prop<bool>(0x310);
}

bool Object::targetable() {
  return prop<bool>(0xEB0);
}

float Object::attackrange() {
  return prop<float>(0x16B4);
}

FLOAT3 Object::position() {
  return prop<FLOAT3>(0x220);
}

float Object::health() {
  return prop<float>(0x1058);
}

float Object::height() {
  const auto characterHeight = *(float *)(*(uintptr_t *)(prop<uintptr_t>(0x3528) + 0x28) + 0xC4);
  const auto scale = prop<float>(0x1684);
  return characterHeight * scale;
}

float Object::AttackDelay() {
  using fnAttackDelay = float(__fastcall *)(Object *);
  return ((fnAttackDelay)oAttackDelay)(this);
}

float Object::AttackWindup() {
  using fnAttackWindup = float(__fastcall *)(Object *);
  return ((fnAttackWindup)oAttackWindup)(this);
}

float Object::RealAttackRange() {
  using fnBonusRadius = float(__fastcall *)(Object *);
  return attackrange() + ((fnBonusRadius)oBonusRadius)(this);
}

bool Object::IsAlive() {
  using fnIsAlive = bool(__fastcall *)(Object *);
  return ((fnIsAlive)oIsAlive)(this);
}

bool Object::AttackableFor(Object *other) {
  return team() != other->team() && visible() && targetable() && IsAlive();
}

bool Object::InRangeOf(Object *other) {
  using fnBonusRadius = float(__fastcall *)(Object *);
  return position() - other->position() - ((fnBonusRadius)oBonusRadius)(other) <
         other->attackrange() + ((fnBonusRadius)oBonusRadius)(this);
}

Object **ObjList::list() {
  return prop<Object **>(0x8);
}

int ObjList::size() {
  return prop<int>(0x10);
}

Object *last_object = nullptr;

Object *ObjList::GetLowestHealth(bool dif) {
  Object *me = script::LocalPlayer();
  using namespace std;
  auto filtered = span(list(), size()) | views::filter([me](Object *obj) {
                    return obj->AttackableFor(me) && obj->InRangeOf(me);
                  });
  if(filtered.empty()) return nullptr;
  return *ranges::min_element(filtered, [dif](Object *o, Object *smallest) {
    if(!dif) return o->health() < smallest->health();
    return (o->health() < smallest->health() && o != last_object) || smallest == last_object;
  });
}
