#ifndef __DIALOGUE_DATA_H__
#define __DIALOGUE_DATA_H__

#include <Godot.hpp>
#include <Resource.hpp>
#include <Node.hpp>
#include <vector>

namespace godot {

class DialogueData : public Resource {
    GODOT_CLASS(DialogueData, Resource);
    //OBJ_CATEGORY("Resources");
public:
    struct DialogueLine {
        String character;
        String content;
        String label;
        String choices[4];
        String choice_labels[4];
    };

    DialogueData() : _current(0), _script_node(nullptr) {
    }

    const DialogueLine& Next(int choice = -1);

    void Push(const DialogueLine& line, int index);
    
    void Push(const DialogueLine& line, String function);

    void Push(const DialogueLine& line, int choices[4]);

    inline void SetScriptNode(Node* script) {
        _script_node = script;
    }
private:
    struct NextLine {
        int index;
        int choices[4];
        String function;

        NextLine() : index(0), function("") {}
        ~NextLine(){}
    };

    std::vector<DialogueLine> _lines;

    std::vector<NextLine> _next;

    int _current;

    Node* _script_node;
protected:  
};

} // namespace godot

#endif // __DIALOGUE_DATA_H__