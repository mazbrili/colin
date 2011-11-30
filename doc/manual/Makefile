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

colin.pdf: colin.tex debversion.tex debchange.tex\
	./pictures/64px-Cc-by_new.png ./pictures/64px-Cc-sa_white.png ./pictures/280px-CreativeCommond_logo_trademark.png
	i=1; while (pdflatex $(PDFLATEXFLAGS) $< || echo "******* Warning: pdflatex pass $$i failed.") && grep -q 'Rerun' colin.log; do i=`expr $$i + 1`; echo "******* Recompiling (pass $$i)..."; done; echo "******* Finished after $$i pdflatex passes."


clean:
	rm -f colin.pdf colin.toc colin.log colin.aux colin.out
