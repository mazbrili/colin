#ifndef COLINMANUAL_H
#define COLINMANUAL_H

#include <QtCore/QString>

class colinManual
{
public:
	static colinManual& instance()
	{
		if(!instance_)
			instance_ = new colinManual();
		return *instance_;
	}

	static void launch(const QString &what);

	static QString pageForUrl(const QString &url);

private:
	colinManual(){};
	static colinManual *instance_;
};

#endif // COLINMANUAL_H
