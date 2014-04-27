#ifndef _BSP_H_ 
#define _BSP_H_

#ifdef BSP_GLOBALS
#define BSP_EXT
#else 
#define BSP_EXT extern 
#endif 

unsigned int OS_CPU_SysTickClkFreq (void);


// LED Pin: PA8
#define LED_CLOCK                      RCC_APB2Periph_GPIOB
#define LED_BASE                       GPIOB
#define LED_CONFIG_PIN                 GPIO_Pin_5
#define LED_RELAY_PIN                  GPIO_Pin_12
  
////KEY Pin: PC13
//#define KEY_CLOCK              RCC_APB2Periph_GPIOB
//#define KEY_BASE               GPIOB
//#define KEY_PIN                GPIO_Pin_10
//#define KEY_INT_PORT           GPIO_PortSourceGPIOB
//#define KEY_INT_PIN            GPIO_PinSource10
//#define KEY_INT_LINE           EXTI_Line10
//#define KEY_INT_CHANNEL        EXTI15_10_IRQn  
  
	
#ifdef TEST_DEMO	
//PWM_32K PIN PB9
#define PWM_32K_CLOCK                  RCC_APB2Periph_GPIOB
#define PWM_32K_BASE                   GPIOB
#define PWM_32K_PIN                    GPIO_Pin_9
#else
//PWM_32K PIN PB0
#define PWM_32K_CLOCK                  RCC_APB2Periph_GPIOB
#define PWM_32K_BASE                   GPIOB
#define PWM_32K_PIN                    GPIO_Pin_0
#endif
 
#ifdef TEST_DEMO	
//EN Pin: PB0
#define CC3000_EN_CLOCK        RCC_APB2Periph_GPIOA
#define CC3000_EN_BASE         GPIOB
#define CC3000_EN_PIN          GPIO_Pin_0
#else
//EN Pin: PB1
#define CC3000_EN_CLOCK        RCC_APB2Periph_GPIOB
#define CC3000_EN_BASE         GPIOB
#define CC3000_EN_PIN          GPIO_Pin_6
#endif
#ifdef TEST_DEMO
//WL_EN Pin: PA15
#define WL_EN_CLOCK            RCC_APB2Periph_GPIOA
#define WL_EN_BASE             GPIOA
#define WL_EN_PIN              GPIO_Pin_15
#else
//WL_EN Pin: PA0
#define WL_EN_CLOCK            RCC_APB2Periph_GPIOA
#define WL_EN_BASE             GPIOA
#define WL_EN_PIN              GPIO_Pin_0

#endif
  

  
#ifdef  DEBUG_WU 
  //TX Pin: PA12
  #define UART_TX_CLOCK            RCC_APB2Periph_GPIOA
  #define UART_TX_BASE             GPIOA
  #define UART_TX_PIN              GPIO_Pin_2
    
  //WL_EN Pin: PA12
  #define UART_RX_CLOCK            RCC_APB2Periph_GPIOA
  #define UART_RX_BASE             GPIOA
  #define UART_RX_PIN              GPIO_Pin_3
 
  #define USART_CLOCK              RCC_APB1Periph_USART2  
  #define USART_BASE               USART2
  #define USART_INT_CHANNEL        USART2_IRQn  
  #define UART_IF_BUFFER           256
#endif    
  
  
  
  
extern void delay_ms(unsigned int ms);
extern void HW_IO_init(void);
extern void BSP_Init(void);
extern void Board_Init(void);
extern void SetLEDStatus(unsigned char status);
void SetLEDToggle(void);
void SetRELAYToggle(void);


#ifdef  DEBUG_WU 
extern void UARTConfigure(void);
extern void UartSendPacket(char *inBuff, unsigned short usLength); 
#endif 
 

#endif
