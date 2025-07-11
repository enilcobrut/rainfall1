#!/usr/bin/env bash

qemu-system-x86_64 -m 4G -smp 4 -cpu host -enable-kvm -cdrom ~/Documents/RainFall.iso  -net nic -net user,hostfwd=tcp::2222-:4242 -daemonize
