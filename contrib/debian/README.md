
Debian
====================
This directory contains files used to package nintd/nint-qt
for Debian-based Linux systems. If you compile nintd/nint-qt yourself, there are some useful files here.

## nint: URI support ##


nint-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install nint-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your nintqt binary to `/usr/bin`
and the `../../share/pixmaps/bitcoin128.png` to `/usr/share/pixmaps`

nint-qt.protocol (KDE)

