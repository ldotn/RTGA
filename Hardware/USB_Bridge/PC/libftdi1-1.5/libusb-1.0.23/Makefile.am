AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = INSTALL_WIN.txt PORTING doc/libusb.png \
	     android msvc Xcode
SUBDIRS = libusb

if BUILD_EXAMPLES
SUBDIRS += examples
endif

if BUILD_TESTS
SUBDIRS += tests
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libusb-1.0.pc

distdir = $(PACKAGE_NAME)-$(VERSION)

# Ensure any generated docs are cleaned out
clean-local:
	rm -rf doc/$(DOXYGEN_HTMLDIR)

# Use dist-hook to accomplish the following things for the dist recipe:
#   1) Remove the GitHub Markdown from the README file
#   2) Remove the .gitattributes file from the msvc directory
dist-hook:
	chmod u+w $(distdir)/README $(distdir)/msvc
	$(SED) -i.orig -e '/Build Status/d' $(distdir)/README
	$(SED) -i.orig -e '/^$$/N;/^\n$$/D' $(distdir)/README
	$(SED) -i.orig -e 's/\[\([A-Z]*\)\](\1)/\1/' $(distdir)/README
	rm -f $(distdir)/README.orig
	rm -f $(distdir)/msvc/.gitattributes

.PHONY: dist-up
dist-up: reldir = .release/$(distdir)
dist-up: dist
	rm -rf $(reldir)
	mkdir -p $(reldir)
	cp $(distdir).tar.bz2 $(reldir)/
	read -p "Enter SourceForge username: " sfuser; \
		rsync -rv $(reldir) $$sfuser@frs.sourceforge.net:/home/frs/project/libusb/libusb-1.0/
	rm -rf $(reldir)
