# Shell script to
# launch the Operating System (OS)
# found in the floppy disk image file
# using the Qemu virtual machine

qemu-system-i386 -m 128 -soundhw pcspk -fda ROS.img

