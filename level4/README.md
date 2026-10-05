To install the module into the kernel:
```
install -D -m 0644 /root/level4/detect_kbd.ko "/lib/modules/$(uname -r)/extra/detect_kbd.ko"
depmod -a
```