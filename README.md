# Levels

### level0
Straight forward, show config and show kernel log  

### level1
Straight forward, show code, show mode loading and unloading  

### level2
Straight forward, show git patch and kernel log  

### level3
Straight forward, show Documentation/process/coding-style.rst  

### level4 #verify
Launch VM with:
```
qemu-system-x86_64 \
        -enable-kvm \
        -m 8192 \
        -smp 20 \
        -cpu max \
        -nic user,hostfwd=tcp::2222-:22 \
        -vga virtio \
        -hda /media/aascedu/USB/save_lfs.qcow2 \
        -device qemu-xhci,id=xhci \
        -device usb-kbd,bus=xhci.0 \
        -monitor telnet:127.0.0.1:4444,server,nowait
```

To add a keyboard or remove a keyboard:
```
telnet 127.0.0.1 4444
(qemu) device_add usb-kbd,bus=xhci.0,id=vkbd
(qemu) device_remove vkbd
```


To install the module into the kernel:
```
install -D -m 0644 /root/level4/detect_kbd.ko "/lib/modules/$(uname -r)/extra/detect_kbd.ko"
depmod -a
```

### level5
To check that we get the right result during write operation we can use : `strace -e write echo -n "aascedu" > /dev/fortytwo`  
