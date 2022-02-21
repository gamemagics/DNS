#include "dialogue_box.h"

#include <GodotGlobal.hpp>
#include <Control.hpp>
#include <SceneTree.hpp>
#include <PoolArrays.hpp>
#include <Input.hpp>

#include "dialogue_loader.h"

using namespace godot;

void DialogueBox::_register_methods() {
    register_method("_init", &DialogueBox::_init);
    register_method("_process", &DialogueBox::_process);
    register_method("_ready", &DialogueBox::_ready);
    register_method("_unhandled_input", &DialogueBox::_unhandled_input);
    register_method("StartDialogue", &DialogueBox::StartDialogue);

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
    _next_key = "dialogue_next";
    _data = nullptr;
    _speed = 0.1f;
}

void DialogueBox::_process(float delta) {
    switch (_status) {
        case DialogueStatus::IDLE:
            UpdateIdle();
            break;
        case DialogueStatus::PLAY:
        case DialogueStatus::WAIT:
        default: // DISABLE
            break;
    }
}

void DialogueBox::_ready() {
    Disable();

    _tween = get_node<Tween>(_tween_path);
    if (_tween == nullptr) {
        Godot::print_error("Tween is not set.", "DialogueBox::_ready", __FILE__, __LINE__);
    }

    _content_node = get_node<RichTextLabel>(_content_path);
    if (_tween == nullptr) {
        Godot::print_error("Content node is not set.", "DialogueBox::_ready", __FILE__, __LINE__);
    }
}

void DialogueBox::_unhandled_input(const Ref<InputEvent> event) {
    switch (_status) {
        case DialogueStatus::PLAY:
            UpdatePlay();
            break;
        case DialogueStatus::WAIT:
            UpdateWait();
            break;
        case DialogueStatus::IDLE:
        default: // DISABLE
            break;
    }
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

    _status = DialogueStatus::IDLE;
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

void DialogueBox::UpdateIdle() {
    if (_data == nullptr) {
        Godot::print_error("Dialogue data is not set.", "DialogueBox::UpdateIdle", __FILE__, __LINE__);
        return;
    }

    using CommandType = DialogueData::CommandType;

    auto& line = _data->Next(-1);
    if (line.type == CommandType::QUIT) {
        emit_signal("on_dialogue_end");
        Disable();
    }
    else if (line.type == CommandType::SHOW) {
        String text;
        if (line.relative) {
            text = (Object::tr(line.content[0]));
        }
        else {
            text = (line.content[0]);
        }

        _content_node->set_bbcode(text);
        if (line.time > 0.0f) {
            _speed = 1.0f / line.time;
        }
        
        _content_node->set_percent_visible(0.0);
        _tween->interpolate_property(_content_node, "percent_visible", 0, 1,
                                    _speed * text.length(), Tween::TRANS_LINEAR);
        _tween->start();

        _status = DialogueStatus::PLAY;
    }
}

void DialogueBox::UpdatePlay() {
    if (Input::get_singleton()->is_action_pressed(_next_key)) {
        _content_node->set_percent_visible(1.0);
        get_tree()->set_input_as_handled();
    }
    
    if (_content_node->get_percent_visible() >= 1.0) {
        _tween->stop_all();
        _status = DialogueStatus::WAIT;
        Show(_hint_path);
    }
}

void DialogueBox::UpdateWait() {
    if (Input::get_singleton()->is_action_pressed(_next_key)) {
        _status = DialogueStatus::IDLE;
        get_tree()->set_input_as_handled();
        Hide(_hint_path);
    }
}

void DialogueBox::Disable() {
    Hide(_background_path);
    Hide(_content_path);
    Hide(_name_path);
    Hide(_avatar_path);
    Hide(_choices_path);
    Hide(_hint_path);

    _status = DialogueStatus::DISABLE;
}