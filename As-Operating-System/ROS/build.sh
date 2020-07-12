# Shell script to
# build the Operating System (OS)
# as a floppy disk image file

echo ""

echo "Creating operating system floppy image ..."
	rm -f ROS.img
	mkdosfs -F 12 -C ROS.img 1440 || exit
echo "done."
echo ""


echo "Assembling bootloader and kernelloader programs ..."
cd bootloader
	./nasm bootloader.asm -f bin -o bootloader.bin || exit
	./nasm KRNLDR.ASM -f bin -o KRNLDR.SYS || exit
cd ..
echo "done."
echo ""


echo "Adding bootloader to floppy image ..."
	dd status=noxfer conv=notrunc if=bootloader/bootloader.bin of=ROS.img || exit
echo "done."
echo ""


echo "Copying all files ..."
	rm -rf tmp-loop
	mkdir tmp-loop
	sudo mount -o loop -t vfat ROS.img tmp-loop || exit

	echo " - copying assembled kernelloader file ..."
		sudo cp bootloader/KRNLDR.SYS tmp-loop || exit
	echo "   done."

	echo " - copying extra files ..."
		sudo cp files/* tmp-loop || exit
	echo "   done."

	sleep 0.2
	echo "Unmounting loopback floppy ..."
	sudo umount tmp-loop || exit
	rm -rf tmp-loop
echo "done."
echo ""

