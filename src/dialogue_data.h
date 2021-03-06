#ifndef __DIALOGUE_DATA_H__
#define __DIALOGUE_DATA_H__

#include <Godot.hpp>
#include <Object.hpp>
#include <Node.hpp>
#include <vector>
#include <Dictionary.hpp>
#include <cstring>
#include <string>

namespace godot {

class DialogueBox;

class DialogueData : public Object {
    GODOT_CLASS(DialogueData, Object);
public:
    enum class CommandType {
        SHOW = 0,
        CHARACTER = 1,
        SELECT = 2,
        QUIT = 3,
        GOTO = 4,
        EXECUTE = 5
    };

    struct DialogueLine {
        CommandType type;
        float time;
        PoolStringArray content;
        PoolStringArray jump;
        unsigned char relative;
        unsigned char runtime;

        DialogueLine() : time(-1), relative(0), runtime(0) {}
    };

    DialogueData() : _current(0), _db(nullptr) {
    }

    static void _register_methods();

    DialogueLine& Next(int choice);

    inline void Push(const DialogueLine& line) {
        _lines.push_back(line);
    }

    inline void SetScriptNode(NodePath script) {
        _script_node = script;
    }

    inline void AddMapping(String key) {
        _mapping[key] = _lines.size();
    }

    inline NodePath GetScriptNode() const {
        return _script_node;
    }

    inline void SetDialogueBoxRef(DialogueBox* db) {
        _db = db;
    }

    inline bool NeedToSelect() const {
        return _lines[_current].type == CommandType::SELECT;
    }

    void free();
private:
    std::vector<DialogueLine> _lines;
    Dictionary _mapping;

    int _current;

    NodePath _script_node;
    DialogueBox* _db;

    int ProcessSelect(int choice);
    void ProcessCharacter();
    int ProcessGoto();
    void ProcessExecute();

    String Execute(String name);
protected:  
};

} // namespace godot

#endif // __DIALOGUE_DATA_H__