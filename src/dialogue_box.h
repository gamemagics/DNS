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
#include <AudioStream.hpp>
#include <AudioStreamPlayer.hpp>

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

    inline void AddCharacter(Ref<Texture> avatar, Ref<AudioStream> sound, String name) {
        _avatars[name] = avatar;
        _sounds[name] = sound;
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
    NodePath _effect_path;

    String _next_key;
    String _up_key;
    String _down_key;

    DialogueData* _data;
    Node* _script_node;
    Tween* _tween;
    RichTextLabel* _content_node;
    Label* _choices[4];
    AudioStreamPlayer* _player;

    Dictionary _avatars;
    Dictionary _sounds;
    
    float _speed;
    bool _selecting;
    int _selected;
    int _select_range;
    int _shown;

    DialogueStatus _status;

    void Hide(NodePath path);
    void Show(NodePath path);

    void UpdateIdle();
    void UpdatePlay();
    void UpdateWait();
    void UpdateSound();

    void Disable();

    void ParseVariables(String& text);

    void PlayChoicesAnimation(float percent);
};

} // namespace godot


#endif // __DIALOGUE_BOX_H__