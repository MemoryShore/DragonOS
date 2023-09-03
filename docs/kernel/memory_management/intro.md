# 内存管理模块简介

## 1. 概述

&emsp;&emsp;DragonOS实现了具有优秀架构设计的内存管理模块，对内核空间和用户空间的内存映射、分配、释放、管理等操作进行了封装，使得内核开发者可以更加方便地进行内存管理。

&emsp;&emsp;DragonOS的内存管理模块主要由以下类型的组件组成：

- **硬件抽象层（MemoryManagementArch）** - 提供对具体处理器架构的抽象，使得内存管理模块可以在不同的处理器架构上运行
- **页面映射器（PageMapper）**- 提供对虚拟地址和物理地址的映射，以及页表的创建、填写、销毁、权限管理等操作。分为两种类型：内核页表映射器（KernelMapper）和用户页表映射器（位于具体的用户地址空间结构中）
- **页面刷新器（PageFlusher）** - 提供对页表的刷新操作（整表刷新、单页刷新、跨核心刷新）
- **页帧分配器（FrameAllocator）** - 提供对页帧的分配、释放、管理等操作。具体来说，包括BumpAllocator、BuddyAllocator
- **小对象分配器** - 提供对小内存对象的分配、释放、管理等操作。指的是内核里面的SlabAllocator （SlabAllocator的实现目前还没有完成）
- **MMIO空间管理器** - 提供对MMIO地址空间的分配、管理操作。（目前这个模块待进一步重构）
- **用户地址空间管理机制** - 提供对用户地址空间的管理。
    - VMA机制 - 提供对用户地址空间的管理，包括VMA的创建、销毁、权限管理等操作
    - 用户映射管理 - 与VMA机制共同作用，管理用户地址空间的映射
- **系统调用层** - 提供对用户空间的内存管理系统调用，包括mmap、munmap、mprotect、mremap等
- **C接口兼容层** - 提供对原有的C代码的接口，使得C代码能够正常运行。
