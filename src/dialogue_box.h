#ifndef __DIALOGUE_BOX_H__
#define __DIALOGUE_BOX_H__

#include <Godot.hpp>
#include <CanvasLayer.hpp>

namespace godot {

class DialogueBox : public CanvasLayer {
    GODOT_CLASS(DialogueBox, CanvasLayer);

public:
    static void _register_methods();

    void _init();

    void _process();

    void _ready();
private:
    NodePath _background_path;
    NodePath _content_path;
    NodePath _name_path;
    NodePath _avatar_path;
    NodePath _choices_path;

    void _hide(NodePath path);
};

} // namespace godot


#endif // __DIALOGUE_BOX_H__