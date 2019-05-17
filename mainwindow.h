#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include "predicate_creator_dialog.h"
#include "searchwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addPredButton_clicked();

    void on_searchButton_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<PredicateCreatorDialog> m_pPredCreateDialog;
    std::shared_ptr<SearchWindow> m_pSearchWindow;
};

#endif // MAINWINDOW_H
