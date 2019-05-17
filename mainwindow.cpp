#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addPredButton_clicked()
{
    m_pPredCreateDialog = std::make_shared<PredicateCreatorDialog>(this);
    m_pPredCreateDialog->exec();
}

void MainWindow::on_searchButton_clicked()
{
    m_pSearchWindow = std::make_shared<SearchWindow>(this);
    m_pSearchWindow->show();
}
