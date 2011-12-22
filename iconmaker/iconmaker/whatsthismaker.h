#ifndef WHATSTHISMAKER_H
#define WHATSTHISMAKER_H

#include "icons.h"

class whatsThisMaker : public iconMaker
{
public:
	whatsThisMaker();
	void paint(QPainter *p, bool light, bool shadow);
};

#endif // WHATSTHISMAKER_H
