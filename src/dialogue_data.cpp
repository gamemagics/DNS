#include "dialogue_data.h"

#include <GodotGlobal.hpp>

using namespace godot;

const DialogueData::DialogueLine& DialogueData::Next(int choice/* = -1*/) {
    const NextLine& next_line = _next[_current];    
    if (choice == -1) {
        if (!next_line.function.empty()) {
            // TODO:
        }
        else {
            _current = next_line.index;
        }
    } 
    else {
        if (!next_line.function.empty()) {
            // TODO:
        }
        else {
            _current = next_line.choices[choice];
        }
    }

    if (_current >= _lines.size()) {
        Godot::print_error("error in dialouge file!", "DialogueData::Next", __FILE__, __LINE__);
        _current = 0;
    }
    else if (_current < 0) {
        _current = 0;
        return {}; // end dialogue.
    }

    return _lines[_current];
}

void DialogueData::Push(const DialogueData::DialogueLine& line, int index) {
    NextLine next_line{};
    next_line.index = index;
    _next.push_back(next_line);
    _lines.push_back(line);
}

void DialogueData::Push(const DialogueData::DialogueLine& line, String function) {
    NextLine next_line{};
    next_line.function = function;
    _next.push_back(next_line);
    _lines.push_back(line);
}

void DialogueData::Push(const DialogueData::DialogueLine& line, int choices[4]) {
    NextLine next_line{};
    next_line.choices[0] = choices[0];
    next_line.choices[1] = choices[1];
    next_line.choices[2] = choices[2];
    next_line.choices[3] = choices[3];
    _next.push_back(next_line);
    _lines.push_back(line);
}