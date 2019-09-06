#
# Boiler plate makefile
#
# Needs to be included in a toplevel makefiles, which defines
# the following variables:
#
# srcdir
# prefix
# CXX
# CXXFLAGS
# LDFLAGS
# PKGCONFIG
# WINDRES
#

ARTIFACT=Colin
ABI_VERSION=1

ifeq ($(WINDRES),)
EXTPKGS=QtGui
endif

ifeq ($(INCLUDES),)
INCLUDES=-I$(srcdir)
endif

ifeq ($(MOC),)
MOC=moc-qt4
endif



LD=$(CXX)

MOCHEADERS= abstractcolintooltip.h \
    beammenu.h \
    bmenu.h \
    closedialog.h \
    cmenu.h \
    colinhmultibutton.h \
    colinicons.h \
    colinmenu.h \
    colinpushbuttonpart.h \
    colintooltip.h \
    colorsettingswidget.h \
    ctabbar.h \
    ctoolbar.h \
    cwidget.h \
    dloadmenu.h \
    drawselectionwidget.h \
    editmaterialdialog.h \
    editprofiledialog.h \
    filelist.h \
    grid.h \
    jmenu.h \
    libsidebar.h \
    lmenu.h \
    loadmenu.h \
    mainwindow.h \
    miscsettingswidget.h \
    momentmenu.h \
    newmaterialdialog.h \
    newprofiledialog.h \
    nloadmenu.h \
    nodemenu.h \
    nwidget.h \
    previewrenderer.h \
    previewwidget.h \
    resultvisualwidget.h \
    scalewidget.h \
    shortcutsettings.h \
    shortcutsettingswidget.h \
    structpainter.h \
    structprinter.h \
    swidget.h \
    tempmenu.h \
    termovaluewidget.h \
    toolTipEater.h \
    treedelegate.h \
    treeview.h \
    undocommandmaker.h \
    unitsettings.h \
    viewmenu.h \
    viewmenu2.h \
    viewport.h \
    viewportpresenter.h \
    viewportsettings.h \
    viewporttooltip.h \
    wgv.h \
    wgv_abstract_tw.h \
    wgv_lib.h \
    wgv_load.h \
    wgv_tw.h \
    zmenu.h


BUILDDATESOURCES=colinversion.cpp

SOURCES= beammenu.cpp \
    bmenu.cpp \
    catcher.cpp \
    closedialog.cpp \
    cmenu.cpp \
    colin.cpp \
    colinhmultibutton.cpp \
    colinicons.cpp \
    colinmenu.cpp \
    colinpushbuttonpart.cpp \
    colintooltip.cpp \
    colorsettingswidget.cpp \
    ctabbar.cpp \
    cwidget.cpp \
    dloadmenu.cpp \
    drawselectionwidget.cpp \
    editmaterialdialog.cpp \
    editprofiledialog.cpp \
    filelist.cpp \
    grid.cpp \
    jmenu.cpp \
    libsidebar.cpp \
    lmenu.cpp \
    loadmenu.cpp \
    mainwindow.cpp \
    miscsettingswidget.cpp \
    momentmenu.cpp \
    newmaterialdialog.cpp \
    newprofiledialog.cpp \
    nloadmenu.cpp \
    nodemenu.cpp \
    nwidget.cpp \
    previewrenderer.cpp \
    previewwidget.cpp \
    resultvisualwidget.cpp \
    scalewidget.cpp \
    shortcutsettings.cpp \
    shortcutsettingswidget.cpp \
    structpainter.cpp \
    structprinter.cpp \
    swidget.cpp \
    tempmenu.cpp \
    termovaluewidget.cpp \
    treedelegate.cpp \
    treeview.cpp \
    undo_commands.cpp \
    undocommandmaker.cpp \
    unitsettings.cpp \
    viewmenu.cpp \
    viewmenu2.cpp \
    viewport.cpp \
    viewportpresenter.cpp \
    viewportsettings.cpp \
    viewporttooltip.cpp \
    wgv.cpp \
    wgv_beam.cpp \
    wgv_bearing.cpp \
    wgv_lib.cpp \
    wgv_load.cpp \
    wgv_node.cpp \
    wgv_tw.cpp \
    xmlreader.cpp \
    xmlwriter.cpp \
    zmenu.cpp



include /usr/share/pba-cbs/mk/main-executable.mk

LUPDATE=lupdate-qt4
LRELEASE=lupdate-qt4

$(srcdir)/colin_%.qm: $(srcdir)/colin_%.ts
	( cd $(srcdir) ; $(LUPDATE) colin.pro ; $(LRELEASE) colin.pro ) 

install-bin::
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/colin/icons
	( cd $(srcdir)/../icons ; install -m 0755 *.png $(DESTDIR)$(prefix)/share/colin/icons )
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/colin/icons/mouse
	( cd $(srcdir)/../icons/mouse ; install -m 0755 *.png $(DESTDIR)$(prefix)/share/colin/icons/mouse )
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/colin/icons/tooltip
	( cd $(srcdir)/../icons/tooltip ; install -m 0755 *.png $(DESTDIR)$(prefix)/share/colin/icons/tooltip )
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/colin/translations
	( cd $(srcdir) ; install -m 0755 colin_*.qm $(DESTDIR)$(prefix)/share/colin/translations )
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/applications
	( cd $(srcdir)/../xdg ; install -m 0755 colin.desktop $(DESTDIR)$(prefix)/share/applications )
	install -m 0755 -v -d $(DESTDIR)$(prefix)/share/colin/pixmaps
	( cd $(srcdir)/../xdg ; install -m 0755 colin.png colin.xpm $(DESTDIR)$(prefix)/share/colin/pixmaps )

