#include "dialogue_box.h"

#include <GodotGlobal.hpp>

using namespace godot;

void DialogueBox::_register_methods() {
    register_method("_process", &DialogueBox::_process);
}

void DialogueBox::_init() {
    Godot::print("rua");
}

void DialogueBox::_process() {

}