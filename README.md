# mktables

This is basically just a hidden firewall rule.

mktables is a kernel module that uses netfilter to hook the Linux network stack. In this scenario, the NF_IP_PRE_ROUTING hook is used to run a handle function before any routing decisions are made with a packet, incoming or outgoing. With the current configuration (PORT=0x50 or PORT=80 if you convert to decimal), any incoming packets with a destination port of 80 will be dropped.

![alt text](images/image.png)

This throws off the usual Blue Team debugging steps of "if it is reachable locally but not remotely, it must be a firewall, iptables or nftables". The NF_IP_PRE_ROUTING hook is executed prior to any routing decisions, so that includes local connections.

While this may be too difficult for most Blue Teamers to find, I plan on dropping this on only one host mid-way through the competition, then giving them hints for where to look (i.e. syslog, loaded modules)

## Usage

To compile the module, you will need to first download the target kernel version (run uname -r on the target) and all the dependencies for building kernel modules. For Debian systems run:

`sudo apt install gcc make build-essential libncurses-dev exuberant-ctags`

To have the tools to build the module, then:

`sudo apt install linux-headers-<KERNEL VERSION>`

for the target kernel. If you are running this from the machine you would like to load the module to, you can install `linux-headers-$( uname -r )`.

Then, you need to export the environment variable that will be used for the build with:

`export KERN=<KERNEL VERSION>`

Before building, if you would like to change destination port to which packets will be dropped, change the value of `#define PORT 0x50` in line 17 of `packet.c` to the hex representation of the port you want to block. Once you have configured this, you must run 

`make`

in the root directory of this repo. Then, you can insert the resulting module with 

`sudo insmod net_helper`

Traffic to that port will now be dropped!