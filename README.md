# Janitor_scm OS

A really crappy OS, made by a Janitor.

## How to Run the code
make
qemu-system-i386 -drive format=raw,file=C:\Users\minot\OneDrive\Desktop\assembly\output\os.img
qemu-system-i386 -drive format=raw,file=C:\Users\minec\Desktop\JanOS\output\os.img

## good sources:
https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/extensible-host-controler-interface-usb-xhci.pdf

## TODO
- implement cat
- implement touch

## time bombs
- cd does shinanigans to calculate the right cluster
- every command cuts at space
- terminal only takes the line you are in
- cant delete characters after reaching next line
- I was only able to read 2 bytes at the same time so the whole code now bases on that
- folders with spaces cant be accessed.
- extension in non-longname entries are ignored