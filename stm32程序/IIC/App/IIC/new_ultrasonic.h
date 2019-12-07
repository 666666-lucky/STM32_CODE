#ifndef _NEW_ULTRASONIC_H_
#define _NEW_ULTRASONIC_H_

#include "i2c.h"
#include "delay.h"

/*******Port******************/
#define SONIC_I2C_PORT GPIOB
#define SONIC_SCL  GPIO_Pin_10
#define SONIC_SDA  GPIO_Pin_11

/**********Command************/
#define Year 0
#define Week 1
#define Cmd_Reg 0x02
 
#define R100MM 0x01 //80-577 0x50-0x241| 
#define R200MM 0x02 //66-1154|  0x42-0x482| 
#define R300MM 0x03 //66-1731|  0x42-0x6c3| 
#define R400MM 0x04 //80-2308|  0x50-0x904| 
#define R500MM 0x05 //82-2885|  0x52-0xb05| 
#define R600MM 0x06 //80-3462|  0x50-0xd86| 
#define R700MM 0x07 //68-4039|  0x44-0xfc7| 
#define R800MM 0x08 //80-4616|  0x50-0x1208| 
#define R900MM 0x09 //80-5193|  0x50-0x1449| 
#define R1000MM 0x0a //66-5770|  0x42-0x168a| 
#define R1500MM 0x0f //79-8655|  0x4f-0x21cf| 
#define R2000MM 0x14 //79-11540|  0x4f-0x2d14| 
#define R2500MM 0x19 //77-14425|  0x4d-0x3859| 
#define R3000MM 0x1e //79-17310|  0x4f-0x439e| 
#define R3500MM 0x23 //79-20195|  0x4f-0x4ee3| 
#define R4000MM 0x28 //79-23080|  0x4f-0x5a28| 

#define FIRST_LEVEL_NOISE 0x70	
#define SECOND_LEVEL_NOISE 0x71 
#define THIRD_LEVEL_NOISE 0x72 
#define FORTH_LEVEL_NOISE 0x73 
#define FIFTH_LEVEL_NOISE 0x74 
#define SIXTH_LEVEL_NOISE 0x75 

//Test cmd, LED display the hex received
#define I2C_TEST_0 0x8a 
#define I2C_TEST_1 0x8b 
#define I2C_TEST_2 0x8c 

#define TO_5M_MM 0xb0 //10-5200mm 0x0a-0x1450mm
#define TO_5M_US 0xb2 //79-30000|  0x4f-0x7530us
#define TO_5M_MM_TEMP 0xb4 //10-5200mm 0x0a-0x1450mm

#define SONIC_LED_ON 0xc0 //TURN ON LED
#define SONIC_LED_OFF 0xc1 //TURN OFF LED

#define PULL_LOW_SCL 0xc2 //PULL LOW SCL WHEN DETECT I2C, DEFAULT
#define N_PULL_LOW_SCL 0xc3 //DON'T PULL LOW SCL WHEN DETECT I2C, DEFAULT
#define SLEEP_5SEC 0xc4 //ENTER SLEEP MODE AFTER 5SEC
#define SLEEP_1SEC 0xc5 //ENTER SLEEP MODE AFTER 1SEC

/*******functions**********/
void sonic_i2c_init(void);
void change_i2c_address(u8 addr_old,u8 addr_new); // addr_old is the address now, addr_new will be the new address
void new_sonic_init(void);
void sonic_write(u8 cmd);
u8 sonic_read(u8 addr);
u32 sonic_receive(void);
u32 sonic_detect(u8 range);
void sonic_send(u8 data);
void toggle_led(void);
s16 detect_5m(void);
u8 SDA_state(void);
u8 SCL_state(void);
u32 sonic_detect_m(u8 range);
void sonic_i2c_init_m(void);
u8 read_reg(u8 reg);

extern u8 SONIC_ADDR;
extern I2C_TypeDef *SONIC_I2C;


#endif		/*  _NEW_ULTRASONIC_H_ */


