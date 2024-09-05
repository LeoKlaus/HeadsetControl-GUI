#ifndef DIALOGINFO_H
#define DIALOGINFO_H

#include <QDialog>

namespace Ui {
class dialogInfo;
}

class DialogInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInfo(QWidget *parent = nullptr);
    ~DialogInfo();

    void setTitle(const QString &title);
    void setLabel(const QString &text);

private:
    Ui::dialogInfo *ui;
};

#endif // DIALOGINFO_H
