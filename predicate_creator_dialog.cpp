#include "predicate_creator_dialog.h"
#include "ui_predicate_creator_dialog.h"
#include "contextstree.h"
#include "syntaxtree.h"
#include "pugixml.hpp"
#include "markgetter.h"
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>
#include <QTextStream>

PredicateCreatorDialog::PredicateCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PredicateCreatorDialog)
{
    ui->setupUi(this);
}

PredicateCreatorDialog::~PredicateCreatorDialog()
{
    delete ui;
}

void PredicateCreatorDialog::runParser()
{
    QProcess* pProcess = new QProcess(this);
    QString parser("parser\\parser.exe");
    QStringList arguments{"-lemmatizer", "0", "-lang", "russian", "-emit_morph", "0", "-render_tree", "1", "-i",
                          "parser/contexts.txt", "-o", "parser/syntax_trees.xml", "-d", "parser/dictionary.xml",
                          "-eol", "-parser", "0"};
    pProcess->start(parser, arguments);
    pProcess->waitForFinished(-1);
}

void PredicateCreatorDialog::on_chooseFileButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл с контекстами.");
    ui->fileNameLine->setText(path);
}

void PredicateCreatorDialog::accept()
{
    QString predName = ui->predNameLine->text();
    if(predName.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Имя предиката не должно быть пустым.");
        return;
    }

    QFile contexts;
    contexts.setFileName(ui->fileNameLine->text());
    contexts.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!contexts.isReadable())
    {
        QMessageBox::warning(this, "Ошибка", "Невозможно прочитать файл.");
        return;
    }

    auto predicatesDir = QDir::current(), parserDir = predicatesDir;
    if(!parserDir.exists(g_parserDirName))
    {
        QMessageBox::warning(this, "Ошибка", "Отсутствует парсер.");
        return;
    }
    parserDir.cd(g_parserDirName);

    if(!predicatesDir.exists(g_predicatesDirName))
    {
        predicatesDir.mkdir(g_predicatesDirName);
    }
    predicatesDir.cd(g_predicatesDirName);

    if(predicatesDir.exists(predName))
    {
        auto answer = QMessageBox::question(this, "Существующий предикат", "Данный предикат уже существует. Вы хотите перезаписать его?");
        if(answer == QMessageBox::No)
            return;

        predicatesDir.cd(predName);
        predicatesDir.removeRecursively();
        predicatesDir.cdUp();
    }

    if(!predicatesDir.mkdir(predName))
    {
        QMessageBox::warning(this, "Ошибка", "Невозможно создать папку, введите корректное имя.");
        return;
    }
    predicatesDir.cd(predName);

    m_contextsFile.open(parserDir.filePath("contexts.txt").toStdString(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    m_contextsFile.clear();

    QFile contextsTreeFile;
    contextsTreeFile.setFileName(predicatesDir.filePath("contexts_tree.txt"));
    contextsTreeFile.open(QFile::ReadOnly | QFile::Text | QFile::NewOnly);

    QFile syntaxTreesFile;
    syntaxTreesFile.setFileName(predicatesDir.filePath("syntax_trees.txt"));
    syntaxTreesFile.open(QFile::ReadWrite | QFile::Text | QFile::NewOnly);
    QTextStream syntaxTreesFileStream(&syntaxTreesFile);

    ContextsTree contextsTree;
    contextsTree.deserialize(contextsTreeFile);
    contextsTreeFile.close();

    QTextStream in(&contexts);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        m_contextsFile << line.toStdString() << std::endl;
    }
    m_contextsFile.close();

    runParser();

    pugi::xml_document syntaxTreesDoc;
    syntaxTreesDoc.load_file(parserDir.filePath("syntax_trees.xml").toStdString().c_str());

    std::vector<SyntaxTree> aSyntaxTrees;
    pugi::xml_node parsingNode = syntaxTreesDoc.child(L"parsing");
    for(const auto& sentenceNode : parsingNode.children(L"sentence"))
    {
        auto tokensNode = sentenceNode.child(L"tokens");
        auto syntaxTreeNode = sentenceNode.child(L"syntax_tree");
        auto sentence = QString::fromStdWString(sentenceNode.child(L"text").text().as_string());
        bool hasCode = contextsTree.findOrCreate(sentence);
        if(!hasCode)
        {
            MarkGetter markGetter(tokensNode, this);
            auto res = markGetter.exec();
            if(res != QDialog::Accepted)
                continue;

            contextsTree.setMark(sentence, g_sentenceMark);
        }
        SyntaxTree syntaxTree(syntaxTreeNode, tokensNode, g_sentenceMark);
        SyntaxTree subTree = syntaxTree.getMinimalSubjectObjectSubtree();
        aSyntaxTrees.push_back(subTree);
    }
    syntaxTreesFileStream << aSyntaxTrees.size() << '\n';
    for(const auto& syntaxTree : aSyntaxTrees)
    {
        syntaxTree.serialize(syntaxTreesFileStream);
    }
    syntaxTreesFile.close();
    contextsTreeFile.open(QFile::WriteOnly | QFile::Text);
    contextsTree.serialize(contextsTreeFile);
    contextsTreeFile.close();

    done(QDialog::Accepted);
}
