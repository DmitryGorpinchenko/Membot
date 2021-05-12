#include "graphnode.h"
#include "graphedge.h"

GraphNode::GraphNode(int id)
    : _id(id)
{}

GraphNode::~GraphNode() = default;

void GraphNode::AddToken(const std::string &token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> &&edge)
{
    _childEdges.push_back(std::move(edge));
}

void GraphNode::MoveChatbotHere(ChatBot &&chatbot)
{
    _chatBot = std::move(chatbot);
    _chatBot.SetCurrentNode(this);
}
