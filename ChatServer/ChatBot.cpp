#include <iostream>
#include <string>
#include <algorithm>
#include <regex>
#include <map>
#include "ChatBot.h"
using namespace std;

void to_lower(string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

string bot(string text)
{
    cout << "[BOT]: " << text << endl;
    return text;
}

string user(string question, string name)
{
    cout << "[USER-" << name << "]:" << question << endl;
    to_lower(question);
    return question;
}

string startBot(string text, string name)
{
    string question;

    map<string, string> database = {
        {"hello", "Oh, hello to you living substance"},
        {"what is your name", "My name is Skill Bot 3042"},
        {"how are you", "I'm fine"},
        {"what are you doing", "I'm answering stupid question"},
        {"fuck you", "oh shit! OK, fucking bag of bones"},
    };

    question = user(text, name);
    bool isAnswerFound = false;
    for (auto line : database)
    {
        regex pattern = regex(".*" + line.first + ".*");
        if (regex_match(question, pattern))
            return bot(line.second);
    }
    return bot("Stupid human i don't understand you");
}

