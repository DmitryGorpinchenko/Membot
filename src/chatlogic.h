#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include "chatgui.h"

#include <vector>
#include <string>
#include <memory>

class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic {
public:
    ChatLogic(const std::string &answer_graph_file, ChatBotPanelDialog *panelDialog);
    ~ChatLogic();

    void Run();

    void SetChatbotHandle(ChatBot *chatbot);
    void SendMessageToChatbot(const std::string &message);
    void SendMessageToUser(const std::string &message);

    wxBitmap *GetImageFromChatbot() const;

private:
    std::vector<std::unique_ptr<GraphNode>> _nodes;

    ChatBot *_chatBot;
    ChatBotPanelDialog *_panelDialog;
};

#endif /* CHATLOGIC_H_ */
