# NXP FRDM-MCXN947 开发板 按键中断示例 说明

## 简介

**FRDM-MCXN947** 开发板上配备了多个用户按键（Buttons），常用于用户输入、系统唤醒、模式切换等功能。开发者可以通过配置 **GPIO 中断（GPIO Interrupt）** 实现对按键事件的实时响应，无需轮询方式，提高系统效率。

本例程实现对sw2的中断控制，按下sw2后，通过串口打印字符串信息。

## 硬件说明

![image-20250526181055698](figures/image-20250526181055698.png)

如上图所示，按键WAKEUP 对应引脚P0_23。

## 示例代码

```c
#include <rtdevice.h>
#include <rtthread.h>
#include "drv_pin.h"

#define LEDB_PIN        ((1*32)+2)
#define BUTTON_PIN      ((0*32)+23)

static void sw_pin_cb(void *args);

int main(void)
{
#if defined(__CC_ARM)
    rt_kprintf("using armcc, version: %d\n", __ARMCC_VERSION);
#elif defined(__clang__)
    rt_kprintf("using armclang, version: %d\n", __ARMCC_VERSION);
#elif defined(__ICCARM__)
    rt_kprintf("using iccarm, version: %d\n", __VER__);
#elif defined(__GNUC__)
    rt_kprintf("using gcc, version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
#endif

    rt_pin_mode(LEDB_PIN, PIN_MODE_OUTPUT);  /* Set GPIO as Output */

    rt_pin_mode(BUTTON_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(BUTTON_PIN, PIN_IRQ_MODE_FALLING, sw_pin_cb, RT_NULL);
    rt_pin_irq_enable(BUTTON_PIN, 1);

    rt_kprintf("MCXN947 Hello\n");


#ifdef RT_USING_SDIO
    rt_thread_mdelay(2000);
    if (dfs_mount("sd", "/", "elm", 0, NULL) == 0)
    {
        rt_kprintf("sd mounted to /\n");
    }
    else
    {
        rt_kprintf("sd mount to / failed\n");
    }
#endif

    while (1)
    {
        rt_pin_write(LEDB_PIN, PIN_HIGH);    /* Set GPIO output 1 */
        rt_thread_mdelay(500);               /* Delay 500mS */
        rt_pin_write(LEDB_PIN, PIN_LOW);     /* Set GPIO output 0 */
        rt_thread_mdelay(500);               /* Delay 500mS */
    }
}

static void sw_pin_cb(void *args)
{
    rt_kprintf("sw pressed\r\n");
}
// end file
```

## 编译&下载

* RT-Thread Studio：在RT-Thread Studio 的包管理器中下载FRDM-MCXN947 资源包，然后创建新工程，执行编译。

* MDK：首先双击mklinks.bat，生成rt-thread 与libraries 文件夹链接；再使用Env 生成MDK5工程；最后双击project.uvprojx打开MDK工程，执行编译。

编译完成后，将开发板的CMSIS-DAP接口与PC 机连接，然后将固件下载至开发板。(连接如下图)

![](figures/usb_pc.png)

## 运行

将开发板对应的串口与PC相连，在串口工具中打开对应的串口（115200-8-1-N），复位设备。则显示RT-Thread的输出信息，按下按键，打印 sw pressed。

```bash
 \ | /
- RT -     Thread Operating System
 / | \     5.0.1 build May 26 2025 18:17:41
 2006 - 2022 Copyright by RT-Thread team
using gcc, version: 10.2
MCXN947 Hello
msh >sw pressed
sw pressed
```

