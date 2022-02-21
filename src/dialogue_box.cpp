#include "dialogue_box.h"

#include <regex>

#include <GodotGlobal.hpp>
#include <Control.hpp>
#include <SceneTree.hpp>
#include <PoolArrays.hpp>
#include <Input.hpp>
#include <Theme.hpp>

#include "dialogue_loader.h"

using namespace godot;

void DialogueBox::_register_methods() {
    register_method("_init", &DialogueBox::_init);
    register_method("_process", &DialogueBox::_process);
    register_method("_ready", &DialogueBox::_ready);
    register_method("_unhandled_input", &DialogueBox::_unhandled_input);
    register_method("StartDialogue", &DialogueBox::StartDialogue);
    register_method("FinishPlaying", &DialogueBox::FinishPlaying);
    register_method("AddCharacter", &DialogueBox::AddCharacter);

    register_property<DialogueBox, Dictionary>("avatars", &DialogueBox::_avatars, Dictionary{});

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
    _up_key = "dialogue_up";
    _down_key = "dialogue_down";
    _data = nullptr;
    _speed = 0.1f;
    _selected = -1;
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
    
    _tween->connect("tween_all_completed", this, "FinishPlaying");

    _content_node = get_node<RichTextLabel>(_content_path);
    if (_tween == nullptr) {
        Godot::print_error("Content node is not set.", "DialogueBox::_ready", __FILE__, __LINE__);
    }

    Node* choices = get_node(_choices_path);
    if (choices == nullptr) {
        Godot::print_error("Choices is not set.", "DialogueBox::UpdateIdle", __FILE__, __LINE__);
    }

    for (int i = 0; i < 4; ++i) {
        _choices[i] = choices->get_node<Label>(NodePath{"Choice" + String::num(i)});
        if (_choices[i] == nullptr) {
            Godot::print_error("Choices is not set.", "DialogueBox::UpdateIdle", __FILE__, __LINE__);
            break;
        }
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

void DialogueBox::FinishPlaying() {
    if (_data->NeedToSelect()) {
        _status = DialogueStatus::IDLE;
    }
    else {
        _selecting = false;
        _status = DialogueStatus::WAIT;
        Show(_hint_path);
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

    _data->SetDialogueBoxRef(this);
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

    auto& line = _data->Next(_selected);
    _selected = -1;
    if (line.type == CommandType::QUIT) {
        _data->free();
        _data = nullptr;
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

        ParseVariables(text);
        _content_node->set_bbcode(text);

        if (line.time > 0.0f) {
            _speed = 1.0f / line.time;
        }
        else {
            _speed = 0.1f;
        }
        
        _content_node->set_percent_visible(0.0);
        _tween->interpolate_property(_content_node, "percent_visible", 0, 1,
                                    _speed * text.length(), Tween::TRANS_LINEAR);
        _tween->start();

        _status = DialogueStatus::PLAY;
    }
    else if (line.type == CommandType::SELECT) {
        _selecting = true;
        _selected = 0;
        _select_range = line.content.size();

        for (int i = 0; i < _select_range; ++i) {
            String text;
            if (line.relative & (1 << i)) {
                text = Object::tr(line.content[i]);
            }
            else {
                text = line.content[i];
            }

            _choices[i]->set_text(text);
            _choices[i]->set_visible(true);
        }

        _choices[_selected]->set("custom_colors/font_color", Color(0, 0, 1));

        Show(_choices_path);
        _status = DialogueStatus::WAIT;
    }
}

void DialogueBox::UpdatePlay() {
    if (Input::get_singleton()->is_action_pressed(_next_key)) {
        _content_node->set_percent_visible(1.0);
        get_tree()->set_input_as_handled();

        _tween->stop_all();
        FinishPlaying();
    }
}

void DialogueBox::UpdateWait() {
    if (!_selecting) {
        if (Input::get_singleton()->is_action_pressed(_next_key)) {
            _status = DialogueStatus::IDLE;
            get_tree()->set_input_as_handled();
            Hide(_hint_path);
        }
    }
    else {
        int next = _selected;
        if (Input::get_singleton()->is_action_pressed(_next_key)) {
            _status = DialogueStatus::IDLE;
            get_tree()->set_input_as_handled();

            _choices[_selected]->set("custom_colors/font_color", Color(1, 1, 1));
            for (int i = 0; i < 4; ++i) {
                _choices[i]->set_visible(false);
            }

            Hide(_choices_path);
        }
        else if (Input::get_singleton()->is_action_pressed(_up_key)) {
            next = (_selected + _select_range - 1) % _select_range;
            get_tree()->set_input_as_handled();
        }
        else if (Input::get_singleton()->is_action_pressed(_down_key)) {
            next = (_selected + 1) % _select_range;
            get_tree()->set_input_as_handled();
        }

        if (next != _selected) {
            _choices[_selected]->set("custom_colors/font_color", Color(1, 1, 1));
            _choices[next]->set("custom_colors/font_color", Color(0, 0, 1));

            _selected = next;
        }
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

void DialogueBox::ParseVariables(String& text) {
    while (true) {
        int begin = text.find("{{");
        if (begin == -1) {
            break;
        }

        int end = text.find("}}", begin + 2);
        if (end == -1) {
            break;
        }

        String func = text.substr(begin + 2, end - begin - 2);
        String res = Call(func);
        text = text.replace(text.substr(begin, end + 2 - begin), res);
    }
}

void DialogueBox::SetCurrentCharacter(String name) {
    if (_avatars.has(name)) {
        TextureRect* rect = get_node<TextureRect>(_avatar_path);
        if (rect == nullptr) {
            Godot::print_error("Avatar texture rect is not set.", "DialogueBox::SetCurrentCharacter", __FILE__, __LINE__);
            return;
        }

        rect->set_texture(Object::cast_to<Texture>(_avatars[name]));
    }

    name = Object::tr(name);
    Label* label = get_node<Label>(_name_path);

    if (label == nullptr) {
        Godot::print_error("Name label is not set.", "DialogueBox::SetCurrentCharacter", __FILE__, __LINE__);
        return;
    }

    label->set_text(name);
}
