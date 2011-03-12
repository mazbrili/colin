#ifndef CLOSEDIALOG_H
#define CLOSEDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>

class closeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit closeDialog(QWidget *parent = 0);

signals:

public slots:
    void oke();
    void esc();
    void saveAll();
    void closewS();

private:
    QString getFileName();
    QList<QCheckBox*> checker;


};

#endif // CLOSEDIALOG_H
