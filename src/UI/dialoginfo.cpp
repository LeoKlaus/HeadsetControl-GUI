#include "dialoginfo.h"
#include "ui_dialoginfo.h"

dialogInfo::dialogInfo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialogInfo)
{
    setModal(true);
    ui->setupUi(this);
}

dialogInfo::~dialogInfo()
{
    delete ui;
}

void dialogInfo::setTitle(const QString& title)
{
    this->setWindowTitle(title);
}

void dialogInfo::setLabel(const QString& text)
{
    ui->label->setText(text);
}
