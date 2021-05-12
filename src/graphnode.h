#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include "chatbot.h"

#include <vector>
#include <string>
#include <memory>

class GraphEdge;

class GraphNode {
public:
    GraphNode(int id);
    ~GraphNode();

    int GetID() const { return _id; }
    size_t GetNumberOfParents() const { return _parentEdges.size(); }
    size_t GetNumberOfChildEdges() const { return _childEdges.size(); }
    GraphEdge *GetChildEdgeAtIndex(size_t index) const { return _childEdges[index].get(); }
    const std::vector<std::string> &GetAnswers() const { return _answers; }

    void AddToken(const std::string &token);
    void AddEdgeToParentNode(GraphEdge *edge);
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> &&edge);

    void MoveChatbotHere(ChatBot &&chatbot);

private:
    int _id;
    std::vector<std::unique_ptr<GraphEdge>> _childEdges;
    std::vector<GraphEdge *> _parentEdges;
    ChatBot _chatBot;

    std::vector<std::string> _answers;
};

#endif /* GRAPHNODE_H_ */
