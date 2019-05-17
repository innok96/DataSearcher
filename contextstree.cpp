#include "contextstree.h"
#include <iostream>
#include <string>
#include <QTextStream>


ContextsTree::ContextsTree()
{
    m_aVertices.push_back(ContextsTreeNode());
}

void ContextsTree::deserialize(QFile& from)
{
    if(!from.isOpen() || !from.isReadable())
    {
        std::cout << "Not opened or readable\n";
        return;
    }

    QTextStream fromStream(&from);
    if(fromStream.atEnd())
    {
        std::cout << "No content inside\n";
        return;
    }

    fromStream >> m_nVertices;
    m_aVertices.resize(m_nVertices);

    for(unsigned i = 0; i < m_nVertices - 1 ; i++)
    {
        unsigned u = 0, v = 0;
        fromStream >> u >> v;
        QChar sym;
        fromStream >> sym;
        m_aVertices[u].m_next[sym] = v;
    }

    for(unsigned i = 0; i < m_nVertices; i++)
    {
        int isLeaf = 0;
        fromStream >> isLeaf;
        m_aVertices[i].m_isLeaf = isLeaf ? true : false;
        if(m_aVertices[i].m_isLeaf)
        {
            QString mark;
            fromStream >> mark;
            m_aVertices[i].m_mark = mark;
        }
    }
}

void ContextsTree::serialize(QFile& to)
{
    if(!to.isOpen() || !to.isWritable())
    {
        std::cout << "Not opened or writable\n";
        return;
    }

    QTextStream toStream(&to);
    toStream << m_nVertices << "\n";

    for(unsigned u = 0; u < m_nVertices; u++)
    {
        for(const auto& v : m_aVertices[u].m_next)
        {
            toStream << u << ' ' << v.second << ' ' << QString(v.first) << "\n";
        }
    }

    for (unsigned i = 0; i < m_nVertices; i++)
    {
        toStream << m_aVertices[i].m_isLeaf << ' ';
        if(m_aVertices[i].m_isLeaf)
            toStream << m_aVertices[i].m_mark;
        toStream << "\n";
    }
}

bool ContextsTree::findOrCreate(const QString& sentence)
{
    unsigned pos = 0;
    for(const QChar& sym : sentence)
    {
        if(!m_aVertices[pos].m_next.count(sym))
        {
            m_aVertices[pos].m_next[sym] = m_nVertices++;
            m_aVertices.push_back(ContextsTreeNode());
        }
        pos = m_aVertices[pos].m_next[sym];
    }

    return m_aVertices[pos].m_isLeaf;
}

void ContextsTree::setMark(const QString& sentence, const QString& mark)
{
    unsigned pos = 0;
    for(const QChar& sym : sentence)
    {
        if(!m_aVertices[pos].m_next.count(sym))
        {
            return;
        }
        pos = m_aVertices[pos].m_next[sym];
    }

    m_aVertices[pos].m_isLeaf = true;
    m_aVertices[pos].m_mark = mark;
}
