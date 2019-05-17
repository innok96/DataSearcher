#ifndef CONTEXTSTREE_H
#define CONTEXTSTREE_H

#include <map>
#include <vector>
#include <QString>
#include <fstream>
#include <QFile>

struct ContextsTreeNode
{
    std::map<QChar, unsigned> m_next;
    bool m_isLeaf = false;
    QString m_mark;
};

class ContextsTree
{
public:
    ContextsTree();

    void deserialize(QFile& from);
    void serialize(QFile& to);
    bool findOrCreate(const QString& sentence);
    void setMark(const QString& sentence, const QString& mark);

private:
    unsigned m_nVertices = 1;
    std::vector<ContextsTreeNode> m_aVertices;
};

#endif // CONTEXTSTREE_H
