Run:
```
qemu-system-x86_64 \
  -enable-kvm \
  -m 8192 \
  -smp 20 \
  -cpu max \
  -nic user,hostfwd=tcp::2222-:22 \
  -vga virtio \
  -hda image_name.qcow2
```
