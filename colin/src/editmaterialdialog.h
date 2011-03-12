#ifndef EDITMATERIALDIALOG_H
#define EDITMATERIALDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

class editMaterialDialog : public QDialog
{
    Q_OBJECT
public:
    explicit editMaterialDialog(const int &i, QWidget *parent = 0);

private slots:
    void ok();
    void cancel();
private:
    QLineEdit *NLine,
              *ELine,
              *aLine;
    int nr;

};

#endif // EDITMATERIALDIALOG_H
