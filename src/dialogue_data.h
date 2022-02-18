#ifndef __DIALOGUE_DATA_H__
#define __DIALOGUE_DATA_H__

#include <Godot.hpp>
#include <Resource.hpp>
#include <Node.hpp>
#include <vector>
#include <unordered_map>
#include <cstring>

namespace godot {

class DialogueData : public Resource {
    GODOT_CLASS(DialogueData, Resource);
public:
    enum class CommandType {
        SHOW,
        CHARACTER,
        SELECT,
        QUIT,
        GOTO,
        EXECUTE
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

    void Push(const DialogueLine& line);

    inline void SetScriptNode(String script) {
        _script_node = script;
    }

    inline void AddMapping(String key) {
        _mapping[key] = _lines.size();
    }
private:
    std::vector<DialogueLine> _lines;
    std::unordered_map<String, int> _mapping;

    int _current;

    String _script_node;
protected:  
};

} // namespace godot

#endif // __DIALOGUE_DATA_H__