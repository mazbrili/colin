#
# Build nsis installers.
#

srcdir=../src

VERSION=$(shell /usr/share/pba-cbs/sh/get_deb_version.sh $(srcdir))

INSTALLERS=colin-$(VERSION)-setup-x86.exe colin-$(VERSION)-setup-x64.exe
NSIFILES=$(addsuffix .nsi,$(basename $(INSTALLERS)))
DESTDIR=$(shell pwd)/install-root

MINGW32BUILDDIR=../Mingw32
MINGW64BUILDDIR=../Mingw64

all: $(INSTALLERS)

%.exe: %.nsi
	makensis $<

#%.rcc: $(srcdir)/%.qrc
#	rcc -binary $< -o $@

clean-mingw32:
	make -C $(MINGW32BUILDDIR) clean

build-mingw32:
	make -C $(MINGW32BUILDDIR)

install-mingw32: build-mingw32
	make -C $(MINGW32BUILDDIR) DESTDIR=$(DESTDIR) install

build-mingw64:
	make -C $(MINGW64BUILDDIR)

install-mingw64: build-mingw64
	make -C $(MINGW64BUILDDIR) DESTDIR=$(DESTDIR) install

colin-$(VERSION)-setup-x86.nsi: $(srcdir)/colin-setup.nsi install-mingw32
	sed -e 's/__VERSION__/$(VERSION)/g' -e 's/__SRCDIR__/..\/src/g' -e 's/__ARCHITECTURE__/x86/g' -e 's/__ARCHTRIPLET__/i686-pc-mingw32/g' -e 's/__PROGRAMFILES__/PROGRAMFILES/g'  -e 's/__REGVIEW__/32/g' < $< > $@

colin-$(VERSION)-setup-x64.nsi: $(srcdir)/colin-setup.nsi install-mingw64
	sed -e 's/__VERSION__/$(VERSION)/g' -e 's/__SRCDIR__/..\/src/g' -e 's/__ARCHITECTURE__/x64/g' -e 's/__ARCHTRIPLET__/x86_64-pc-mingw32/g' -e 's/__PROGRAMFILES__/PROGRAMFILES64/g'  -e 's/__REGVIEW__/64/g' < $< > $@

clean:
	make -C $(MINGW32BUILDDIR) clean
	make -C $(MINGW64BUILDDIR) clean
	rm -rf ./install-root
	#rm -rf colin.rcc
	rm -rf $(NSIFILES) $(INSTALLERS)
