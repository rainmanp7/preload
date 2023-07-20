##                                               -*- Automake -*-
## Process this file with automake to produce Makefile.in
##############################################################################

SUBDIRS = doc src

EXTRA_DIST = bootstrap
CLEANFILES = 
MAINTAINERCLEANFILES = \
		$(srcdir)/INSTALL \
		$(srcdir)/aclocal.m4 \
		$(srcdir)/autoscan.log \
		$(srcdir)/compile \
		$(srcdir)/config.guess \
		$(srcdir)/config.h.in \
		$(srcdir)/config.sub \
		$(srcdir)/configure.scan \
		$(srcdir)/depcomp \
		$(srcdir)/install-sh \
		$(srcdir)/ltmain.sh \
		$(srcdir)/missing \
		$(srcdir)/mkinstalldirs \
		`find "$(srcdir)" -type f -name Makefile.in -print` \
		`find "$(srcdir)" -type f -name "*~" -print` \
		`$(srcdir)/bootstrap --write-configure`
# The last line above is a horrible hack.
# GNU Coding Standards recommends that `make maintainer-clean' should not 
# remove the configure script.
# We instead make configure call bootstrap.

# Indent all C source and header files, using indent(1):

indent: $(srcdir)/.indent.pro
	cd "$(srcdir)" && \
	find . -mindepth 2 -type f -name "*.[ch]" -print | \
	  xargs ./missing --run indent

.PHONY: indent

# Creating logrotate and init-script, etc

EXTRA_DIST += preload.logrotate.in preload.init.in preload.sysconfig

initd_SCRIPTS = preload.init
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT = $(initd_SCRIPTS)
sysconfig_DATA = preload.sysconfig
logrotate_DATA = preload.logrotate
log_DATA = preload.log
pkglocalstate_DATA = preload.state

CLEANFILES += preload.init preload.logrotate preload.state preload.log

do_subst = sed \
		-e 's,@datadir\@,$(pkgdatadir),g' \
		-e 's,@sbindir\@,$(sbindir),g' \
		-e 's,@initddir\@,$(initddir),g' \
		-e 's,@sysconfigdir\@,$(sysconfigdir),g' \
		-e 's,@logrotatedir\@,$(logrotatedir),g' \
		-e 's,@logdir\@,$(logdir),g' \
		-e 's,@subsysdir\@,$(subsysdir),g' \
		-e 's,@PACKAGE_NAME\@,$(PACKAGE_NAME),g' \
		-e 's,@PACKAGE_SUMMARY\@,$(PACKAGE_SUMMARY),g' \
		-e 's,@PACKAGE\@,$(PACKAGE),g' \
		-e 's,@VERSION\@,$(VERSION),g'

preload.init: preload.init.in Makefile
	$(do_subst) $(srcdir)/$< > $@
	chmod +x $@

preload.logrotate: preload.logrotate.in Makefile
	$(do_subst) $(srcdir)/$< > $@

preload.state preload.log:
	touch $@

install-data-hook:
	@cd "$(DESTDIR)$(initddir)" && mv -f preload.init preload
	@cd "$(DESTDIR)$(logrotatedir)" && mv -f preload.logrotate preload
	@cd "$(DESTDIR)$(sysconfigdir)" && mv -f preload.sysconfig preload

uninstall-hook:
	$(RM) "$(DESTDIR)$(initddir)/preload"
	$(RM) "$(DESTDIR)$(logrotatedir)/preload"
	$(RM) "$(DESTDIR)$(sysconfigdir)/preload"




# Some custom targets to make it easier to release things.
# Use either:
#		make distcheck
# or		make release-publish

tar_file = $(distdir).tar.gz
sha1_file = $(tar_file).sha1
gpg_file = $(sha1_file).asc

$(sha1_file): $(tar_file)
	sha1sum $^ > $@

$(gpg_file): $(sha1_file)
	@echo "Please enter your GPG password to sign the checksum."
	gpg --armor --sign $^ 

release-publish: distcheck $(tar_file) $(sha1_file) $(gpg_file)
	@echo ""
	@echo "Please send an announcement to $(MAILING_LIST)"
	@echo "including the following:"
	@echo ""
	@echo "Subject: $(PACKAGE) release $(VERSION) now available"
	@echo ""
	@echo "============================== CUT HERE =============================="
	@echo "A new $(PACKAGE) release $(VERSION) is now available from:"
	@echo ""
	@echo "	$(DOWNLOAD_URL_BASE)/$(tar_file)"
	@echo ""
	@echo "    which can be verified with:"
	@echo ""
	@echo "	$(DOWNLOAD_URL_BASE)/$(sha1_file)"
	@echo -n "	"
	@cat $(sha1_file)
	@echo ""
	@echo "	$(DOWNLOAD_URL_BASE)/$(gpg_file)"
	@echo "	(signed by `getent passwd "$$USER" | cut -d: -f 5 | cut -d, -f 1`)"
	@echo ""
	@echo "You can access these from:"
	@echo ""
	@echo "	$(SITE_URL)"
	@echo "============================== CUT HERE =============================="
	@echo ""
	@echo "Also, please include the new entries from the NEWS file."
	@echo "Needless to say, upload the four files to the project"
	@echo "download server and push the NEWS entries to the news"
	@echo "section of the site."

.PHONY: release-verify-even-micro release-check release-publish
