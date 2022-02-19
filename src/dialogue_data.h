#ifndef __DIALOGUE_DATA_H__
#define __DIALOGUE_DATA_H__

#include <Godot.hpp>
#include <Resource.hpp>
#include <Node.hpp>
#include <vector>
#include <Dictionary.hpp>
#include <cstring>
#include <string>

namespace godot {

class DialogueData : public Resource {
    GODOT_CLASS(DialogueData, Resource);
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

    DialogueData() : _current(0) {
    }

    static void _register_methods();

    const DialogueLine& Next(int choice = -1);

    inline void Push(const DialogueLine& line) {
        _lines.push_back(line);
    }

    inline void SetScriptNode(String script) {
        _script_node = script;
    }

    inline void AddMapping(String key) {
        _mapping[key] = _lines.size();
    }

    inline String GetScriptNode() const {
        return _script_node;
    }
private:
    std::vector<DialogueLine> _lines;
    Dictionary _mapping;

    int _current;

    String _script_node;
protected:  
};

} // namespace godot

#endif // __DIALOGUE_DATA_H__