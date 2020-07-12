# Shell script to
# write the Operating System (OS)
# found in the floppy disk image file
# inside a USB Flash Drive

echo "Writing floppy image to USB flash drive ..."
	sudo dd if=ROS.img of=/dev/sdb
echo "done."
echo ""

