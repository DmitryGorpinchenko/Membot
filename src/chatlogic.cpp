#include "chatlogic.h"
#include "chatbot.h"
#include "graphnode.h"
#include "graphedge.h"

#include <fstream>
#include <iostream>
#include <algorithm>

namespace {

using Token = std::pair<std::string, std::string>;
using Tokenlist = std::vector<Token>;
using Nodes = std::vector<std::unique_ptr<GraphNode>>;

template <typename T>
void AddAllTokensToElement(const std::string &tokenID, const Tokenlist &tokens, T &element)
{
    const auto pred = [&tokenID](const Token &t) { return t.first == tokenID; };

    auto token = std::find_if(tokens.begin(), tokens.end(), pred);
    while (token != tokens.end()) {
        element.AddToken((*token).second);
        token = std::find_if(++token, tokens.end(), pred);
    }
}

Tokenlist ExtractTokens(std::string_view lineStr)
{
    Tokenlist tokens;

    while (!lineStr.empty()) {
        const auto posTokenFront = lineStr.find("<");
        const auto posTokenBack = lineStr.find(">");

        if (posTokenFront == std::string::npos ||
            posTokenBack == std::string::npos) {
            break;
        }

        const auto tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - posTokenFront - 1);
        const auto posTokenInfo = tokenStr.find(":");

        if (posTokenInfo != std::string::npos) {
            const auto tokenType = tokenStr.substr(0, posTokenInfo);
            const auto tokenInfo = tokenStr.substr(posTokenInfo + 1);
            tokens.push_back(std::make_pair(std::string(tokenType), std::string(tokenInfo)));
        }

        lineStr.remove_prefix(posTokenBack + 1);
    }

    return tokens;
}

void ProcessTokens(const Tokenlist &tokens, Nodes &nodes)
{
    auto type = std::find_if(tokens.begin(), tokens.end(), [](const Token &t) { return t.first == "TYPE"; });
    if (type == tokens.end()) {
        return;
    }

    auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const Token &t) { return t.first == "ID"; });
    if (idToken == tokens.end()) {
        std::cout << "Error: ID missing. Line is ignored!" << std::endl;
        return;
    }

    const int id = std::stoi((*idToken).second);

    if ((*type).second == "NODE") {
        auto newNode = std::find_if(nodes.begin(), nodes.end(), [&id](const auto &node) { return node->GetID() == id; });

        if (newNode == nodes.end()) {
            auto node = std::make_unique<GraphNode>(id);
            AddAllTokensToElement("ANSWER", tokens, *node);
            nodes.push_back(std::move(node));
        }

        return;
    }

    if ((*type).second == "EDGE") {
        auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const Token &t) { return t.first == "PARENT"; });
        auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const Token &t) { return t.first == "CHILD"; });

        if (parentToken != tokens.end() && childToken != tokens.end()) {
            auto parentNode = std::find_if(nodes.begin(), nodes.end(), [&parentToken](const auto &node) { return node->GetID() == std::stoi((*parentToken).second); });
            auto childNode = std::find_if(nodes.begin(), nodes.end(), [&childToken](const auto &node) { return node->GetID() == std::stoi((*childToken).second); });

            auto edge = std::make_unique<GraphEdge>(id, (*childNode).get(), (*parentNode).get());
            AddAllTokensToElement("KEYWORD", tokens, *edge);

            (*childNode)->AddEdgeToParentNode(edge.get());
            (*parentNode)->AddEdgeToChildNode(std::move(edge));
        }

        return;
    }
}

Nodes AnswerGraph(const std::string &filename)
{
    Nodes nodes;

    std::ifstream file(filename);
    std::string lineStr;
    while (std::getline(file, lineStr)) {
        const Tokenlist tokens = ExtractTokens(lineStr);
        ProcessTokens(tokens, nodes);
    }

    return nodes;
}

} // end namespace

ChatLogic::ChatLogic(const std::string &answer_graph_file, ChatBotPanelDialog *panelDialog)
    : _nodes(AnswerGraph(answer_graph_file))
    , _chatBot(nullptr)
    , _panelDialog(panelDialog)
{}

ChatLogic::~ChatLogic() = default;

void ChatLogic::Run()
{
    auto root = std::find_if(_nodes.begin(), _nodes.end(), [](const auto &node) { return node->GetNumberOfParents() == 0; });

    if (root == _nodes.end()) {
        return;
    }

    ChatBot chatBot((*root).get(), this, "../images/chatbot.png");
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(const std::string &message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(const std::string &message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot() const
{
    return _chatBot->GetImageHandle();
}
