
#include "main.h"
#include <Arduino.h>
#include <Wire.h>
#include <RH_RF69.h>
#include <SPI.h>
#include "main.h"
#include "rfm69.h"
#include "rfm_receive.h"

extern RH_RF69 *rf69p;

rfm_receive_msg_st  receive_msg;


rfm_receive_msg_st *rfm_receive_get_data_ptr(void)
{
    return &receive_msg;
}

void rfm_receive_initialize()
{
    receive_msg.avail = false;
}


void rfm_receive_message(void)
{
    //rfm_receive_msg_st *tx_msg = &receive_msg;
    if (rf69p->available()) 
    {
        receive_msg.len = sizeof(receive_msg.radio_msg);
        if (rf69p->recv((uint8_t*)receive_msg.radio_msg, &receive_msg.len)) 
        {
            receive_msg.avail = true;
            if (receive_msg.len > 0)
            {   
                io_led_flash(LED_INDX_GREEN,20); 
                if (receive_msg.len >= MAX_MESSAGE_LEN) receive_msg.len = MAX_MESSAGE_LEN -1;
                receive_msg.radio_msg[receive_msg.len] = 0;
                Serial.println((char*)receive_msg.radio_msg);
                #ifdef DEBUG_PRINT
                Serial.print("Received [");Serial.print(receive_msg.len);Serial.print("]: ");
                Serial.println((char*)receive_msg.radio_msg);               
                Serial.print("len: ");
                Serial.print(receive_msg.len, DEC);
                Serial.print("  RSSI: ");
                Serial.println(rf69p->lastRssi(), DEC);
                #endif
                receive_msg.rssi = rf69p->lastRssi();

            }
        }
    }
}

bool rfm_receive_message_is_avail(void)
{
    bool  is_avail = receive_msg.avail;
    // receive_msg.avail = false;
    return  is_avail;
}

void rfm_receive_clr_message_flag(void)
{
    receive_msg.avail = false;
}


