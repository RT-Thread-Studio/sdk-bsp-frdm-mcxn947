/*
* Copyright (c) 2006-2024, RT-Thread Development Team
* Copyright (c) 2019-2020, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-10-24     Magicoe      first version
* 2020-01-10     Kevin/Karl   Add PS demo
* 2020-09-21     supperthomas fix the main.c
*
*/

#include <rtdevice.h>
#include <rtthread.h>
#include "drv_pin.h"    

#define LEDG_PIN        ((0*32)+27)   
#define PWM_DEV_NAME     "pwm0"
#define PWM_DEV_CHANNEL    3

struct rt_device_pwm *pwm_dev;

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

    rt_kprintf("MCXN947 HelloWorld\r\n");

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

    rt_uint32_t period = 1000000; /* 1ms,单位ns */
    rt_uint32_t pulse = 0; //占空比
    rt_uint32_t dir = 1; //增长方向
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);

    if(pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm test run failed! can't find %s device!\n",PWM_DEV_NAME);
    }

    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 0);
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    rt_kprintf("pwm start\r\n");

    while (1)
    {
        rt_thread_mdelay(20);

        if(dir)
        {
            pulse += 10000; /* 从0开始每次增加10000ns */
        }
        else
        {
            pulse -= 10000; /* 从最大值每次减少10000ns */
        }

        if(pulse >= period)
        {
            dir = 0;
        }
        if(pulse == 0)
        {
            dir = 1;
        }
        /* 设置PWM周期和脉冲宽度 */
        rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    }
}
// end file
