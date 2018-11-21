#include <config.h>
#include <common.h>
#include <linux/types.h>
#include <asm/arch/bits.h>
#include <linux/string.h>
#include <android_bootimg.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <nand.h>
#include <android_boot.h>
#include <environment.h>
#include <jffs2/jffs2.h>
#include <boot_mode.h>
#include <asm/arch/sci_types.h>
#include "../disk/part_uefi.h"

static unsigned int nv_buffer[256]={0};
static int s_is_calibration_mode = 0;
static char *calibration_cmd_buf;
char *get_calibration_parameter()
{
	if(s_is_calibration_mode != 0)
	return calibration_cmd_buf;
	else
	return NULL;
}

#define CALIBERATE_COMMAND_AUTOTEST  0x15
/* calibration support uart only */
#ifdef CONFIG_MODEM_CALI_UART

#define mdelay(_ms) udelay(_ms*1000)
#define CALIBERATE_STRING_LEN 10
#define CALIBERATE_HEAD 0x7e
#define CALIBERATE_COMMOND_T 0xfe
#define CALIBERATE_COMMAND_REQ  1

#define CALIBERATE_DEVICE_NULL  0
#define CALIBERATE_DEVICE_USB   1
#define CALIBERATE_DEVICE_UART  2

extern int charger_connected(void);
typedef  struct tag_cali_command {
	unsigned int   	reserved;
	unsigned short  size;
	unsigned char   cmd;
	unsigned char   sub_cmd;
} COMMAND_T;

extern int serial_tstc(void);
static unsigned long long start_time;
static unsigned long long now_time;



static caliberate_device = CALIBERATE_DEVICE_NULL;

static void send_caliberation_request(void)
{
	COMMAND_T cmd;
        unsigned int i;
        unsigned char *data = (unsigned char *)&cmd;

        cmd.reserved = 0;
        cmd.cmd = CALIBERATE_COMMOND_T;
        cmd.size = CALIBERATE_STRING_LEN-2;
        cmd.sub_cmd = CALIBERATE_COMMAND_REQ;

        serial_putc(CALIBERATE_HEAD);

        for (i = 0; i < sizeof(COMMAND_T); i++)
             serial_putc(data[i]);

        serial_putc(CALIBERATE_HEAD);
}

static int receive_caliberation_response(uint8_t *buf,int len)
{
        int count = 0;
        int ch;
        uint32_t is_not_empty = 0;
        uint32_t start_time = 0,current_time = 0;

	if ((buf == NULL) || (len == 0))
        	return 0;
        
        is_not_empty = serial_tstc();
        if (is_not_empty) {
             start_time = get_timer_masked();
             do {
                  do {
                  	ch = serial_getc();
                  	if (count < CALIBERATE_STRING_LEN)
		        	buf[count++] = ch;
                  } while (serial_tstc());

                  if ((count >= CALIBERATE_STRING_LEN) || (count >= len)) {
                       caliberate_device = CALIBERATE_DEVICE_UART;
                       break;
                  }

                  current_time = get_timer_masked();
             } while((current_time - start_time) < 500);
        }
        
        return count;
}

unsigned int check_caliberate(uint8_t * buf, int len)
{
	unsigned int command = 0;
    	unsigned int freq = 0;

	if (len != CALIBERATE_STRING_LEN)
		return 0;

	if ((*buf == CALIBERATE_HEAD) && (*(buf + len -1) == CALIBERATE_HEAD)) {
		if ((*(buf+7) == CALIBERATE_COMMOND_T) && (*(buf + len - 2) != 0x1)) {
			command = *(buf + len - 2);
			command &= 0x7f;

            		freq = *(buf + 1);
            		freq = freq << 8;
            		freq += *(buf + 2);

            		command += freq << 8;
		}
	}

	return command;
}

