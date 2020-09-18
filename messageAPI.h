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

#include "Lora/LoraAPI.h"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef uint8_t location;          /* source/destination data. add       
                                      new modules to begining of
                                      enum, as code and # of enum
                                      defines rely on this          */
enum 
    {
    NUM_OF_MODULES,                /* number of modules             */
    MODULE_NONE,                   /* no module                     */
    INVALID_LOCATION               /* invalid module address        */
    }; 

typedef struct                              /* rx message format    */
    {
    location source;                        /* source               */
    location destination;                   /* destination          */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_LORA_MSG_SIZE ];   /* data buffer          */
    bool valid;                             /* data marked valid?   */
    } rx_message;

typedef struct                              /* tx message format    */
    {
    location source;                        /* source               */
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
    lora_errors *errors        /* pointer to store errors received  */
    );

lora_errors init_message
    (
    void
    );

/* messageAPI.h */
