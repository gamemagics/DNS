#ifndef __DIALOGUE_BOX_H__
#define __DIALOGUE_BOX_H__

#include <Godot.hpp>
#include <Control.hpp>

namespace godot {

class DialogueBox : public Control {
    GODOT_CLASS(DialogueBox, Control);

public:
    static void _register_methods();

    void _init();
};

} // namespace godot


#endif // __DIALOGUE_BOX_H__