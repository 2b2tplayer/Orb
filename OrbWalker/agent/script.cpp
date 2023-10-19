#include "pch.hpp"

#include "script.hpp"

#include "config/config.hpp"
#include "memory/function.hpp"
#include "memory/global.hpp"

void script::update() {
  game_time = GameTime();

  check_orb_state();
  check_marked_object();

  Draw([&] {
    if(config::inst().show_attack_range) Circle(self->position(), real_range(), 0xffffffff, 1.5f);
    if(markedObject && markedObject->IsAlive() && markedObject->visible()) {
      Circle(markedObject->position(), markedObject->BonusRadius(), 0xff0c9d00, 4.5f);
    }
  });

  if(!CanSendInput()) return;

  if(orbState != OrbState::Off) {
    if(is_reloading()) idle();
    else attack();
  }
}

bool script::can_attack() { return self->state() & CanAttack; }

bool script::can_do_action() {
  if(game_time < last_action_time + interval) return false;
  last_action_time = game_time;
  return true;
}

bool script::is_reloading() { return game_time < last_attack_time + self->AttackDelay() - 0.1f; }

bool script::is_attacking() { return game_time < last_attack_time + self->AttackWindup() - 0.02f; }

void script::idle() {
  if(!is_attacking() && can_do_action()) Move2Mouse();
}

void script::attack() {
  if(const auto obj = get_attack_target(); obj && can_attack() && can_do_action()) {
    AttackObject(obj);
  } else idle();
}

float script::real_range() { return self->attack_range() + self->BonusRadius(); }

void script::check_marked_object() {
  if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    if(const auto obj = *objUnderMouse; heros->contains(obj) && obj->IsEnemy()) markedObject = obj;
  }
}

void script::check_orb_state() {
  if(ImGui::IsKeyDown(config::inst().kite_key)) orbState = OrbState::Kite;
  else if(ImGui::IsKeyDown(config::inst().clean_key)) orbState = OrbState::Clear;
  else orbState = OrbState::Off;
}

Object *script::get_attack_target() {
  if(orbState == OrbState::Kite) {
    auto target = heros->best_object([&](Object *obj) {
      return in_attack_range(obj) && obj == markedObject;
    });
    if(!target) {
      target = heros->best_object([&](Object *obj) {
        return in_attack_range(obj);
      });
    }
    return target;
  }
  if(orbState == OrbState::Clear) {
    auto target = minions->best_object([&](Object *obj) {
      return in_attack_range(obj);
    });
    if(!target) {
      target = turrets->best_object([&](Object *obj) {
        return in_attack_range(obj);
      });
    }
    if(!target) {
      target = inhibs->best_object([&](Object *obj) {
        return in_attack_range(obj);
      });
    }
    return target;
  }
  return nullptr;
}

Object *script::get_skill_target(float range) {
  if(orbState == OrbState::Kite) {
    auto target = heros->best_object([&](Object *obj) {
      return in_skill_range(obj, range) && obj == markedObject;
    });
    if(!target) {
      target = heros->best_object([&](Object *obj) {
        return in_skill_range(obj, range);
      });
    }
    return target;
  }
  if(orbState == OrbState::Clear) {
    return minions->best_object([&](Object *obj) {
      return in_skill_range(obj, range);
    });
  }
  return nullptr;
}

bool script::has_buff(Object *obj, std::string_view name) {
  return std::ranges::any_of(obj->buffs(), [name, this](Buff *buff) {
    return buff->name() == name && buff->starttime() <= game_time && buff->endtime() >= game_time;
  });
}

bool script::in_attack_range(Object *obj) {
  return obj->position() - self->position() <= self->attack_range() + self->BonusRadius() + obj->BonusRadius();
}

bool script::in_skill_range(Object *obj, float range) { return obj->position() - self->position() <= range; }
