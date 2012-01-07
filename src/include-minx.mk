
prefix=/usr/$(arch)

CXX=$(arch)-g++
CXXFLAGS=$(CXXOPTFLAGS) -DQT_DLL -DQT_FATAL_ASSERT

PKGCONFIG=$(arch)-pkg-config
WINDRES=$(arch)-windres

# do not delete the space after the '='!!!
EXTPKGS= 

MOC=/usr/bin/$(arch)-moc-qt4

INCLUDES=-I$(srcdir) -I/usr/$(arch)/include/qt4 -I/usr/$(arch)/include

LIBS=-lqtmain -lQtCore4 -lQtGui4 -lQtScript4 -lQtSvg4 -lintl
LDFLAGS=-Wl,-subsystem,windows -L/usr/$(arch)/lib/qt4 $(LDOPTFLAGS)

include $(srcdir)/include.mk
