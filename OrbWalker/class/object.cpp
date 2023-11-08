#include "pch.hpp"

#include "object.hpp"

#include "memory/function.hpp"
#include "memory/offset.hpp"

Object *Object::self() { return Read<Object *>(RVA(oLocalPlayer)); }

Object *Object::obj_under_mouse() { return Read<Object *>(Read<uptr>(RVA(oObjUnderMouse)) + 0x18); }

i32 Object::index() { return MEMBER<i32>(objIndex); }

i32 Object::team() { return MEMBER<i32>(objTeam); }

FLOAT3 Object::position() { return MEMBER<FLOAT3>(objPosition); }

bool Object::visible() { return MEMBER<bool>(objVisible); }

float Object::mana() { return MEMBER<float>(objMana); }

bool Object::targetable() { return MEMBER<bool>(objTargetable); }

float Object::health() { return MEMBER<float>(objHealth); }

float Object::max_health() { return MEMBER<float>(objMaxHealth); }

float Object::attack_range() { return MEMBER<float>(objAttackRange); }

ActionState Object::state() { return MEMBER<ActionState>(objActionState); }

DataStack *Object::dataStack() {
  if (!compare_type_flags(AI)) return nullptr;
  return pMEMBER<DataStack>(objDataStack);
}

std::vector<Buff *> Object::buffs() {
  if (!compare_type_flags(AI)) return {};
  struct BuffStruct {
    Buff *buff;
    uintptr_t pad;
  };
  return MEMBER<std::vector<BuffStruct>>(objBuff) | std::views::transform([](const BuffStruct &bs) { return bs.buff; })
       | std::ranges::to<std::vector>();
}

std::string Object::name() { return MEMBER<RiotString16>(objName).str(); }

// float Object::attackdamage() {
//   return prop<float>(0x166C) + prop<float>(0x15D8);
// }

float Object::AttackDelay() { return call_function<float>(RVA(oAttackDelay), this); }

float Object::AttackWindup() { return call_function<float>(RVA(oAttackWindup), this, 65); }

float Object::BonusRadius() { return call_virtual<37, float>(this); }

void *Object::ops_base() { return pMEMBER<void *>(0x11B8); }

bool Object::IsAlive() { return call_virtual<134, bool>(this); }

bool Object::IsEnemy() { return team() != self()->team(); }

bool Object::IsTargetable() {
  auto flags = MEMBER<int32_t>(objTargetFlags);
  return flags == 4 || flags == 1;
}

bool Object::IsValidTarget() { return visible() && targetable() && IsEnemy() && IsAlive() && IsTargetable(); }

bool Object::compare_type_flags(TypeFlag flag) { return call_function<bool>(RVA(oCompareTypeFlags), this, flag); }

float Object::get_mana_cost(SpellSlot slot) {
  if (slot > SpellSlot_R) return 0.f;
  return MEMBER<float>(objManaCost + slot * 0x18ull);
}

Spell *Object::GetSpell(SpellSlot slot) { return pMEMBER<Spell *>(objSpell)[slot]; }

Object *Object::GetOwner() { return call_function<Object *>(RVA(oGetOwner), this); }