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
#include <stdbool.h>

#include "Lora/LoraAPI.h"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef uint8_t location;          /* source/destination data       */
enum 
    {
    MODULE_NONE                    /* no module                     */
    }; 

typedef struct                              /* rx message format    */
    {
    location source;                        /* source               */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_LORA_MSG_SIZE ];   /* data buffer          */
    } rx_message;

typedef struct                              /* tx message format    */
    {
    location destination;                   /* destination          */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_LORA_MSG_SIZE ];   /* data buffer          */
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
    lora_errors *errros        /* pointer to store errors received  */
    );

/* messageAPI.h */
