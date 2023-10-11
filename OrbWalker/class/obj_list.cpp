#include "pch.hpp"

#include "obj_list.hpp"

#include "config/config.hpp"
#include "memory/global.hpp"

using namespace std;

vector<Object *> ObjList::objects_in_range(float range, bool collision) {
  vector<Object *> result;
  result.reserve(size);
  ranges::copy_if(span(data, size), back_inserter(result), [&](Object *obj) {
    return obj->IsValidTarget() && !(obj->IsPlant() || obj->IsWard())
        && obj->position() - self->position() <= range + (collision ? obj->BonusRadius() : 0.f);
  });
  return result;
}

Object *ObjList::best_object_in_range(float range, bool collision) {
  const auto &list = objects_in_range(range, collision);
  auto target = ranges::min_element(list, {}, [](Object *obj) {
    using config::Selector;
    if(config::selector == Selector::distance_closest) {
      return obj->position() - self->position();
    } else {
      return obj->health();
    }
  });
  return target == list.end() ? nullptr : *target;
}