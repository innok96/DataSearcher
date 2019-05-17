#ifndef MARKGETTER_H
#define MARKGETTER_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include "pugixml.hpp"

extern QString g_sentenceMark;

namespace Ui {
class MarkGetter;
}

class MarkGetter : public QDialog
{
    Q_OBJECT

public:
    explicit MarkGetter(pugi::xml_node tokensNode, QWidget *parent = nullptr);
    ~MarkGetter() override;

private slots:
    void accept() override;
    void on_subjectButton_clicked();
    void on_objectButton_clicked();

private:
    Ui::MarkGetter *ui;
    std::vector<QCheckBox*> m_apCheckBoxes;
};

#endif // MARKGETTER_H
