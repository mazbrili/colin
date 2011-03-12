#ifndef EDITPROFILEDIALOG_H
#define EDITPROFILEDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

class editProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit editProfileDialog(const int &i, QWidget *parent = 0);

private slots:
    void ok();
    void cancel();
signals:

public slots:
private:
    QLineEdit *NLine,
              *ALine,
              *ILine,
              *hLine;
    int nr;

};

#endif // EDITPROFILEDIALOG_H
