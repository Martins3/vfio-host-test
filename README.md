 VFIO host test case

This is a test suite to try all the VFIO features on a selected device.
To run the test simply:

./vfio_v5_test_device.sh [-i|--test-irq]

If the the FastModels DMA330 controller is tested then, after the generic test,
the device specific code will be executed.

## 实际上的测试

需要提前利用 ubind 和确定 iommu group 了

```txt
10002  cd src_test
10003  make
10004  ./vfio_dev_test 0000:02:00.0 16 1 pci

dma-mapping some memory to the device
dma-map successful

Num regions: 9
    region #0:
        size: 4000
        offset: 0x0
        flags: 0xf
        Successful MMAP to address 0x7ff8d5e08000
    region #4:
        size: 10000
        offset: 0x40000000000
        flags: 0x7
        Successful MMAP to address 0x7ff8d5a4d000
    region #7:
        size: 1000
        offset: 0x70000000000
        flags: 0x3
mmap failed: Invalid argument

Num irqs: 5
    irq #0:
        flags: 0x7
        count: 1
    irq #1:
        flags: 0x9
        count: 1
    irq #2:
        flags: 0x1
        count: 66
    irq #3:
        flags: 0x9
        count: 1
    irq #4:
        flags: 0x9
        count: 1
```
