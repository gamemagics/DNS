#include "dialogue_data.h"

#include <GodotGlobal.hpp>

using namespace godot;

void DialogueData::_register_methods() {
}

const DialogueData::DialogueLine& DialogueData::Next(int choice/* = -1*/) {
    // TODO:
    return _lines[_current];
}