#include "dialogue_loader.h"

#include <File.hpp>
#include <GodotGlobal.hpp>
#include <cctype>

using namespace godot;

DialogueLoader* DialogueLoader::_instance = nullptr;

void DialogueLoader::_register_methods() {
}

DialogueLoader* DialogueLoader::GetInstance() {
    if (_instance == nullptr) {
        _instance = new DialogueLoader{};
    }

    return _instance;
}

DialogueData* DialogueLoader::LoadDialogue(String filename) {
    File* fp = File::_new();
    fp->open(filename, File::ModeFlags::READ);

    if (!fp->is_open()) {
        return nullptr;
    }

    String content = fp->get_as_text();
    fp->close();

    DialogueData* dialogue = Parse(content);
    return dialogue;
}

DialogueData* DialogueLoader::Parse(String content) {
    DialogueData* dialogue = new DialogueData{};
    PoolStringArray lines = content.split("\n", false);

    int len = lines.size();
    for (int i = 0; i < len; ++i) {
        DialogueData::DialogueLine dl;
        String line = lines[i];

        if (line.length() < 2) {
            Godot::print_error("Error in: " + line, "DialogueLoader::Parse", __FILE__, __LINE__);
            break;
        }

        if (line[0] == '#') {
            String script = ParseScript(line);
            dialogue->SetScriptNode(script);
        }
        else if (line[0] == '[') {
            dl.type = DialogueData::CommandType::CHARACTER;
            dl.content.push_back(ParseCharacter(line));
            dialogue->Push(dl);
        }
        else if (line[0] == '"') {
            dl.type = DialogueData::CommandType::SHOW;
            dl.content.push_back(ParseContent(line));
            dl.time = ParseTime(line);
            dialogue->Push(dl);
        }
        else if (line[0] == '@' && line[1] == '"') {
            dl.type = DialogueData::CommandType::SHOW;
            dl.content.push_back(ParseI18NContent(line));
            dl.relative = true;
            dl.time = ParseTime(line);
            dialogue->Push(dl);
        }
        else if (line.begins_with(">>")) {
            int index = 0;
            do {
                String choice = "", next = "";
                unsigned char relative = 0; unsigned char runtime = 0;
                ParseChoice(line, choice, next, relative, runtime);
                if (choice.empty()) {
                    Godot::print_error("Error in: " + line, "DialogueLoader::Parse", __FILE__, __LINE__);
                    break;
                }

                dl.content.push_back(choice);
                dl.jump.push_back(next);
                dl.relative |= (relative << index); dl.runtime |= (runtime << index);
                ++index; ++i;
                if (i < len) {
                    line = lines[i];
                }
                else {
                    line = "";
                }

            } while (line.begins_with(">>"));
            --i;

            dl.type = DialogueData::CommandType::SELECT;
            dialogue->Push(dl);
        }
        else if (line.ends_with(":")) {
            dialogue->AddMapping(ParseLabel(line));
        }
        else if (line.find("goto ") != -1) {
            String label, func;
            ParseGoto(line, label, func);
            if (label.empty()) {
                dl.jump.push_back(func);
                dl.relative = 1;
            }
            else {
                dl.jump.push_back(label);
            }

            dl.type = DialogueData::CommandType::GOTO;
            dialogue->Push(dl);
        }
        else if (line.find("()") != -1) {
            dl.content.push_back(ParseFunction(line));
            dl.type = DialogueData::CommandType::EXECUTE;
            dialogue->Push(dl);
        }
        else if (line.find("quit") != -1) {
            dl.type = DialogueData::CommandType::QUIT;
            dialogue->Push(dl);
        }
        else {
            Godot::print_error("Error in: " + line, "DialogueLoader::Parse", __FILE__, __LINE__);
            break;
        }
    }

    return dialogue;
}

String DialogueLoader::ParseScript(String& text) {
    int start = 1;
    while (std::isspace(text[start])) {
        ++start;
    }

    return text.substr(start, text.length() - start);
}

String DialogueLoader::ParseCharacter(String& text) {
    if (!text.ends_with("]")) {
        return "";
    }

    int start = 1, length = text.length() - 2;
    while (std::isspace(text[start])) {
        ++start;
    }

    while (std::isspace(text[start + length - 1])) {
        --length;
    }

    return text.substr(start, length);
}

String DialogueLoader::ParseContent(String& text) {
    String content = "";
    int length = text.length();
    for (int i = 1; i < length; ++i) {
        if (text[i] == '"' && text[i - 1] != '\\') {
            text = text.substr(i + 1, length - i - 1);
            break;
        }

        content += text[i];
    }

    return content;
}

String DialogueLoader::ParseI18NContent(String& text) {
    text = text.substr(1, text.length() - 1);
    return ParseContent(text);
}

void DialogueLoader::ParseChoice(String& text, String&choice, String& label, unsigned char& rel, unsigned char& func) {
    int start = 2;
    while (std::isspace(text[start])) {
        ++start;
    }

    text = text.substr(start, text.length() - start);
    if (text.length() < 2) {
        choice = "";
        return;
    }

    if (text[0] == '"') {
        choice = ParseContent(text);
    }
    else if (text[0] == '@' && text[1] == '"') {
        choice = ParseI18NContent(text);
        rel = 1;
    }

    String function;
    ParseGoto(text, label, function);
    if (label.empty()) {
        label = function;
        func = 1;
    }
}

String DialogueLoader::ParseFunction(String& text) {
    int start = 0, length = text.length() - 2;
    while (std::isspace(text[start])) {
        ++start;
    }

    while (std::isspace(text[start + length - 1])) {
        --length;
    }

    return text.substr(start, length);
}

void DialogueLoader::ParseGoto(String& text, String& label, String& function) {
    text = text.substr(5, text.length() - 5);
    if (text.ends_with("()")) {
        function = ParseFunction(text);
        label = "";
    }
    else {
        label = text;
        function = "";
    }
}

String DialogueLoader::ParseLabel(String& text) {
    return text.substr(0, text.length() - 1);
}

float DialogueLoader::ParseTime(String& text) {
    int start = text.find("as");
    if (start == -1) {
        return -1.0f;
    }

    start += 2;
    text = text.substr(start, text.length() - start);
    return text.to_float();
}