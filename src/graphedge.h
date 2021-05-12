#ifndef GRAPHEDGE_H_
#define GRAPHEDGE_H_

#include <vector>
#include <string>

class GraphNode;

class GraphEdge {
public:
    GraphEdge(int id, GraphNode *childNode, GraphNode *parentNode);

    int GetID() const { return _id; }
    GraphNode *GetChildNode() const { return _childNode; }
    const std::vector<std::string> &GetKeywords() const { return _keywords; }

    void AddToken(const std::string &token);

private:
    int _id;
    GraphNode *_childNode;
    GraphNode *_parentNode;

    std::vector<std::string> _keywords;
};

#endif /* GRAPHEDGE_H_ */
