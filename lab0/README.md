# A Kernel Seedling

## Building

To count how many processes are running in the kernel, I use a counter and increment the counter for every loop in for_each_process.

The count is printed to a file, which is set as the open option in the process options. I create an process in init with the process options and remove the process on exit. 

## Running

First call "make" to compile proc_count.c into .ko file, and "sudo insmod proc_count.ko to add it to kernel modules. Then use "cat /proc/count" to get the count of processes running. 

## Cleaning Up

Call "sudo rmmod proc_count.ko" to remove the module.

## Testing

The module is tested on linux 5.14.8-arch1-1.

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

