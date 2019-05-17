#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <QString>
#include <vector>
#include <QTextStream>
#include <pugixml.hpp>

struct SyntaxTreeNode
{
    QString m_lemma;
    int m_parent = -1;
    bool m_isRoot = false;
    std::vector<unsigned> m_children;
};

class SyntaxTree
{
public:
    SyntaxTree() {}
    SyntaxTree(const pugi::xml_node& syntaxTreeNode, const pugi::xml_node& tokensNode, const QString& mark);

    SyntaxTree getMinimalSubjectObjectSubtree();
    void serialize(QTextStream& stream) const;
    void deserialize(QTextStream& stream);

private:
    void markAsSubjectParent(int index);
    void markAsObjectParent(int index);
    int findNewRoot(unsigned index);
    void findSubtreeNode(int index);

private:
    std::vector<SyntaxTreeNode> m_aNodes;
    int m_rootNode = -1, m_newRoot = -1;
    QString m_mark;
    std::vector<bool> m_subjectParents, m_objectParents, m_isSubtreeNode;
};

#endif // SYNTAXTREE_H
