#include "pch.h"

int32_t Object::team() {
	return *(int32_t*)((uintptr_t)this + 0x3C);
}

XMFLOAT3 Object::position() {
	return *(XMFLOAT3*)((uintptr_t)this + 0x220);
}

bool Object::visible() {
	return *(bool*)((uintptr_t)this + 0x310);
}

bool Object::targetable() {
	return *(bool*)((uintptr_t)this + 0xEB0);
}

float Object::health()
{
	return *(float*)((uintptr_t)this + 0x1058);
}

float Object::base_attack() {
	return *(float*)((uintptr_t)this + 0x1654);
}

float Object::bonus_attack(){
	return *(float*)((uintptr_t)this + 0x15C0);
}

float Object::attack_range()
{
	return *(float*)((uintptr_t)this + 0x169C);
}

float Object::br() const{
	return Functions::GetRadius(this) - 1.f;
}

float Object::acd() const{
	return Functions::GetAttackCastDelay(this) + 0.01f;
}

float Object::ad() const{
	return Functions::GetAttackDelay(this) + 0.01f;
}


float Object::DistanceTo(Object* other) {
	float dx = position().x - other->position().x;
	float dy = position().y - other->position().y;
	float dz = position().z - other->position().z;
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

bool Object::IsFacing(Object* other) {
	auto v1 = XMLoadFloat3(new XMFLOAT3(other->position().x - position().x, 0.f, other->position().z - position().z));
	auto v2 = XMLoadFloat3((XMFLOAT3*)((uintptr_t)this + 0x2190));
	return XMVectorGetX(XMVector3AngleBetweenVectors(v1, v2)) < 1.96349f;
}