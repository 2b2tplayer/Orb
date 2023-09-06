class Property {
protected:
	template<typename Type>
	inline Type prop(uintptr_t offset) {
		return *(Type*)((uintptr_t)this + offset);
	}
};

class Object : Property {
	int team();
	bool visible();
	bool targetable();
	float attackrange();
public:
	FLOAT3 position();
	float health();
	float height();
	float AttackDelay();
	float AttackWindup();
	bool IsAlive();
	bool AttackableFor(Object*);
	bool InRangeOf(Object*);
};

extern Object* last_object;

class ObjList : Property {
	Object** list();
	int size();
public:
	Object* GetLowestHealth(bool);
};