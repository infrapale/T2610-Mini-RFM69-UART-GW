#include "main.h"
#include "uart.h"
#include "atask.h"
#include "rfm69.h"
#include "rfm_receive.h"
#include "rfm_send.h"
#include "io.h"

uart_msg_st         uart;
void uart_rx_task(void);
//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st uart_rx_th         =     { "RFM RX Task    ", 100,    0,     0,  255,    0,  1,  uart_rx_task };

uart_msg_st *uart_get_data_ptr(void)
{
    return &uart;
}

void uart_initialize(void)
{
    uart.rx.avail = false;
    atask_add_new(&uart_rx_th);
}

void uart_rx_task(void)
{
    static  char c;
    switch(uart_rx_th.state)
    {
        case 0:
            uart_rx_th.state = 5;
            break;
        case 5:
            uart.rx.pos = 0;
            uart.rx.avail = false;
            uart_rx_th.state = 10;
            break;
        case 10:
            while (Serial.available()) {
                c = Serial.read();
                //Serial.printf("@%d-%c\n",rfm_pos,c);
                if (c == '\n' || c == '\r') {
                    if (uart.rx.pos > 0) {
                        uart.rx.buff[uart.rx.pos] = 0;
                        uart.rx.avail = true;   // signal to msg_task()
                        uart_rx_th.state = 20;
                        //Serial.println(uart.rx.buff);
                    }
                } else {
                    if (uart.rx.pos < MAX_MESSAGE_LEN- 1) {
                        uart.rx.buff[uart.rx.pos++] = c;
                    } else {
                        // overflow protection
                        uart.rx.buff[MAX_MESSAGE_LEN - 1] = 0;
                        uart_rx_th.state = 100;
                    }
                }
            }
            break;
        case 20:
            Serial.println(uart.rx.buff); Serial.flush();
            rfm_send_radiate_msg( uart.rx.buff );
            uart_rx_th.state = 5;
            break;
        case 100:
            uart.rx.buff[MAX_MESSAGE_LEN - 1] = 0;
            Serial.print("RX buffer limit:");
            uart_rx_th.state = 20;
            break;
        case 200:
            uart_rx_th.state = 10;
            break;
    }



}

void uart_rx_send_rfm_from_raw(void)
{  
    rfm_send_radiate_msg(uart.rx.buff);
}


