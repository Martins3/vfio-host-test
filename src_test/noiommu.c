/*
 * chagpt 写的，感觉就是参考这个项目，不过这个调整是非常科学的
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/vfio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
  /* 假设设备 group 是 noiommu-25 */
  const char *group_path = "/dev/vfio/noiommu-0";
  const char *device_name = "0000:00:03.0";

  int container = open("/dev/vfio/vfio", O_RDWR);
  if (container < 0) {
    perror("open container");
    return 1;
  }

  if (ioctl(container, VFIO_GET_API_VERSION) != VFIO_API_VERSION) {
    printf("VFIO API mismatch\n");
    return 1;
  }

  /* 打开 noiommu group */
  int group = open(group_path, O_RDWR);
  if (group < 0) {
    perror("open group");
    return 1;
  }

  if (ioctl(group, VFIO_GROUP_SET_CONTAINER, &container)) {
    perror("SET_CONTAINER");
    return 1;
  }

  /* 分配 noiommu container 类型 */
  int t = VFIO_NOIOMMU_IOMMU;
  if (ioctl(container, VFIO_SET_IOMMU, t)) {
    perror("SET_IOMMU (noiommu)");
    return 1;
  }

  /* 获取设备句柄 */
  int dev = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, device_name);
  if (dev < 0) {
    perror("GET_DEVICE_FD");
    return 1;
  }

  printf("\n=== Regions of device %s ===\n\n", device_name);

  /* 枚举 region */
  for (uint32_t i = 0;; i++) {
    struct vfio_region_info reg = {
        .argsz = sizeof(reg),
        .index = i,
    };

    if (ioctl(dev, VFIO_DEVICE_GET_REGION_INFO, &reg) < 0)
      break; // 没有更多 region
	     //
    if(reg.size == 0)
	    continue;

    printf("Region %u:\n", i);
    printf("  size   : 0x%llx\n", (unsigned long long)reg.size);
    printf("  offset : 0x%llx\n", (unsigned long long)reg.offset);
    printf("  flags  : 0x%x\n", reg.flags);

    if (reg.flags & VFIO_REGION_INFO_FLAG_READ)
      printf("    - READ\n");
    if (reg.flags & VFIO_REGION_INFO_FLAG_WRITE)
      printf("    - WRITE\n");
    if (reg.flags & VFIO_REGION_INFO_FLAG_MMAP)
      printf("    - MMAP capable\n");

    printf("\n");
  }

  close(dev);
  close(group);
  close(container);
  return 0;
}
