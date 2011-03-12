#ifndef COLINPUSHBUTTONPART_H
#define COLINPUSHBUTTONPART_H

#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStyle>
#include <QtGui/QPainter>


class ColinPushButtonPart : public QPushButton
{
    Q_OBJECT

public:

    enum Position {
        Alone   = 0,        //00000000
        Left    = 1,        //00000001
        Right   = 2,        //00000010
        Up      = 4,        //00000100
        Bottom  = 8,        //00001000
        HCenter = 3,        //00000011
        VCenter = 12,       //00001100
        Center  = 15        //00001111
    } pos;

    Q_DECLARE_FLAGS(Positions, Position);

    explicit ColinPushButtonPart(QWidget *parent = 0):QPushButton(parent){pos = ColinPushButtonPart::Alone;}
    ColinPushButtonPart(const QString &text, QWidget *parent = 0):QPushButton(text, parent){pos = ColinPushButtonPart::Alone;}
    ColinPushButtonPart(const QIcon &icon, const QString &text, QWidget *parent = 0):QPushButton(icon, text, parent){pos = ColinPushButtonPart::Alone;}

    void paintEvent(QPaintEvent *);

    void setCutted(ColinPushButtonPart::Position pos_, bool free = true);



};

#endif // COLINPUSHBUTTONPART_H
