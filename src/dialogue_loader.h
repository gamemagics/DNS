#ifndef __DIALOGUE_LOADER_H__
#define __DIALOGUE_LOADER_H__

#include <Godot.hpp>
#include <Object.hpp>

namespace godot {

class DialogueLoader : public Object {
    GODOT_CLASS(DialogueLoader, Object);
public:
    static void _register_methods();
private:

protected:
};

} // namespace godot

#endif