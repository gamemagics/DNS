#include "dialogue_data.h"

#include <GodotGlobal.hpp>
#include <PoolArrays.hpp>

#include "dialogue_box.h"

using namespace godot;

void DialogueData::_register_methods() {
}

const DialogueData::DialogueLine& DialogueData::Next(int choice/* = -1*/) {
    bool flag = true;
    while (flag) {
        const DialogueLine& line = _lines[_current];
        switch (line.type) {
            case CommandType::CHARACTER:
                ProcessCharacter();
                ++_current;
                break;
            case CommandType::EXECUTE:
                ProcessExecute();
                ++_current;
                break;
            case CommandType::GOTO:
                _current = ProcessGoto();
                break;
            case CommandType::SELECT:
                _current = ProcessSelect(choice);
                if (choice != -1) break;
            case CommandType::QUIT:
            default:
                flag = false;
                break;
        }
    }

    return _lines[_current++];
}

int DialogueData::ProcessSelect(int choice) {
    if (choice == -1) {
        return _current;
    }

    DialogueLine& line = _lines[_current];
    String name = line.jump[choice];
    int runtime = line.runtime & (1 << choice);

    if (runtime) {
        name = Execute(name);
    }

    if (!_mapping.has(name)) {
        Godot::print_error("Can't find label" + name, "DialogueData::ProcessSelect", __FILE__, __LINE__);
        return _current;
    }

    return _mapping[name];
}

void DialogueData::ProcessCharacter() {
    // TODO:
}

int DialogueData::ProcessGoto() {
    DialogueLine& line = _lines[_current];
    String name = line.jump[0];

    if (line.runtime) {
        name = Execute(name);
    }

    if (!_mapping.has(name)) {
        Godot::print_error("Can't find label" + name, "DialogueData::ProcessGoto", __FILE__, __LINE__);
        return _current;
    }

    return _mapping[name];
}

void DialogueData::ProcessExecute() {
    DialogueLine& line = _lines[_current];
    Execute(line.content[0]);
}


String DialogueData::Execute(String name) {
    return _db->Call(name); // TODO:
}