#ifndef UNDOCOMMANDER_H
#define UNDOCOMMANDER_H

#include <QtCore/QObject>
#include <QtGui/QUndoStack>

#include "undo_commands.h"

class UndoCommander : public QObject
{
    Q_OBJECT
public:
    UndoCommander(QObject *parent=0) : QObject(parent){t=0; stack=0;}
    UndoCommander(wgv_tw *tw, QUndoStack *us, QObject *parent =0) : QObject(parent){t=0; stack=us; setTw(tw);}
    void setTw(wgv_tw *tw);
    void setStack(QUndoStack *us);

public slots:
    void save_edited(const int &id, const wgv_node &after);
    void save_appended(const wgv_node &n);
    void save_removed(const int &id, const wgv_node &r);

    void save_edited(const int &id, const wgv_beam &after);
    void save_appended(const wgv_beam &n);
    void save_removed(const int &id, const wgv_beam &r);

    void save_edited(const int &id, const wgv_load &after);
    void save_appended(const wgv_load &n);
    void save_removed(const int &id, const wgv_load &r);



private:
    wgv_tw *t;
    QUndoStack *stack;

};

#endif // UNDOCOMMANDER_H
