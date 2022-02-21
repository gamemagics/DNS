#include "dialogue_box.h"

#include <GodotGlobal.hpp>
#include <Control.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "dialogue_loader.h"

using namespace godot;

void DialogueBox::_register_methods() {
    register_method("_init", &DialogueBox::_init);
    register_method("_process", &DialogueBox::_process);
    register_method("_ready", &DialogueBox::_ready);
    register_method("StartDialogue", &DialogueBox::StartDialogue);
    register_method("SkipAnimation", &DialogueBox::SkipAnimation);
    register_method("Next", &DialogueBox::Next);

    register_signal<DialogueBox>(const_cast<char*>("on_dialogue_end"));
}

void DialogueBox::_init() {
    _background_path = "Background";
    _content_path = "Content";
    _name_path = "Name";
    _avatar_path = "Avatar";
    _choices_path = "Choices";
    _tween_path = "Tween";
    _hint_path = "NextHint";
    _data = nullptr;
}

void DialogueBox::_process(float delta) {

}

void DialogueBox::_ready() {
    Hide(_background_path);
    Hide(_content_path);
    Hide(_name_path);
    Hide(_avatar_path);
    Hide(_choices_path);
    Hide(_hint_path);
}

void DialogueBox::Hide(NodePath path) {
    auto* control_node = this->get_node<Control>(path);
    if (control_node != nullptr) {
        control_node->hide();
    }
    else {
        Godot::print_error("Can't get node " + path, "DialogueBox::Hide", __FILE__, __LINE__);
    }
}

void DialogueBox::Show(NodePath path) {
    auto* control_node = this->get_node<Control>(path);
    if (control_node != nullptr) {
        control_node->show();
    }
    else {
        Godot::print_error("Can't get node " + path, "DialogueBox::Hide", __FILE__, __LINE__);
    }
}

void DialogueBox::StartDialogue(String filename) {
    if (_data != nullptr) {
        _data->free();
    }

    _data = DialogueLoader::GetInstance()->LoadDialogue(filename);
    if (_data == nullptr) {
        Godot::print_error("Can't load dialogue " + filename, "DialogueBox::StartDialogue", __FILE__, __LINE__);
        return;
    }

    _data->setDialogueBoxRef(this);
    _script_node = get_tree()->get_current_scene()->get_node(_data->GetScriptNode());

    Show(_background_path);
    Show(_content_path);
    Show(_name_path);
    Show(_avatar_path);
    Show(_choices_path);
}

void DialogueBox::SkipAnimation() {

}

void DialogueBox::Next() {

}

String DialogueBox::Call(String name) {
    if (_script_node == nullptr) {
        Godot::print_error("No script node given.", "DialogueBox::Call", __FILE__, __LINE__);
        return "";
    }

    Variant res = _script_node->call(name);
    if (res.get_type() == res.NIL) {
        return "";
    }
    else if (res.get_type() == res.STRING) {
        return res;
    }
    else {
        Godot::print_error("Type is not allowed.", "DialogueBox::Call", __FILE__, __LINE__);
        return "";
    }
}