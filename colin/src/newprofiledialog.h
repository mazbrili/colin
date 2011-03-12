#ifndef NEWPROFILEDIALOG_H
#define NEWPROFILEDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>



class newProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newProfileDialog(QWidget *parent = 0);
    ~newProfileDialog();

private slots:
    void ok();
    void cancel();
private:
    QLineEdit *NLine,
              *ALine,
              *ILine,
              *hLine;


};

#endif // NEWPROFILEDIALOG_H
