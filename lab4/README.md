# Hey! I'm Filing Here

Create a ext2 file system containing superblock, group descriptor, block & inode bitmap, and inode table. Create a root directory, a directory, a file and a symbolic link to the file.

## Building

First, set properties of superblock and group descriptor table to correct value.
In block and inode bitmaps, write 1 when a block or inode exists, and write the rest as 0.
Write root, hello-world, hello inodes in inode table. 
Write root dir and hello world file blocks.

## Running

make
./ext2-create
mkdir <mount_dir_name>
sudo mount -o loop cs111-base.img <mount_dir_name>

## Cleaning up

sudo umount <mount_dir_name>
rmdir <mount_dir_name>
make clean
