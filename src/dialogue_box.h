#ifndef __DIALOGUE_BOX_H__
#define __DIALOGUE_BOX_H__

#include <Godot.hpp>
#include <Control.hpp>
#include <CanvasLayer.hpp>
#include <Tween.hpp>
#include <RichTextLabel.hpp>
#include <InputEvent.hpp>
#include <TextureRect.hpp>
#include <Label.hpp>
#include <Texture.hpp>
#include <Dictionary.hpp>

#include "dialogue_data.h"

namespace godot {

class DialogueBox : public CanvasLayer {
    GODOT_CLASS(DialogueBox, CanvasLayer);

public:
    static void _register_methods();

    void _init();

    void _process(float delta);

    void _unhandled_input(const Ref<InputEvent> event);

    void _ready();

    void StartDialogue(String filename);

    String Call(String name);

    void FinishPlaying();

    inline void AddCharacter(Ref<Texture> avatar, String name) {
        _avatars[name] = avatar;
    }

    void SetCurrentCharacter(String name);
private:
    enum class DialogueStatus {
        IDLE, PLAY, WAIT, DISABLE
    };

    NodePath _background_path;
    NodePath _content_path;
    NodePath _name_path;
    NodePath _avatar_path;
    NodePath _choices_path;
    NodePath _tween_path;
    NodePath _hint_path;

    String _next_key;
    String _up_key;
    String _down_key;

    DialogueData* _data;
    Node* _script_node;
    Tween* _tween;
    RichTextLabel* _content_node;
    Label* _choices[4];

    Dictionary _avatars;
    
    float _speed;
    bool _selecting;
    int _selected;
    int _select_range;

    DialogueStatus _status;

    void Hide(NodePath path);
    void Show(NodePath path);

    void UpdateIdle();
    void UpdatePlay();
    void UpdateWait();

    void Disable();

    void ParseVariables(String& text);
};

} // namespace godot


#endif // __DIALOGUE_BOX_H__