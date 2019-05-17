#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QListWidgetItem>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QTextCodec>
#include <vector>
#include "contextstree.h"
#include "syntaxtree.h"

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchWindow(QWidget *parent = nullptr);
    ~SearchWindow();

private slots:
    void on_searchButton_clicked();

private:
    Ui::SearchWindow *ui;
    std::vector<QListWidgetItem*> m_aListWidgets;
    ContextsTree m_contextsTree;
    QDir m_predicatesDir;
    std::vector<SyntaxTree> m_aSyntaxTrees;
    QNetworkAccessManager* m_pManager = nullptr;
};

#endif // SEARCHWINDOW_H