BOOLEAN calibration_detect(int key)
{
	int ret;
	int i ;
	unsigned int caliberate_mode;
	uint8_t buf[20];
	int got = 0;

	printf("%s\n", "uart calibrate detecting");
	loff_t off = 0;    	
    	send_caliberation_request();
   
#ifdef CONFIG_MODEM_CALIBERATE
	for(i = 0; i < 20; i++)
		buf[i] = i + 'a';

    	start_time = get_timer_masked();
    	while (1) {
   		got = receive_caliberation_response(buf, sizeof(buf));
   		if (caliberate_device == CALIBERATE_DEVICE_UART)
			break;

    		now_time = get_timer_masked();
    		if ((now_time - start_time) > CALIBRATE_ENUM_MS) {
      			printf("usb calibrate configuration timeout\n");
			return FALSE;
    		}
    	}	

	printf("caliberate : what got from host total %d is \n", got);
	for (i = 0; i < got; i++)
		printf("0x%x ", buf[i]);
	printf("\n");

	caliberate_mode = check_caliberate(buf, CALIBERATE_STRING_LEN);
	if (!caliberate_mode) {
        	printf("func: %s line: %d caliberate failed\n", __func__, __LINE__);
		return FALSE;
        } else {
        calibration_cmd_buf=malloc(1024);
        if(calibration_cmd_buf==NULL){
            printf("%s: out of memory\n", __func__);
            return FALSE;
        }
		memset(calibration_cmd_buf, 0, 1024);
              	if (caliberate_device == CALIBERATE_DEVICE_UART)
					sprintf(calibration_cmd_buf, " androidboot.mode=cali calibration=%d,%d,0", caliberate_mode&0xff, (caliberate_mode&(~0xff)) >> 8);
                s_is_calibration_mode = 2;
#if defined(CONFIG_SC7710G2)
		vlx_nand_boot(BOOT_PART, buf, BACKLIGHT_OFF);
#else
	#if defined(BOOT_NATIVE_LINUX_MODEM)
    
        int str_len = 0;
        char* bootargs = CONFIG_BOOTARGS;
        char* pos = NULL;
        pos = strstr(bootargs, "console=");
        if(NULL != pos)
        {
            str_len = pos-bootargs;
            strncpy(&calibration_cmd_buf[0], bootargs, str_len);
            bootargs = pos;
            pos = strstr(bootargs, " ");
        }
        else
        {
            pos = CONFIG_BOOTARGS;
            str_len = 0;
        }
        sprintf(&calibration_cmd_buf[str_len], "%s", pos);
	  str_len = strlen(calibration_cmd_buf);

        if (caliberate_device == CALIBERATE_DEVICE_UART)
            sprintf(&calibration_cmd_buf[str_len], " androidboot.mode=cali calibration=%d,%d,130 ", caliberate_mode&0xff, (caliberate_mode&(~0xff)) >> 8);
        
        vlx_nand_boot(RECOVERY_PART,calibration_cmd_buf, BACKLIGHT_OFF);
    #else
		vlx_nand_boot(BOOT_PART, calibration_cmd_buf, BACKLIGHT_OFF);
	#endif
#endif
	}	
    
    	/* nerver come to here */
   	return FALSE;
#endif
}

#else	/* usb calibration only */

extern int dwc_otg_driver_init(void);
extern void dwc_otg_driver_cleanup(void);
extern int usb_fastboot_initialize(void);
extern int usb_serial_init(void);
extern void  usb_serial_cleanup(void);
extern int usb_serial_configed;
extern int usb_port_open;
extern int usb_read_done;
extern int usb_write_done;
extern int usb_trans_status;
extern int usb_gadget_handle_interrupts(void);

extern int gs_open(void);
extern int gs_close(void);
extern int gs_read(const unsigned char *buf, int *count);
extern int gs_write(const unsigned char *buf, int count);
extern void usb_wait_trans_done(int direct);
extern int usb_is_trans_done(int direct);
extern int usb_is_configured(void);
extern int usb_is_port_open(void);
extern void udc_power_on(void);
extern void udc_power_off(void);
extern void usb_in_cal(int flag);

