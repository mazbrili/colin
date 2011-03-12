#ifndef NEWMATERIALDIALOG_H
#define NEWMATERIALDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

class newMaterialDialog : public QDialog
{
    Q_OBJECT
public:
    explicit newMaterialDialog(QWidget *parent = 0);

private slots:
    void ok();
    void cancel();
private:
    QLineEdit *NLine,
              *ELine,
              *aLine;

};

#endif // NEWMATERIALDIALOG_H
