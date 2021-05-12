#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>

#include <string>
#include <memory>

class GraphNode;
class ChatLogic;

class ChatBot {
public:
    ChatBot();
    ChatBot(GraphNode *rootNode, ChatLogic *chatLogic, const std::string &avatar);
    ~ChatBot();

    ChatBot(const ChatBot &rhs);
    ChatBot(ChatBot &&rhs);
    ChatBot &operator =(const ChatBot &rhs);
    ChatBot &operator =(ChatBot &&rhs);

    wxBitmap *GetImageHandle() const { return _avatar_img.get(); }

    void SetCurrentNode(GraphNode *node);
    void ReceiveMessageFromUser(const std::string &message);

private:
    void Sink(GraphNode *node);
    void Emerge(GraphNode *node);

    void Swap(ChatBot &rhs);

    GraphNode *NextNode(const std::string &message) const;

    GraphNode *_currentNode;
    GraphNode *_rootNode;
    ChatLogic *_chatLogic;

    std::unique_ptr<wxBitmap> _avatar_img;
};

#endif /* CHATBOT_H_ */
