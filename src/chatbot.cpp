#include "chatbot.h"
#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <limits>

namespace {

int ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    const size_t m = s1.size();
    const size_t n = s2.size();

    if (m == 0) {
        return n;
    }
    if (n == 0) {
        return m;
    }

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; ++k) {
        costs[k] = k;
    }

    size_t i = 0;
    for (auto it1 = s1.cbegin(); it1 != s1.cend(); ++it1, ++i) {
        costs[0] = i + 1;

        size_t j = 0, corner = i;
        for (auto it2 = s2.cbegin(); it2 != s2.cend(); ++it2, ++j) {
            const size_t upper = costs[j + 1];

            if (*it1 == *it2) {
                costs[j + 1] = corner;
            } else {
                const size_t t = (upper < corner) ? upper : corner;
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    const int result = costs[n];
    delete[] costs;

    return result;
}

} // end namespace

ChatBot::ChatBot()
    : _currentNode(nullptr)
    , _rootNode(nullptr)
    , _chatLogic(nullptr)
{}

ChatBot::ChatBot(GraphNode *rootNode, ChatLogic *chatLogic, const std::string &avatar)
    : _currentNode(nullptr)
    , _rootNode(rootNode)
    , _chatLogic(chatLogic)
    , _avatar_img(new wxBitmap(avatar, wxBITMAP_TYPE_PNG))
{
    std::cout << "ChatBot Constructor" << std::endl;
    
    Sink(_rootNode);
}

ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor" << std::endl;
}

ChatBot::ChatBot(const ChatBot &rhs)
    : _avatar_img(new wxBitmap(*rhs._avatar_img))
    , _currentNode(rhs._currentNode)
    , _rootNode(rhs._rootNode)
    , _chatLogic(rhs._chatLogic)
{
    std::cout << "ChatBot Copy Constructor" << std::endl;
}

ChatBot::ChatBot(ChatBot &&rhs)
    : ChatBot()
{
    std::cout << "ChatBot Move Constructor" << std::endl;

    Swap(rhs);
}

ChatBot &ChatBot::operator =(const ChatBot &rhs)
{
    std::cout << "ChatBot Copy Assignment Operator" << std::endl;

    if (&rhs != this) {
        ChatBot tmp(rhs);
        Swap(tmp);
    }
    return *this;
}

ChatBot &ChatBot::operator =(ChatBot &&rhs)
{
    std::cout << "ChatBot Move Assignment Operator" << std::endl;

    if (&rhs != this) {
        ChatBot tmp(std::move(rhs));
        Swap(tmp);
    }
    return *this;
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    Emerge(node);
}

void ChatBot::ReceiveMessageFromUser(const std::string &message)
{
    Sink(NextNode(message));
}

void ChatBot::Sink(GraphNode *node)
{
    node->MoveChatbotHere(std::move(*this));
}

void ChatBot::Emerge(GraphNode *node)
{
    _currentNode = node;

    const auto &answers = _currentNode->GetAnswers();
    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);

    _chatLogic->SetChatbotHandle(this);
    _chatLogic->SendMessageToUser(answers[dis(generator)]);
}

void ChatBot::Swap(ChatBot &rhs)
{
    std::swap(_avatar_img, rhs._avatar_img);
    std::swap(_currentNode, rhs._currentNode);
    std::swap(_rootNode, rhs._rootNode);
    std::swap(_chatLogic, rhs._chatLogic);
}

GraphNode *ChatBot::NextNode(const std::string &message) const
{
    GraphEdge *newEdge = nullptr;
    int minDist = std::numeric_limits<int>::max();

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i) {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);

        for (const auto &keyword : edge->GetKeywords()) {
            const int dist = ComputeLevenshteinDistance(keyword, message);

            if (dist < minDist) {
                minDist = dist;
                newEdge = edge;
            }
        }
    }

    return newEdge ? newEdge->GetChildNode() : _rootNode;
}

