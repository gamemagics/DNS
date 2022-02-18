#include "dialogue_box.h"

#include <GodotGlobal.hpp>
#include <Control.hpp>

#include "dialogue_loader.h"

using namespace godot;

void DialogueBox::_register_methods() {
    register_method("_init", &DialogueBox::_init);
    register_method("_process", &DialogueBox::_process);
    register_method("_ready", &DialogueBox::_ready);
    register_method("StartDialogue", &DialogueBox::StartDialogue);
}

void DialogueBox::_init() {
    _background_path = "Background";
    _content_path = "Content";
    _name_path = "Name";
    _avatar_path = "Avatar";
    _choices_path = "Choices";
    _tween_path = "Tween";
    _data = nullptr;
}

void DialogueBox::_process() {

}

void DialogueBox::_ready() {
    _hide(_background_path);
    _hide(_content_path);
    _hide(_name_path);
    _hide(_avatar_path);
    _hide(_choices_path);
}

void DialogueBox::_hide(NodePath path) {
    auto* control_node = this->get_node<Control>(path);
    if (control_node != nullptr) {
        control_node->hide();
    }
    else {
        Godot::print_error("Can't get node " + path, "DialogueBox::_hide", __FILE__, __LINE__);
    }
}

void DialogueBox::_show(NodePath path) {
    auto* control_node = this->get_node<Control>(path);
    if (control_node != nullptr) {
        control_node->show();
    }
    else {
        Godot::print_error("Can't get node " + path, "DialogueBox::_hide", __FILE__, __LINE__);
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

    _show(_background_path);
    _show(_content_path);
    _show(_name_path);
    _show(_avatar_path);
    _show(_choices_path);
}