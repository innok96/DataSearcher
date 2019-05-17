#ifndef PREDICATE_CREATOR_DIALOG_H
#define PREDICATE_CREATOR_DIALOG_H

#include <QDialog>
#include <fstream>


const QString g_predicatesDirName = "predicates";
const QString g_parserDirName = "parser";

namespace Ui {
class PredicateCreatorDialog;
}

class PredicateCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PredicateCreatorDialog(QWidget *parent = nullptr);
    ~PredicateCreatorDialog() override;

private:
    void runParser();

private slots:
    void on_chooseFileButton_clicked();
    void accept() override;

private:
    Ui::PredicateCreatorDialog *ui;
    std::fstream m_contextsFile;
};

#endif // PREDICATE_CREATOR_DIALOG_H
