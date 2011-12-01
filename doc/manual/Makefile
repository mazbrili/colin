#
# Makefile for building the Colin manual
#
# Use
#
#   apt-get install texlive-full
#
# for a complete latex environment.
#

PDFLATEXFLAGS=--interaction=nonstopmode --shell-escape

all: colin.pdf

foo:

debversion.tex: foo
	/usr/share/pba-cbs/sh/get_deb_version.sh > debversion.tmp
	if cmp -s debversion.tmp $@; then rm debversion.tmp; else mv debversion.tmp $@; fi

debchange.tex: ../../debian/changelog dch2tex.awk
	awk -f dch2tex.awk $< > $@

PICS = 	./pictures/280px-CreativeCommond_logo_trademark.png \
	./pictures/64px-Cc-by_new.png \
	./pictures/64px-Cc-sa_white.png \
	./pictures/calctoolbar.png \
	./pictures/clazzes2.jpg \
	./pictures/clazzes.jpg \
	./pictures/clipboardtoolbar.png \
	./pictures/drawload_overlay.png \
	./pictures/drawload.png \
	./pictures/drawload.svg \
	./pictures/editmenu.png \
	./pictures/editshortcut.png \
	./pictures/filemenu.png \
	./pictures/hinge_opt.png \
	./pictures/load_opt.png \
	./pictures/logo2-icon_128x128.png \
	./pictures/movetoolbar.png \
	./pictures/node_dark_64.png \
	./pictures/qt_ambassador_logo.png \
	./pictures/scaletoolbar.png \
	./pictures/selectionrect.png \
	./pictures/selecttoolbar.png \
	./pictures/settings_colors.png \
	./pictures/settings_misc.png \
	./pictures/settings_shortcuts.png \
	./pictures/snap_opt.png \
	./pictures/startpage.png \
	./pictures/support_opt.png \
	./pictures/tabbar.png \
	./pictures/temp_wid.png \
	./pictures/title.png \
	./pictures/toolbar.png \
	./pictures/undoredotoolbar.png \
	./pictures/windowtoolbar2.png \
	./pictures/windowtoolbar.png \
	./pictures/workspace_overlay.png \
	./pictures/workspace.png \
	./pictures/workspace.svg \
	./pictures/zoom_opt.png


colin.pdf: colin.tex debversion.tex debchange.tex script.tex\
	$(PICS)	
	i=1; while (pdflatex $(PDFLATEXFLAGS) $< || echo "******* Warning: pdflatex pass $$i failed.") && grep -q 'Rerun' colin.log; do i=`expr $$i + 1`; echo "******* Recompiling (pass $$i)..."; done; echo "******* Finished after $$i pdflatex passes."


clean:
	rm -f colin.pdf colin.toc colin.log colin.aux colin.out debchange.tex debversion.tex
