#include "dialogue_data.h"

#include <GodotGlobal.hpp>

using namespace godot;

void DialogueData::_register_methods() {
}

const DialogueData::DialogueLine& DialogueData::Next(int choice/* = -1*/) {
    // const NextLine& next_line = _next[_current];    
    // if (choice == -1) {
    //     if (!next_line.function.empty()) {
    //         // TODO:
    //     }
    //     else {
    //         _current = next_line.index;
    //     }
    // } 
    // else {
    //     if (!next_line.function.empty()) {
    //         // TODO:
    //     }
    //     else {
    //         _current = next_line.choices[choice];
    //     }
    // }

    // if (_current >= _lines.size()) {
    //     Godot::print_error("error in dialouge file!", "DialogueData::Next", __FILE__, __LINE__);
    //     _current = 0;
    // }
    // else if (_current < 0) {
    //     _current = 0;
    //     return {}; // end dialogue.
    // }

    return _lines[_current];
}