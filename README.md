# NANVIX

## WHAT IS NANVIX?

Nanvix is a Unix-like operating system written by [Pedro H. Penna](https://github.com/ppenna) for 
educational purposes. It is designed to be simple and small, but also 
modern and fully featured.
	
## WHAT HARDWARE IS REQUIRED?

Nanvix targets 32-bit x86-based PCs and only requires a floppy or 
CD-ROM drive and 16 MB of RAM. You can run it either in a real PC 
or in a virtual machine, from a Live System's Image, available 
at: https://sourceforge.net/projects/nanvix/files/
	
## BUILDING

In order to build Nanvix, you will need a Linux like programming 
environment, the x86 GCC compiler and the x86 GNU binutils.

**TIP**: If you are running a Debian-based Linux distribution, like Ubuntu, 
you can simply run the `tools/dev/setup-toolchain.sh` script.
	
Once you have setup the required tools, you should set the 
environment variable `$TARGET` to point to the toolchain's directory. 
	
When done, you can build Nanvix by typing, at the root directory:
	
```sh
$ make nanvix
```

Or you can build a Live System's Image by typing, at the same directory:

```sh
$ make image
```

Note that for build a image you may require priveleged access.

## LICENSE AND MAINTAINERS

Nanvix is a free software that is under the GPL V3 license and is 
maintained by Pedro H. Penna. Any questions or suggestions send him an 
email: <pedrohenriquepenna@gmail.com>
