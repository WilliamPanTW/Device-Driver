sudo insmod Cat.ko
sudo dmesg | tail -n 1
sudo mknod /dev/Cat c 415 0
sudo chmod 666 /dev/Cat