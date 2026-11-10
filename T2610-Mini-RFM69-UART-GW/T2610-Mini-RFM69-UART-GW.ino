/**
T2610-Mini-RFM69-UART-GW
HW: Adafruit M0 RFM69 Feather or Arduino Pro Mini + RFM69

Send and receive data via UART

*******************************************************************************
https://github.com/infrapale/T2610-Mini-RFM69-UART-GW.git
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all

*******************************************************************************

*******************************************************************************
**/

#include <Arduino.h>
#include "main.h"
#include "avr_watchdog.h"
#include "secrets.h"
#include <RH_RF69.h>
#include <VillaAstridCommon.h>
#include "atask.h"
#include "rfm69.h"
#include "uart.h"
#include "rfm_receive.h"
#include "rfm_send.h"
#include "io.h"

#define ZONE  "OD_1"
//*********************************************************************************************
#define SERIAL_BAUD   9600
#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h


RH_RF69         rf69(RFM69_CS, RFM69_INT);
RH_RF69         *rf69p;
time_type       MyTime = {2023, 11,01,1,01,55}; 

#define NBR_TEST_MSG  4
#define LEN_TEST_MSG  60
const char test_msg[NBR_TEST_MSG][LEN_TEST_MSG] =
{  //12345678901234567890123456789012
    "<S;#;PIHA1;T;-12.3;H;44;L;2344>",
    "<S;#;Dock;T;22.1;W;10.2>",
    "<R;MH1;KOK1;1;KOK2;0;KOK3;1>",
    "<R;TK1;TUP1;1;TUP2;1>"
};

void debug_print_task(void);
void run_100ms(void);
void send_test_data_task(void);
void rfm_receive_task(void); 


atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};
atask_st clock_handle              = {"Tick Task      ", 100,0, 0, 255, 0, 1, run_100ms};
atask_st rfm_receive_handle        = {"Receive <- RFM ", 100,0, 0, 255, 0, 1, rfm_receive_task};

#ifdef SEND_TEST_MSG
atask_st send_test_data_handle     = {"Send Test Task ", 10000,0, 0, 255, 0, 1, send_test_data_task};
#endif

#ifdef PRO_MINI_RFM69
AVR_Watchdog watchdog(4);
#endif

rfm_receive_msg_st  *receive_p;
rfm_send_msg_st     *send_p;
uart_msg_st         *uart_p;



void initialize_tasks(void)
{
  atask_initialize();
  // atask_add_new(&debug_print_handle);
  atask_add_new(&clock_handle);
  atask_add_new(&rfm_receive_handle);

  #ifdef SEND_TEST_MSG
  atask_add_new(&send_test_data_handle);
  #endif
  Serial.print("Tasks initialized ");
}


void setup() 
{
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer
    delay(2000);
    Serial.begin(9600);
  

    Serial.print("T2610-Mini-RFM69-UART-GW"); Serial.print(" Compiled: ");
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.println();

    
    uart_p = uart_get_data_ptr();
    send_p = rfm_send_get_data_ptr();

    rf69p = &rf69;
    rfm69_initialize(&rf69);
    rfm_receive_initialize();
    io_initialize();
    // Hard Reset the RFM module
    
    initialize_tasks();
    uart_initialize();

    #ifdef PRO_MINI_RFM69
    watchdog.set_timeout(4);
    #endif
}



void loop() 
{
    atask_run();  
}


void rfm_receive_task(void) 
{
    rfm_receive_message();
    #ifdef PRO_MINI_RFM69
    watchdog.clear();
    #endif
}


void run_100ms(void)
{
    static uint8_t ms100 = 0;
    if (++ms100 >= 10 )
    {
        ms100 = 0;
        if (++MyTime.second > 59 )
        {
          MyTime.second = 0;
          if (++MyTime.minute > 59 )
          {    
            MyTime.minute = 0;
            if (++MyTime.hour > 23)
            {
                MyTime.hour = 0;
            }
          }   
      }
    }
    io_run_100ms();
}

void debug_print_task(void)
{
  atask_print_status(true);
}

#ifdef SEND_TEST_MSG
void send_test_data_task(void)
{
    if  (send_test_data_handle.state >= NBR_TEST_MSG ) send_test_data_handle.state = 0;

    //uart_p->tx.str  = test_msg[send_test_data_handle.state];
    rfm_send_radiate_msg(test_msg[send_test_data_handle.state]);
    Serial.println(test_msg[send_test_data_handle.state]);
    //uart_p->rx.avail = true;
    send_test_data_handle.state++;
}
#endif

