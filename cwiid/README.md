CWiid Wiimote Interface
=======================

QUICKSTART
----------
1. sudo apt-get install bison flex libgtk2.0-dev autotools-dev automake libbluetooth-dev
2. cd cwiid
3. aclocal
4. autoconf
5. ./configure --with-cwiid-config-dir=/etc/cwiid/
6. make
7. sudo make install
8. sudo modprobe uinput
9. wminput -c <key_bind_file>

You can find key bind files at /etc/cwiid/wminput/

DESCRIPTION
-----------
The CWiid package contains the following parts:

1. libcwiid - wiimote API.
2. cwiid module - python interface to libcwiid
3. wmgui - GTK gui to the wiimote.
4. wminput - an event/joystick/mouse driver for the wiimote.
5. lswm - list wiimote devices (in the spirit of ls{,pci,usb}, etc.
6. wmdemo - a minimal demonstration of the libwiimote API. (not installed)

REQUIREMENTS
------------
awk, bison, flex, bluez-libs, gtk+-2 dev libs, python 2.4 or greater, python dev for python module, uinput kernel support, kernel sources

INSTALLATION
------------
If installing from git, run the following commands.  Otherwise, or if you aren't sure, ignore them.

- aclocal
- autoconf

All users should then run:

- ./configure
- make
- sudo make install

To install global configuration files to the more familiar /etc rather than /usr/etc or /usr/local/etc, execute configure with --sysconfdir=/etc.

While CWiid is still in the early development stages, installation directories and filenames are changed more often than in a mature, stable package.  In order to prevent the collection of obselete directories and files, it is recommended that you run 'make uninstall && make uninstall_config' from your current CWiid source directory before installing a new one.  Files are currently installed to the following directories: /usr/local/{bin,etc,lib,lib/cwiid,lib/python2.X/site-packages,share/doc,share/man}.  Recently, but no longer, used directories include /usr/share/CWiid - this directory may be deleted.

Many distributions do not have /usr/local/lib in the library search path, and on many of these same distributions, the library installation directory (as determined by autoconf) is /usr/local/lib, creating a problem when you try to run programs depending on libraries installed there. There are two clean ways to fix this:

1. Add a --prefix=/usr argument to ./configure
2. Add /usr/local/lib to /etc/ld.so.conf, then run ldconfig.

I prefer the second method since it keeps local software in the local directory where it should be, and it fixes the problem for every package using /usr/local/lib, not just CWiid.  Until someone offers a convincing argument for the omission of /usr/local/lib from /etc/ld.so.conf, I consider this to be a distribution bug, and method #2 above is the fix.

Please report any compilation errors at http://cwiid.org.

EXECUTION
---------
wmgui [-h] [bdaddr]
wminput [-h] [-c config] [bdaddr]

The bluetooth device address (bdaddr) of the wiimote can be specified on the command-line, or through the WIIMOTE_BDADDR environment variable, in that order of precedence.  If neither is given, the first wiimote found by hci_inquiry will be used.
See wminput/README for more information on wminput configuration and execution.
