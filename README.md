# Janitor_scm OS

A really crappy OS, made by a Janitor.

## How to Run the code
make
qemu-system-i386 -drive format=raw,file=C:\Users\minot\OneDrive\Desktop\assembly\output\os.img

## good sources:
https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/extensible-host-controler-interface-usb-xhci.pdf

## TODO
- use long A format to get file names and print them out with ls
- use long A format in cd to check for directories and implement a correct !strcmp
- fix that it every file and not every 2nd (not long A format)
- implement touch