#define mdelay(_ms) udelay(_ms*1000)

//#define DEBUG
#ifdef DEBUG
#define dprintf(fmt, args...) printf(fmt, ##args)
#else
#define dprintf(fmt, args...) 
#endif

#ifdef DEBUG
#define IO_DEBUG 1     // open write and read debug
#define WRITE_DEBUG 1  // 0: for read debug, 1 for write debug
#else
#define IO_DEBUG 0
#endif


#define CALIBERATE_STRING_LEN 10
#define CALIBERATE_HEAD 0x7e
#define CALIBERATE_COMMOND_T 0xfe

extern int get_cal_enum_ms(void);
extern int get_cal_io_ms(void);	

unsigned int check_caliberate(uint8_t * buf, int len)
{
	unsigned int command = 0;
    unsigned int freq = 0;
	if(len != CALIBERATE_STRING_LEN)
		return 0;

	if((*buf == CALIBERATE_HEAD) && (*(buf + len -1) == CALIBERATE_HEAD)){
		if((*(buf+7)==CALIBERATE_COMMOND_T) && (*(buf+len-2) != 0x1)){
			command = *(buf+len-2);
			command &= 0x3f;
            freq = *(buf+1);
            freq = freq<<8;
            freq += *(buf+2);
            command += freq<<8;
		}
	}
	return command;
}

extern int power_button_pressed(void);
static int count_ms;
static unsigned long long start_time;
static unsigned long long now_time;

static int recheck_power_button(void)
{
    int cnt = 0;
    int ret = 0;
    do{
        ret = power_button_pressed();
        if(ret == 0)
          cnt++;
        else
          return 1;

        if(cnt>4)
          return 0;
        else{
            mdelay(1);
        }
    }while(1);
}
int is_timeout(int key)
{
    if(!key){
        if(!recheck_power_button() || charger_connected())
          return 2;
    }
    
    now_time = get_timer_masked();

    if(now_time - start_time>count_ms)
      return 1;
    else{
        return 0;
    }
}
    
