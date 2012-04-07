#!/bin/bash

# Diskmaker.sh : generates a disk image
# diskmaker.sh <bootloader> <stage1> <output

if [[ $# -ne 4 ]]; then
	echo -e "Error Bad usage"
	echo -e "Usage : $0 <bootloader> <chainloader> <stage1> <output>\n"
	echo -e "Generates a disk image in <output>, and create a partition table"
    echo -e "table on it, containg a single FAT32 partition. Then writes the"
    echo -e "<bootloader> binary on the Master Boot Record of <output>, and "
    echo -e "<stage1> on the reserved sectors of the FAT 32 partition, using "
    echo -e "<chainloader> on the boot section of the VBR to chain."
	exit 1
fi

# check the running OS
OS=`uname`

if [[ -f $4 ]]; then
	echo -e "Warning : output file $4 exists, override\n"
fi

echo -e "\\033[1;32mCreating the disk image... \\033[0;39m"
echo -e "(Command:\\033[1;33m\tdd if=/dev/zero of=$4 bs=1440 count=8192\\033[0;39m)"

dd if=/dev/zero of=$4 bs=512 count=1440
if [[ $? -ne 0 ]]; then
	echo -e "\\033[1;31mFAILED: Error creating disk image. (Check command output) \\033[0;39m"
	exit 1
fi





echo -e "\\033[1;32mPartitionning disk... \\033[0;39m"

if [[ $OS = "Darwin" ]]; then

	 echo -e "(Command:\\033[1;33m\tfdisk -i -a dos $4 \\033[0;39m)"
	 fdisk -i -a dos $4

     if [[ $? -ne 0 ]]; then
        echo -e "\\033[1;31mFAILED: Error partitionning disk image. (Check command output) \\033[0;39m"
		exit 1
	fi
else

	#     16065
	#  quite dirty but...
	echo " o
     	x
	c
	16065
	r
	n
	p
	1
	1
	1439
     	t
     	c
     	a
     	1
	w
     	" | fdisk disk.img

	if [[ $? -ne 0 ]]; then
        	echo -e "\\033[1;31mFAILED: Error partitionning disk image. (Check command output) \\033[0;39m"
		exit 1
	fi
fi




echo -e "\\033[1;32mCreating FAT filesystem on first partition... \\033[0;39m"

if [[ $OS = "Darwin" ]]; then
	
	echo -e "(Command:\\033[1;33m\thdiutil attach $4\\033[0;39m)"
	device=`hdiutil attach -nomount $4 | head -n 2 | tail -n 1 | cut -d ' ' -f 1`
	
	if [[ $? -ne 0 ]]; then
    	echo -e "\\033[1;31mFAILED: Error mounting disk image (Check command output) \\033[0;39m"
        exit 1
	fi

	echo -e "(Command:\\033[1;33m\tnewfs_msdos -F 12 -O \"QUASAR 0\" -S 512 -c 1 -r 32 $device\\033[0;39m)"
	newfs_msdos -F 12 -O "QUASAR 0" -S 512 -c 1 -r 32 $device
	
	if [[ $? -ne 0 ]]; then
    	echo -e "\\033[1;31mFAILED: Error creating FS (Check command output) \\033[0;39m"
        exit 1
	fi

	hdiutil detach $device

else

	# This is _VERY_ dirty... one should find a cleaner way
	offset=63
        offset=$(($offset*512))

	losetup -o $offset /dev/loop0 $4

	if [[ $? -ne 0 ]]; then
    	echo -e "\\033[1;31mFAILED: Error mounting disk image (Check command output) \\033[0;39m"
        exit 1
	fi

	# not sure if -n is really the equivalent of Mac OS's -O switch...
	mkdosfs -F 12 -n "QUASAR 0" -S 512 -s 1 -R 32 /dev/loop0

	if [[ $? -ne 0 ]]; then
    	echo -e "\\033[1;31mFAILED: Error creating FS (Check command output) \\033[0;39m"
        exit 1
	fi

	losetup -d /dev/loop0
fi





if [[ !( -f $1) ]]; then
	echo -e "\\033[1;31mFAILED: $1: No such file \\033[0;39m"
	exit 1
fi

if [[ !( -f $2) ]]; then
	echo -e "\\033[1;31mFAILED: $2: No such file \\033[0;39m"
	exit 1
fi

if [[ !( -f $3) ]]; then
	echo -e "\\033[1;31mFAILED: $3: No such file \\033[0;39m"
	exit 1
fi


echo -e "\\033[1;32mWriting binaries on disk... \\033[0;39m"
echo -e "(Command:\\033[1;33m\t./writeloader/writeloader $1 $2 $3 $4\\033[0;39m)"

./writeloader/writeloader $1 $2 $3 $4
if [[ $? -ne 0 ]]; then
	echo -e "\\033[1;31mFAILED: Error writing programs. (Check command output) \\033[0;39m"
	exit 1
fi

echo -e "\\033[1;32mAll operations successful\\033[0;39m"
