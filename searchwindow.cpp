#include "searchwindow.h"
#include "ui_searchwindow.h"
#include "predicate_creator_dialog.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    m_pManager = new QNetworkAccessManager(this);

    m_predicatesDir = QDir::current();
    if(!m_predicatesDir.exists(g_predicatesDirName))
    {
        QMessageBox::warning(this, "Ошибка", "Отсутствуют предикаты.");
        return;
    }
    m_predicatesDir.cd(g_predicatesDirName);

    QStringList lDirs = m_predicatesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(const auto& dirName : lDirs)
    {
        QListWidgetItem* pListWidget = new QListWidgetItem(ui->listWidget);
        pListWidget->setText(dirName);
        m_aListWidgets.push_back(pListWidget);
    }
}

SearchWindow::~SearchWindow()
{
    delete ui;
    for(QListWidgetItem* pListWidget : m_aListWidgets)
        delete pListWidget;
}

void SearchWindow::on_searchButton_clicked()
{
    QString subject = ui->subjectLine->text();
    if(subject.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Субъект не должен быть пустым.");
        return;
    }

    QString dirName;
    for(const auto& listItem : m_aListWidgets)
    {
        if(listItem->isSelected())
        {
            dirName = listItem->text();
            break;
        }
    }
    QFile syntaxTreesFile;
    m_predicatesDir.cd(dirName);
    syntaxTreesFile.setFileName(m_predicatesDir.filePath("syntax_trees.txt"));
    m_predicatesDir.cdUp();

    bool opened = syntaxTreesFile.open(QFile::ReadOnly | QFile::Text | QFile::ExistingOnly);
    if(!opened)
    {
        QMessageBox::warning(this, "Ошибка", syntaxTreesFile.errorString());
        return;
    }
    if(syntaxTreesFile.atEnd())
    {
        QMessageBox::warning(this, "Ошибка", "Не найдено синтаксических деревьев.");
        return;
    }

    QTextStream stream(&syntaxTreesFile);
    unsigned numTrees = 0;
    stream >> numTrees;
    m_aSyntaxTrees.resize(numTrees);
    for(auto& syntaxTree : m_aSyntaxTrees)
    {
        syntaxTree.deserialize(stream);
    }
    syntaxTreesFile.close();
}
