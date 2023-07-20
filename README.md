# preload 1.0.1
Preload for Linux

# Intro
````
preload is an adaptive readahead daemon that prefetches files mapped by
applications from the disk to reduce application startup time.  preload should
run on any recent Linux system with /proc pseudo-filesystem mounted.

preload runs as a daemon and gathers information about processes running on
the system and shared-objects that they use.  This information is saved in a
file to keep across runs of preload.
````
# Install
````
preload is built and installed using the common ./configure; make; make install
See the file named INSTALL for more details on how to control the directories
that vairous files are installed.

If checked out from CVS, running ./bootstrap is needed before configuring.
To build an RPM package using the distributed spec file, run make rpm.
````
# Running
````
The easiest way to run preload is by using the provided initscript.  If you
use the RPM package, you should be fine, otherwise you may need to manually
add the service and enable it, by running commands like:

  /sbin/chkconfig --add preload

Let preload run for a couple of boots before expecting it to predict that
much.  Technically, you need to run any application at least two times
before preload starts predicting it.
````

# Thanks
````
The Fedora Project
Chris DiBona and the Google Summer of Code Program team
Ziga Mahkovec  <ziga.mahkovec@klika.si>
Soeren Sandmann <sandmann@daimi.au.dk>
Arjan van de Ven  <arjanv@redhat.com>
bert hubert <bert.hubert@netherlabs.nl>
Elliot Lee <sopwith@redhat.com>
Christopher W. Brown <muslimsoap@gmail.com>
rainmanp7
````
# Authors
````
behdad: Behdad Esfahbod

rainmanp7: Christopher W Brown
````
# Important Notice
````
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
````
