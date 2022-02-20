#ifndef __DIALOGUE_BOX_H__
#define __DIALOGUE_BOX_H__

#include <Godot.hpp>
#include <CanvasLayer.hpp>

#include "dialogue_data.h"

namespace godot {

class DialogueBox : public CanvasLayer {
    GODOT_CLASS(DialogueBox, CanvasLayer);

public:
    static void _register_methods();

    void _init();

    void _process(float delta);

    void _ready();

    void StartDialogue(String filename);

    void SkipAnimation();
    
    void Next();
private:
    NodePath _background_path;
    NodePath _content_path;
    NodePath _name_path;
    NodePath _avatar_path;
    NodePath _choices_path;
    NodePath _tween_path;
    NodePath _hint_path;

    DialogueData* _data;

    void Hide(NodePath path);
    void Show(NodePath path);
};

} // namespace godot


#endif // __DIALOGUE_BOX_H__