#ifndef __UART_H__
#define __UART_H__

typedef struct
{   
    char            buff[MAX_MESSAGE_LEN];
    bool            avail;
    uint8_t         pos;
} uart_rx_st;

typedef struct
{
    char            buff[MAX_MESSAGE_LEN];
} uart_tx_st;


typedef struct
{
    uart_rx_st      rx;
    uart_tx_st      tx;
} uart_msg_st;

/// @brief Clear rx available
/// @param  -
/// @return -
void uart_initialize(void);

/// @brief  Get pointer to module data
/// @param
/// @return data pointer
uart_msg_st *uart_get_data_ptr(void);


#endif