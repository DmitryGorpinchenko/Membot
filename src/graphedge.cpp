#include "graphedge.h"

GraphEdge::GraphEdge(int id, GraphNode *childNode, GraphNode *parentNode)
    : _id(id)
    , _childNode(childNode)
    , _parentNode(parentNode)
{}

void GraphEdge::AddToken(const std::string &token)
{
    _keywords.push_back(token);
}
