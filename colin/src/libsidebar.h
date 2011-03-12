#ifndef LIBSIDEBAR_H
#define LIBSIDEBAR_H

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QSplitter>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "filelist.h"

class libSidebar : public QSplitter
{
    Q_OBJECT
public:
    explicit libSidebar(QWidget *parent = 0);

signals:

public slots:
    void appendMat();
    void appendProfile();
    void removeMat(const int &o);
    void removeProfile(const int &o);
    void actualUnits();
    void setProfile(const int &o);
    void setMat(const int &o);

private slots:
    void launchDialogNewP();
    void launchDialogNewM();
    void launchDialogEditP();
    void launchDialogEditM();
    void launchDialogEditP(const QModelIndex &mI);
    void launchDialogEditM(const QModelIndex &mI);
    void activatedP(const QModelIndex &mI);
    void activatedM(const QModelIndex &mI);
    void setActiveP(const int &i);
    void setActiveM(const int &i);

private:
    QGridLayout *layout;
#ifdef LIB_ICON
    QLabel *proficon,
           *maticon;
#endif
    QLabel *profvals,
           *profname,
           *matname,
           *matvals;

    QPushButton *newM,
                *newP,
                *editM,
                *editP;

    QTreeWidget *mats,
                *profiles;
    int oldCurrentM,
        oldCurrentP;
};

#endif // LIBSIDEBAR_H
