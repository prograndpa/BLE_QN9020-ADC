/****************************************************************************
 *   $Id:: adc_example.c                                                   $
 *   Project: NXP QN9020 ADC example
 *
 *   Description:
 *     This file contains ADC driver usage.
 *
****************************************************************************/

#include "system.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"
#include "analog.h"
#include "uart.h"
#include "stdio.h"


/****************************************************************************
*               ADC driver code example
*ADC support 4 work mode:
*  Differential with input buffer: input signal range [0.2, VDD-0.2], 
*                                  measure range [-VREF, VREF) map to [-2048, 2047].
*  Differential without input buffer: input signal range [0, VDD], 
*                                     and should have driving capability, 
*                                     measure range [-VREF, VREF) map to [-2048, 2047].
*  Single-End with input buffer: input signal 0.2 =< VIN(V) <= 1.5*VREF <= VDD-0.2, 
*                                measure range [0.2, 1.5*VREF) map to [x,2047].
*  Single-End without input buffer: input signal 0 =< VIN(V) <= VREF <= VDD, 
*                                   and should have driving capability, 
*                                   measure range [0, VREF) map to [0,2047].
*
*ADC reference voltage support 3 types:
*  Internal: 1V (calibration value stored in NVDS)
*  External REF1(with buffer and gain=2): VREF = 2*EXT_REF1 (0 < EXT_REF1 < (VDD-1.0)/2)
*  External REF2(without buffer): VERF = EXT_REF2 (0 < EXT_REF2 < VDD), EXT_REF2 should have driving capability
*
*ADC resolution support: 12/10/8 bits
*
*ADC conversion can be triggered by TIMER overflow/GPIO/software, continue conversion mode only need trigger once. 
*
****************************************************************************/
volatile uint32_t adc_done = 0;
int16_t buf[512];
adc_read_configuration read_cfg;

static void adc_test_cb(void)
{
    adc_done = 1;
    //printf("adc_test_cb\n");
}
static void timer_test_cb(void)
{

    adc_done = 0;
	int sum = 0;
    //printf("timer_test_cb\n");
    // modify here
    read_cfg.trig_src = ADC_TRIG_SOFT;
    read_cfg.mode = SINGLE_MOD;
    read_cfg.start_ch = AIN0;
    read_cfg.end_ch = AIN0;
    adc_read(&read_cfg, buf, 512, adc_test_cb);
    while (adc_done == 0);
    for (int i = 0; i < 10; i++) {
        sum += buf[511 - 2*i];
    }
    sum = sum / 10;
    printf("average: %d\t %d\r\n", sum, ADC_RESULT_mV(sum));
	
}

int main (void)
{

    SystemInit();

    adc_pin_enable(AIN0, MASK_ENABLE);
    
    uart_init(QN_UART0, __USART_CLK, UART_115200);
    uart_tx_enable(QN_UART0, MASK_ENABLE);
    
    adc_init(ADC_SINGLE_WITHOUT_BUF_DRV, ADC_CLK_1000000, ADC_INT_REF, ADC_12BIT);

    timer_init(QN_TIMER1, timer_test_cb);
	timer_config(QN_TIMER1,TIMER_PSCAL_DIV,TIMER_COUNT_S(2,TIMER_PSCAL_DIV));
    timer_enable(QN_TIMER1, MASK_ENABLE);
	
    while (1)                                /* Loop forever */
    {


    }
}

