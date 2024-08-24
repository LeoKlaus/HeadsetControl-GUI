#ifndef DIALOGINFO_H
#define DIALOGINFO_H

#include <QDialog>

namespace Ui {
class dialogInfo;
}

class dialogInfo : public QDialog
{
    Q_OBJECT

public:
    explicit dialogInfo(QWidget *parent = nullptr);
    ~dialogInfo();

    void setTitle(const QString& title);

    void setLabel(const QString& text);

private:
    Ui::dialogInfo *ui;
};

#endif // DIALOGINFO_H
