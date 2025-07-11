## How to create a VM with QEMU :

qemu-system-x86_64 -m 4G -smp 4 -cpu host -enable-kvm -cdrom RainFall.iso  -net nic -net user,hostfwd=tcp::2222-:4242 -daemonize

## How to connect :

ssh level0@localhost -p 2222
password: level00

## How to use scp (dl with secure protocole) :

scp -P 2222 level00@localhost:~/ /tmp

## How to get the flag :

level00@SnowCrash:~$ su flag00
Password:
Don't forget to launch getflag !
flag00@SnowCrash:~$ getflag
Check flag.Here is your token : ?????????????????
flag00@SnowCrash:~$ su level01
Password:
level01@SnowCrash:~$ 

## How to install Wireshark:

sudo add-apt-repository ppa:wireshark-dev/stable
sudo apt-get update
sudo apt-get install wireshark

## How to install John The Ripper

sudo snap install john-the-ripper

## Roles

cjunker level00 rot13
flemaitr level01 john the ripper
flemaitr level02 wireshark
cjunker level03 echo
cjunker level04 perl
flemaitr level05 openaserver
cjunker level06 php regex
flemaitr level07 buffer overflow
flemaitr level08 lien symbolique
cjunker level09 decipher


