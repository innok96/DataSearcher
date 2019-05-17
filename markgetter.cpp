#include "markgetter.h"
#include "ui_markgetter.h"
#include <QMessageBox>

QString g_sentenceMark;

MarkGetter::MarkGetter(pugi::xml_node tokensNode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarkGetter)
{
    ui->setupUi(this);

    int yCoord = 5, numWords = 0;
    for(const auto& tokenNode : tokensNode.children(L"token"))
    {
        QCheckBox* pCheckBox = new QCheckBox(ui->scrollAreaWidgetContents);
        QString word = QString::fromStdWString(tokenNode.child(L"word").text().as_string());
        pCheckBox->setText(word);
        pCheckBox->setGeometry(5, yCoord, 250, 15);
        m_apCheckBoxes.push_back(pCheckBox);

        yCoord += 20;
        numWords++;
    }
    ui->scrollAreaWidgetContents->setMinimumSize(250, numWords * 20 + 10);

    g_sentenceMark.clear();
    g_sentenceMark.fill('N', numWords);
}

MarkGetter::~MarkGetter()
{
    delete ui;
    for(QCheckBox* pCheckBox : m_apCheckBoxes)
        delete pCheckBox;
}

void MarkGetter::accept()
{
    if(!g_sentenceMark.count('S'))
    {
        QMessageBox::warning(this, "Ошибка", "Необходимо отметить субъект");
        return;
    }
    if(!g_sentenceMark.count('O'))
    {
        QMessageBox::warning(this, "Ошибка", "Необходимо отметить объект");
        return;
    }

    done(QDialog::Accepted);
}

void MarkGetter::on_subjectButton_clicked()
{
    for(unsigned ind = 0; ind < m_apCheckBoxes.size(); ind++)
    {
        if(m_apCheckBoxes[ind]->isChecked())
        {
            g_sentenceMark[ind] = 'S';
            m_apCheckBoxes[ind]->setChecked(false);
        }
    }
}

void MarkGetter::on_objectButton_clicked()
{
    for(unsigned ind = 0; ind < m_apCheckBoxes.size(); ind++)
    {
        if(m_apCheckBoxes[ind]->isChecked())
        {
            g_sentenceMark[ind] = 'O';
            m_apCheckBoxes[ind]->setChecked(false);
        }
    }
}
