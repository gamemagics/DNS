#ifndef __DIALOGUE_LOADER_H__
#define __DIALOGUE_LOADER_H__

#include <Godot.hpp>
#include <Object.hpp>

#include "dialogue_data.h"

namespace godot {

class DialogueLoader : public Object {
    GODOT_CLASS(DialogueLoader, Object);
public:
    static void _register_methods();

    static DialogueLoader* GetInstance();

    DialogueData* LoadDialogue(String filename);
private:
    static DialogueLoader* _instance;

    DialogueData* Parse(String content);

    String ParseScript(String& text);
    String ParseCharacter(String& text);
    String ParseContent(String& text);
    String ParseI18NContent(String& text);
    void ParseChoice(String& text, String&choice, String& label, unsigned char& rel, unsigned char& func);
    String ParseFunction(String& text);
    void ParseGoto(String& text, String& label, String& function);
    String ParseLabel(String& text);
    float ParseTime(String& text);
protected:
};

} // namespace godot

#endif