BOOLEAN calibration_detect(int key)
{
	int ret;
	int i ;
    unsigned int caliberate_mode;
	loff_t off = 0;
    printf("%s\n", "usb calibrate detecting");

    count_ms = get_cal_enum_ms();
   // extern lcd_display(void);
   // extern void set_backlight(uint32_t value);
   // lcd_printf("   caliberation mode");
   // lcd_display();
   // set_backlight(50);
#ifdef CONFIG_SC8830
	if(!charger_connected())
		return FALSE;
#endif
#ifdef CONFIG_MODEM_CALIBERATE
	uint8_t buf[20];
	for(i = 0; i<20; i++)
		buf[i] = i+'a';
	usb_in_cal(1);
	dwc_otg_driver_init();
	usb_serial_init();
#if IO_DEBUG 
	while(!usb_serial_configed)
		usb_gadget_handle_interrupts();
	printf("USB SERIAL CONFIGED\n");
	gs_open();
#if WRITE_DEBUG
	while(1){
		ret = gs_write(buf, 20);
		printf("func: %s waitting write done\n", __func__);
		if(usb_trans_status)
			printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
		usb_wait_trans_done(1);
		printf("func: %s readly send %d\n", __func__, ret);
    }		
#else
	while(1){
		int count = 20;
		usb_wait_trans_done(0);	
		if(usb_trans_status)
					printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
		ret = gs_read(buf, &count);
		printf("func: %s readly read %d\n", __func__, count);		
		if(usb_trans_status)
			printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
		for(i = 0; i<count; i++)
			printf("%c ", buf[i]);
		printf("\n");
	}	

#endif
#endif
    start_time = get_timer_masked();
    while(!usb_is_configured()){
	 /*down the device if charger disconnect during calibration detect.*/
	if(!charger_connected()){
		power_down_devices();
		while(1);
	}

        ret = is_timeout(key);
        if(ret == 0)
          continue;
        else if(ret == 2) // POWER KEY pressed
          power_down_devices();
        else{
            printf("usb calibrate configuration timeout\n");
            return FALSE;
        }
    }	
	printf("USB SERIAL CONFIGED\n");

    start_time = get_timer_masked();
    count_ms = get_cal_io_ms();
    while(!usb_is_port_open()){
	 /*down the device if charger disconnect during calibration detect.*/
	if(!charger_connected()){
		power_down_devices();
		while(1);
	}

        ret = is_timeout(key);
        if(ret == 0)
          continue;
        else if(ret == 2) // POWER KEY pressed
          power_down_devices();
        else{
            printf("usb calibrate port open timeout\n");
            return FALSE;
        }
    }	
	printf("USB SERIAL PORT OPENED\n");
	gs_open();
//code for caliberate detect
	int got = 0;
	int count = CALIBERATE_STRING_LEN;
	dprintf("start to calberate\n");
	
	while(got < CALIBERATE_STRING_LEN){
		if(usb_is_trans_done(0))		
//	while(got < CALIBERATE_STRING_LEN){
//		usb_wait_trans_done(0);
        {		
            if(usb_trans_status)
              printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
            ret = gs_read(buf + got, &count);
            if(usb_trans_status)
              printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
            for(i=0; i<count; i++)
              dprintf("0x%x \n", buf[got+i]);
            dprintf("\n");
            got+=count;
        }

        if(got<CALIBERATE_STRING_LEN){
            ret = is_timeout(key);
            if(ret == 0){
                count=CALIBERATE_STRING_LEN-got;
                continue;
            }else if(ret == 2){
                power_down_devices();
            }else{
                printf("usb read timeout\n");
                return FALSE;
            }
        }else{
            break;
        }
	}

	printf("caliberate:what got from host total %d is \n", got);
	for(i=0; i<got;i++)
		printf("0x%x ", buf[i]);
	printf("\n");

	caliberate_mode = check_caliberate(buf, CALIBERATE_STRING_LEN);
	dprintf("check_caliberate return is 0x%x\n", ret);
	if(!caliberate_mode){
        printf("func: %s line: %d caliberate failed\n", __func__, __LINE__);
		return FALSE;
    }
	else{
		ret = gs_write(buf, got);
		dprintf("func: %s waitting %d write done\n", __func__, got);
		if(usb_trans_status)
			printf("func: %s line %d usb trans with error %d\n", __func__, __LINE__, usb_trans_status);
		usb_wait_trans_done(1);
		start_time = get_timer_masked();
		while(usb_port_open){
			usb_gadget_handle_interrupts();
			now_time = get_timer_masked();
			if(now_time - start_time > 500)
				break;
		}
		udc_power_off();
        calibration_cmd_buf=malloc(1024);
        if(calibration_cmd_buf==NULL){
            printf("%s: out of memory\n", __func__);
            return FALSE;
        }
		memset(calibration_cmd_buf, 0, 1024);
        s_is_calibration_mode=1;
#if defined( CONFIG_SP7702) || defined(CONFIG_SP8810W) || defined(CONFIG_SC7710G2)
		vlx_nand_boot(BOOT_PART, buf, BACKLIGHT_OFF);
#else
	#if defined(BOOT_NATIVE_LINUX_MODEM)
		int str_len;
		printf("func: %s caliberate_mode: %x \n",__func__, caliberate_mode&0xff);
		if((caliberate_mode&0xff) == CALIBERATE_COMMAND_AUTOTEST){
			sprintf(calibration_cmd_buf, CONFIG_BOOTARGS" androidboot.mode=engtest autotest=1");
			vlx_nand_boot(BOOT_PART, calibration_cmd_buf, BACKLIGHT_OFF);
		}else {
			sprintf(calibration_cmd_buf, CONFIG_BOOTARGS" androidboot.mode=cali calibration=%d,%d,146", caliberate_mode&0xff, (caliberate_mode&(~0xff))>>8);
			vlx_nand_boot(RECOVERY_PART,calibration_cmd_buf, BACKLIGHT_OFF);
		}
	#else
		vlx_nand_boot(BOOT_PART, calibration_cmd_buf, BACKLIGHT_OFF);
	#endif
#endif
	}	
    
    //nerver come to here
    free(calibration_cmd_buf);
    return FALSE;
#endif
}

