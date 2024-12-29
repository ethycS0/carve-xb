#!/bin/bash

if [ "$(id -u)" -ne 0 ]; then
  echo "This script must be run as root."
  exit 1
fi

USB_PARTITION="/dev/sdb2"
MOUNT_POINT="/mnt/test"

if [ ! -b "$USB_PARTITION" ]; then
  echo "The USB partition $USB_PARTITION does not exist."
  exit 1
fi

if [ ! -d "$MOUNT_POINT" ]; then
  echo "Creating mount point $MOUNT_POINT"
  mkdir -p "$MOUNT_POINT"
fi

echo "Mounting $USB_PARTITION to $MOUNT_POINT"
mount "$USB_PARTITION" "$MOUNT_POINT"

echo "Binding /dev, /proc, /sys, and /run to chroot"
mount --bind /dev "$MOUNT_POINT/dev"
mount --bind /proc "$MOUNT_POINT/proc"
mount --bind /sys "$MOUNT_POINT/sys"
mount --bind /run "$MOUNT_POINT/run"

echo "Entering chroot environment at $MOUNT_POINT"
chroot "$MOUNT_POINT" /bin/zsh

echo "Exiting chroot environment. Unmounting filesystems."

umount "$MOUNT_POINT/run"
umount "$MOUNT_POINT/sys"
umount "$MOUNT_POINT/proc"
umount "$MOUNT_POINT/dev"

umount "$MOUNT_POINT"

if [ -d "$MOUNT_POINT" ]; then
  rmdir "$MOUNT_POINT"
fi

echo "USB partition unmounted and cleaned up."
