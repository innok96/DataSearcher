#include "syntaxtree.h"
#include <unordered_map>

SyntaxTree::SyntaxTree(const pugi::xml_node& syntaxTreeNode, const pugi::xml_node& tokensNode, const QString& mark)
{
    auto aNodes = syntaxTreeNode.children(L"node");
    auto numNodes = std::distance(aNodes.begin(), aNodes.end());
    m_aNodes.resize(static_cast<unsigned>(numNodes));
    for(const auto& node : aNodes)
    {
        bool isRoot = node.attribute(L"is_root").as_bool();
        unsigned index = node.child(L"token").text().as_uint();
        int parent = node.child(L"parent").text().as_int();
        m_aNodes[index].m_parent = static_cast<int>(parent);
        if(parent != -1)
            m_aNodes[static_cast<unsigned>(parent)].m_children.push_back(index);
        m_aNodes[index].m_isRoot = isRoot;
        if(isRoot)
        {
            m_rootNode = static_cast<int>(index);
        }
    }

    for(const auto& token : tokensNode)
    {
        unsigned index = token.child(L"position").text().as_uint();
        auto lemma = QString::fromStdWString(token.child(L"lemma").text().as_string());
        m_aNodes[index].m_lemma = lemma;
    }

    m_mark = mark;
}

SyntaxTree SyntaxTree::getMinimalSubjectObjectSubtree()
{
    if(m_rootNode == -1 || !m_mark.count('S') || !m_mark.count('O'))
        return SyntaxTree();

    m_subjectParents.assign(m_aNodes.size(), false);
    m_objectParents.assign(m_aNodes.size(), false);
    for(int i = 0; i < m_mark.size(); i++)
    {
        if(m_mark[i] == 'S')
            markAsSubjectParent(i);
        else if(m_mark[i] == 'O')
            markAsObjectParent(i);
    }

    m_newRoot = findNewRoot(static_cast<unsigned>(m_rootNode));
    if(m_newRoot == -1)
        return SyntaxTree();
    m_isSubtreeNode.assign(m_aNodes.size(), false);

    for(int i = 0; i < m_mark.size(); i++)
    {
        if(m_mark[i] == 'S' || m_mark[i] == 'O')
            findSubtreeNode(i);
    }

    SyntaxTree retTree;
    std::unordered_map<unsigned, int> mapping;
    int num = 0;
    for(unsigned i = 0; i < m_aNodes.size(); i++)
    {
        if(m_isSubtreeNode[i])
        {
            mapping[i] = num++;
            retTree.m_mark += m_mark[i];
        }
    }

    retTree.m_rootNode = mapping[static_cast<unsigned>(m_newRoot)];
    for(unsigned i = 0; i < m_aNodes.size(); i++)
    {
        if(m_isSubtreeNode[i])
        {
            retTree.m_aNodes.push_back(m_aNodes[i]);
            if(i == static_cast<unsigned>(m_newRoot))
            {
                retTree.m_aNodes.back().m_isRoot = true;
            }
            if(retTree.m_aNodes.back().m_parent != -1)
                retTree.m_aNodes.back().m_parent = mapping[static_cast<unsigned>(retTree.m_aNodes.back().m_parent)];
            retTree.m_aNodes.back().m_children.clear();
            for(const auto& child : m_aNodes[i].m_children)
            {
                if(mapping.count(child))
                    retTree.m_aNodes.back().m_children.push_back(static_cast<unsigned>(mapping[child]));
            }
        }
    }

    return retTree;
}

void SyntaxTree::serialize(QTextStream& stream) const
{
    stream << m_aNodes.size() << ' ' << m_rootNode << ' ' << m_mark << '\n';
    for(const auto& node : m_aNodes)
    {
        stream << node.m_lemma << ' ' << node.m_isRoot << ' ' << node.m_parent << '\n';
        stream << node.m_children.size() << ' ';
        for(auto child : node.m_children)
            stream << child << ' ';
        stream << '\n';
    }
}

void SyntaxTree::deserialize(QTextStream &stream)
{
    unsigned nodesSize = 0;
    stream >> nodesSize >> m_rootNode >> m_mark;
    m_aNodes.resize(nodesSize);
    for(auto& node : m_aNodes)
    {
        int isRoot = 0;
        stream >> node.m_lemma >> isRoot >> node.m_parent;
        node.m_isRoot = isRoot;
        unsigned numChildren = 0;
        stream >> numChildren;
        node.m_children.resize(numChildren);
        for(auto& child : node.m_children)
            stream >> child;
    }
}

void SyntaxTree::markAsSubjectParent(int index)
{
    if(index == -1)
        return;

    m_subjectParents[static_cast<unsigned>(index)] = true;
    markAsSubjectParent(m_aNodes[static_cast<unsigned>(index)].m_parent);
}

void SyntaxTree::markAsObjectParent(int index)
{
    if(index == -1)
        return;

    m_objectParents[static_cast<unsigned>(index)] = true;
    markAsObjectParent(m_aNodes[static_cast<unsigned>(index)].m_parent);
}

int SyntaxTree::findNewRoot(unsigned index)
{
    if(m_subjectParents[index] && m_objectParents[index])
        return static_cast<int>(index);

    for(const auto& child : m_aNodes[index].m_children)
    {
        int newRoot = findNewRoot(child);
        if(newRoot != -1)
            return newRoot;
    }

    return -1;
}

void SyntaxTree::findSubtreeNode(int index)
{
    m_isSubtreeNode[static_cast<unsigned>(index)] = true;
    if(index != m_newRoot)
        findSubtreeNode(m_aNodes[static_cast<unsigned>(index)].m_parent);
}