#endif // CONFIG_MODEM_CALI_UART

int poweron_by_calibration(void)
{
	return s_is_calibration_mode;
}

int cali_file_check(void)
{

#define CALI_MAGIC      (0x49424143) //CALI
#define CALI_COMP       (0x504D4F43) //COMP


	if(do_fs_file_read("prodnv", "/adc.bin", (char *)nv_buffer,sizeof(nv_buffer)))
		return 1;

	if((nv_buffer[0] != CALI_MAGIC)||(nv_buffer[1]!=CALI_COMP))
		return 1;
	else 
		return 0;		
}

#ifndef CONFIG_AP_ADC_CALIBRATION
#if defined(CONFIG_EMMC_BOOT) && defined (CONFIG_SC8825)
#include "calibration_nv_struct.h"

#define VLX_ADC_ID   2
#define VLX_RAND_TO_U32( _addr )	if( (_addr) & 0x3 ){_addr += 0x4 -((_addr) & 0x3); }

u32 Vlx_GetFixedNvitemAddr(u16 identifier, u32 search_start, u32 search_end)
{
	u32 start_addr, end_addr;
	u16 id, len;
	volatile u16 *flash_ptr;

	start_addr = search_start;
	end_addr   = search_end;
	start_addr += sizeof(u32); //skip update flag

	while(start_addr < end_addr)
	{
		flash_ptr = (volatile u16 *)(start_addr);
		id  = *flash_ptr++;
		len = *flash_ptr;
		if(0xFFFF == id)
		{
			return 0xFFFFFFFF;
		}
		if(identifier == id)
		{
			return (start_addr + 4);
		}
		else
		{
			start_addr += 4 + len +(len & 0x1);
			VLX_RAND_TO_U32( start_addr )
		}
	}
	return 0xFFFFFFFF;
}
#endif

int read_adc_calibration_data(char *buffer,int size)
{
#if  defined (CONFIG_SC8830)
	if(do_fs_file_read("prodnv", "/adc.bin", (char *)nv_buffer,sizeof(nv_buffer)))
	    	return 0;

	if(size>48)
		size=48;
	memcpy(buffer,&nv_buffer[2],size);
	return size;
#elif defined(CONFIG_EMMC_BOOT) && defined (CONFIG_SC8825)
	#define FIXNV_ADR        0x80480000
	calibration_param_T *calibration_base;
	u32 item_base;
	uint16 *value = (uint16 *)(&buffer[8]);
	item_base = Vlx_GetFixedNvitemAddr(VLX_ADC_ID, FIXNV_ADR, (FIXNV_ADR+FIXNV_SIZE));
	if(item_base == 0xFFFFFFFF)
		return 0;
	calibration_base = (calibration_param_T *)item_base;
	if(!((calibration_base->adc).reserved[7] & (BIT_9)))
		return 0;

	value[0] = ((calibration_base->adc).battery[0]) & 0xFFFF;
	value[1] = ((calibration_base->adc).battery[0] >> 16 ) & 0xFFFF;
	value[2] = ((calibration_base->adc).battery[1]) & 0xFFFF;
	value[3] = ((calibration_base->adc).battery[1] >> 16 ) & 0xFFFF;
	return size;
#endif
	return 0;
}
#endif
