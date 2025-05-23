#include <rtdevice.h>

#define ADC_DEV_NAME "adc0"
#define REFER_VOLTAGE 3300 // max adc voltage is 3.3V
#define CONVERT_BITS (1 << 16)  // 16bit adc0

void adc_test(int argv, char **argc) 
{
  rt_adc_device_t adc_dev;
  rt_uint32_t value, vol;
  rt_err_t ret = RT_EOK;

  adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);

  if (adc_dev == RT_NULL) 
  {
    rt_kprintf("adc_test failed! can't find %s device!\n", ADC_DEV_NAME);
    return;
  }

  ret = rt_adc_enable(adc_dev, 0);

  while (1) 
  {
    value = rt_adc_read(adc_dev, 0);
    vol   = value * REFER_VOLTAGE / CONVERT_BITS;
    rt_kprintf("the voltage is :[%d.%03d]V \n", vol / 1000, vol % 1000);
    rt_thread_delay(2000);
  }
  return;
}
MSH_CMD_EXPORT(adc_test, ADC test)