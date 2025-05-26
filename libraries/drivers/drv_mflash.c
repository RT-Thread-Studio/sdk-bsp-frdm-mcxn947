#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "fsl_flash.h"


static flash_config_t g_flash_instance = {0};

static uint32_t g_pflashBlockBase  = 0;
static uint32_t g_pflashTotalSize  = 0;
static uint32_t g_pflashSectorSize = 0;
static uint32_t g_pflashPageSize   = 0;

struct mflash_mtd
{
        struct rt_mtd_nor_device mtd_device;
        struct rt_mutex lock;
        uint32_t mflash_start;
        uint8_t  page_size;
        void * user_data;
};


rt_ssize_t mlash_write(struct rt_mtd_nor_device* device, rt_off_t offset, const rt_uint8_t* data, rt_size_t length)
{
    status_t ret;
    uint32_t dest;

    struct mflash_mtd * p_mflash_mtd = (struct mflash_mtd *)device;

    dest = p_mflash_mtd->mflash_start + offset;

    if((dest + length) > (device->block_end*device->block_size))
        return 0;

    for(uint32_t i = 0;i < length; i += 128)
    {
       ret = FLASH_Program(&g_flash_instance, dest, (uint8_t *)data, 128);
       if(ret != kStatus_Success)
         break;
    }
 
    return ret == kStatus_Success ? length : 0;
}


rt_ssize_t mflash_read(struct rt_mtd_nor_device* device, rt_off_t offset, rt_uint8_t* data, rt_size_t length)
{
    uint32_t dest;

    struct mflash_mtd * p_mflash_mtd = (struct mflash_mtd *)device;

    dest = p_mflash_mtd->mflash_start + offset;

    if((dest + length) > (device->block_end*device->block_size))
        return 0;

    for(uint32_t i=0; i < length; i++)
    {
        data[i] = *((__IO rt_uint8_t*)dest);
        dest++;
    }

    return length;
}


rt_err_t mflash_erase_block(struct rt_mtd_nor_device* device, rt_off_t offset, rt_size_t length)
{

    status_t ret;
    uint32_t addr;

    struct mflash_mtd * p_mflash_mtd = (struct mflash_mtd *)device;

    addr = p_mflash_mtd->mflash_start + offset;

    ret = FLASH_Erase(&g_flash_instance, addr, FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES, kFLASH_ApiEraseKey);

    return ret == kStatus_Success ? RT_EOK : -1;

}


const static struct rt_mtd_nor_driver_ops mflash_ops =
{
    .read_id = NULL,
    .read = mflash_read,
    .write = mlash_write,
    .erase_block = mflash_erase_block,
};


int rt_hw_mflash_init(void)
{
    static struct mflash_mtd mflash;

    mflash.mtd_device.ops = &mflash_ops;
    mflash.mtd_device.block_size = FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
    mflash.mtd_device.block_start = 128;
    mflash.mtd_device.block_end = 255;
    mflash.mflash_start = 0x100000;
    mflash.page_size = 128;
    
    status_t result;

    result = FLASH_Init(&g_flash_instance);
    if (result != kStatus_Success)
        return result;

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashBlockBaseAddr, &g_pflashBlockBase);
    if (result != kStatus_Success)
        return result;

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashSectorSize, &g_pflashSectorSize);
    if (result != kStatus_Success)
        return result;

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashTotalSize, &g_pflashTotalSize);
    if (result != kStatus_Success)
        return result;

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashPageSize, &g_pflashPageSize);

    rt_mtd_nor_register_device("mflash",&mflash.mtd_device);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_mflash_init);


static void mflash(int argc,char *argv[])
{
    static rt_device_t mtd = RT_NULL;

    if(!strcmp(argv[1], "open"))
    {
        mtd = rt_device_find("mflash");

        if(!mtd)
            rt_kprintf("rt find device failed.\n");
        else
            rt_kprintf("rt find device ok.\n");
    }
    else if(!strcmp(argv[1], "erase"))
    {
        rt_mtd_nor_erase_block((struct rt_mtd_nor_device*)mtd,0x00,8192);
    }
    else if(!strcmp(argv[1], "write"))
    {
        char * writetest = " hello world   ";
        
        rt_mtd_nor_write((struct rt_mtd_nor_device*)mtd,0,writetest,16);
    }
    else if(!strcmp(argv[1], "read"))
    {
        char readbuff[16];
        
        rt_mtd_nor_read((struct rt_mtd_nor_device*)mtd,0,readbuff,16);
                          
        rt_kprintf("read test data is %s \n",readbuff);
    }

}
MSH_CMD_EXPORT(mflash, mflash mtd test);