#include "dialoginfo.h"
#include "ui_dialoginfo.h"

DialogInfo::DialogInfo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialogInfo)
{
    setModal(true);
    ui->setupUi(this);
}

DialogInfo::~DialogInfo()
{
    delete ui;
}

void DialogInfo::setTitle(const QString &title)
{
    this->setWindowTitle(title);
}

void DialogInfo::setLabel(const QString &text)
{
    ui->label->setText(text);
}
