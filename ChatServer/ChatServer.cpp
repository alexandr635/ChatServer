#include <iostream>
#include <uwebsockets/App.h>
#include <regex>
#include <map>
#include "ChatBot.h"

using namespace std;

struct UserConnection
{
    string name;
    unsigned long user_id;
};

void newUserConnected(string channel, uWS::WebSocket<false, true>* ws, string name, unsigned long user_id) {
    ws->publish(channel, "NEW_USER=" + to_string(user_id) + "," + name);
}

int main()
{
    int port = 8888;
    unsigned long latest_user_id = 10;
    map<unsigned long, string> online_users;
    unsigned long count_users = 0;

    uWS::App().ws<UserConnection>("/*", {
        .open = [&latest_user_id, &online_users, &count_users](auto* ws) {
            UserConnection* data = (UserConnection*)ws->getUserData();
            data->name = "UNNAMED";
            data->user_id = latest_user_id++;
            cout << "New user connected ID = " << data->user_id << endl;

            //Task 1
            count_users++;
            cout << "Total users connected: " << count_users << endl;

            ws->subscribe("broadcast");
            string user_channel("user#" + to_string(data->user_id));
            ws->subscribe(user_channel);

            for (auto entry : online_users) {
                newUserConnected(user_channel, ws, entry.second, entry.first);
            }

            online_users[data->user_id] = data->name;
        },

        .message = [&online_users, &latest_user_id](auto* ws, string_view message, uWS::OpCode opCode) {
            string SET_NAME("SET_NAME=");
            string MESSAGE_TO("MESSAGE_TO=");

            UserConnection* data = (UserConnection*)ws->getUserData();
            cout << "New message received = " << message << endl;
            if (message.find("SET_NAME=") == 0) {
                string try_name = string(message.substr(SET_NAME.length()));

                //Task 2, 3
                if (try_name.length() < 256 && try_name.find(",") > 256)
                {
                    cout << "User sets their name" << endl;
                    data->name = message.substr(SET_NAME.length());
                    newUserConnected("broadcast", ws, data->name, data->user_id);
                    online_users[data->user_id] = data->name;
                    
                }
                else
                    cout << "User tries to set incorrect name" << endl;
            }
            if (message.find("MESSAGE_TO=") == 0) {
                auto rest = string(message.substr(MESSAGE_TO.length()));
                int comma_position = rest.find(",");
                auto ID = string(rest.substr(0, comma_position));
                auto text = data->name + ": " + rest.substr(comma_position + 1);

                //Task 4
                if (atoi(ID.c_str()) < latest_user_id && atoi(ID.c_str()) > 9) {
                    cout << "User sends private message" << endl;
                    ws->publish("user#" + string(ID), text);
                }

                //Task 5
                else if (atoi(ID.c_str()) == 1)
                {
                    cout << data->user_id << endl;
                    ws->send("[BOT]: " + startBot(string(text), data->name), uWS::OpCode::TEXT, false);
                }
                else
                {
                    cout << "Error, there is no user with ID = " << ID;
                    ws->send("Error, there is no user with ID = " + ID, uWS::OpCode::TEXT, false);
                }
            }
        },
        .close = [&online_users, &count_users](auto* ws, int code, std::string_view message) {
            UserConnection* data = (UserConnection*)ws->getUserData();
            ws->publish("broadcast", "DISCONNECT=" + to_string(data->user_id));
            online_users.erase(data->user_id);
            count_users--;
        }
        }).listen(port, [port](auto* token) {
            if (token) {
                cout << "Server started successfully on port " << port << endl;
            }
            else {
                cout << "Server failed to start" << endl;
            }
            }).run();
}

