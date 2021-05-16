/*********************************************************************
*
*   HEADER:
*       header file for messageAPI
*
*   Copyright 2020 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h> 
#include <stdbool.h>

#include "sys_def.h"
#include "LoRa/LoraAPI.h"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
#define MAX_MSG_LENGTH      ( 10 )      /* maximum size of message  */

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef struct                              /* rx message format    */
    {
    location source;                        /* source               */
    location destination;                   /* destination          */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_MSG_LENGTH ];      /* data buffer          */
    bool valid;                             /* data marked valid?   */
    } rx_message;

typedef struct                              /* tx message format    */
    {
    location source;                        /* source               */
    location destination;                   /* destination          */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_MSG_LENGTH ];      /* data buffer          */
    } tx_message;

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
messageAPI.c
--------------------------------------------------------------------*/
lora_errors send_message
    (
    tx_message message                           /* message to send */
    );

bool get_message
    (
    rx_message *message,       /* pointer to store message received */
    lora_errors *errors        /* pointer to store errors received  */
    );

lora_errors init_message
    (
    lora_config config_data                  /* SPI Interface info  */
    );

void update_key
    (
    uint8_t new_key                                      /* new key */
    );
/* messageAPI.h */
