/*********************************************************************
*
*   NAME:
*       messageAPI.c
*
*   DESCRIPTION:
*       custom messaging API for messaging between different lora devices
*
*   Copyright 2020 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/

#include "messageAPI.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
#define API_VERSION         ( 1 )       /* message API v1                  */

#define MAXIMUM_MSG_LENGTH  ( 10 )      /* maximum size of message data    */

#define MINIMUM_MSG_LENGTH  ( 6 )       /* minium size of empty message    */

#define DESTINATION_BYTE    ( 0 )       /* destination byte array index    */

#define SOURCE_BYTE         ( 1 )       /* source byte array index         */
 
#define VERSION_BYTE        ( 3 )       /* version byte array index        */

#define VERSION_MASK        ( 0xF0 )    /* version byte mask               */

#define SIZE_BYTE           ( 3 )       /* size byte array index           */

#define SIZE_MASK           ( 0x0F )    /* size byte mask                  */

#define KEY_BYTE            ( 4 )       /* key byte array index            */

#define DATA_START_BYTE     ( 5 )       /* data byte(s) array start index  */

#define HEADER_BYTE_COUNT   ( 5 )       /* count of non CRC header bytes   */

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

typedef struct                              /* lora message format  */
    {
    location source;                        /* source               */
    location destination;                   /* destination          */
    uint8_t pad;                            /* future update space  */
    uint8_t version;                        /* version of API used  */
    uint8_t size;                           /* size of message[]    */
    uint8_t key;                            /* key                  */
    uint8_t message[ MAX_LORA_MSG_SIZE ];   /* data buffer          */
    uint8_t crc;                            /* crc                  */
    } lora_message;

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/
/*----------------------------------------------------------
This CRC lookup table is for polynomal 0x7
----------------------------------------------------------*/
static uint8_t const crc8_table[] = {
0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF};

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/
static uint8_t current_key; 
/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/
uint8_t calculate_crc
    (
    uint8_t message_array[],  /* message array to caclulate crc       */
    uint8_t size              /* size of message_array                */
    );

lora_message covert_message
    (
    uint8_t message_array[],   /* message array to caclulate crc       */
    uint8_t size,              /* size of message_array                */
    lora_errors *error_ptr     /* pointer to error variable            */
    );

/*********************************************************************
*
*   PROCEDURE NAME:
*       covert_message
*
*   DESCRIPTION:
*       convert message from array to rx_message type
*
*********************************************************************/
lora_message covert_message
    (
    uint8_t message_array[],
    uint8_t size,
    lora_errors *error_ptr
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
lora_message return_msg;
uint8_t crc_byte_index;
uint8_t i;

/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
memset( &return_msg, 0, sizeof( return_msg ) );
crc_byte_index = 0;
i = 0;

/*----------------------------------------------------------
Check for less than one message
----------------------------------------------------------*/
if ( size > MINIMUM_MSG_LENGTH )
    {
    /*----------------------------------------------------------
    Convert header data
    Byte 0 -- destination byte
    Byte 1 -- source byte
    Byte 2 -- pad (future expantion)
    Byte 3 -- version/size byte (upper/lower bits)
    Byte 4 -- key byte
    Byte 5 -- start of data region
    Byte X -- crc (last byte) 
    ----------------------------------------------------------*/
    return_msg.destination  = message_array[ DESTINATION_BYTE ];
    return_msg.source       = message_array[ SOURCE_BYTE ];
    return_msg.version      = ( message_array[ VERSION_BYTE ] & VERSION_MASK ) >> 4;
    return_msg.size         = ( message_array[ SIZE_BYTE ] & SIZE_MASK );
    return_msg.key          = message_array[ KEY_BYTE ];

    /*----------------------------------------------------------
    Issue with message size variable
    ----------------------------------------------------------*/
    if( return_msg.size > MAXIMUM_MSG_LENGTH )
        {
        *error_ptr = RX_INVALID_HEADER;
        return return_msg;
        }
    else
        {
        /*----------------------------------------------------------
        Retrive crc byte from end
        ----------------------------------------------------------*/
        crc_byte_index = return_msg.size + DATA_START_BYTE;
        return_msg.crc = message_array[ crc_byte_index ];

        /*----------------------------------------------------------
        Retrive message
        ----------------------------------------------------------*/
        for ( i = 0; i < return_msg.size; i++ )
            {
            return_msg.message[ i ] = message_array[ i + DATA_START_BYTE ];
            }
        /*----------------------------------------------------------
        Check for more than one message by using size and verifying
        all bytes were looked at
        ----------------------------------------------------------*/
        if ( crc_byte_index != ( size - 1 ) )
            {
            *error_ptr = RX_DOUBLE;
            }

        return return_msg;
        }
    }
else
    {
    *error_ptr = RX_SIZING;
    return return_msg;

    }
} /* covert_message() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       calculate_crc
*
*   DESCRIPTION:
*       caculates crc8 based on inputted message array
*
*********************************************************************/
uint8_t calculate_crc
    (
    uint8_t message_array[],
    uint8_t size
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
uint8_t crc;                                 /* crc value */
uint8_t i;                                   /* iterator  */

/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
crc = 0;
i   = 0;

/*----------------------------------------------------------
Calculate crc
----------------------------------------------------------*/
for( i = 0; i < size; i++ )
    {
    crc = crc8_table[ crc ^ message_array[i] ];
    }

return crc;

} /* calculate_crc() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       get_message
*
*   DESCRIPTION:
*       procedure for receiving messages in messageAPI format 
*       through LoRa
*
*   RETURN:
*       T/F message received y/n
*
*********************************************************************/
bool get_message
    (
    rx_message *message,       /* pointer to store message received */
    lora_errors *errors        /* pointer to store errors received  */
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
uint8_t return_message[ MAX_LORA_MSG_SIZE ]; /* array holding return message */
uint8_t return_message_size;                 /* size of return message array */
lora_errors return_message_errors;           /* errors from lora comm layer  */
lora_message formatted_array;                /* message array formated       */   

/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
memset( &return_message, 0, sizeof( return_message ) );
return_message_size     = 0;
return_message_errors   = RX_TIMEOUT;

/*----------------------------------------------------------
Check is message has been received, if not exit
----------------------------------------------------------*/
if( lora_get_message( return_message, MAX_LORA_MSG_SIZE, &return_message_size, &return_message_errors ) )
    {
    /*----------------------------------------------------------
    if issues with lora_get_message, update global error
    and return false
    ----------------------------------------------------------*/
    if ( return_message_errors != RX_NO_ERROR )
        {
        *errors = return_message_errors;
        return false;
        }

    /*----------------------------------------------------------
    Convert message
    ----------------------------------------------------------*/
    formatted_array = covert_message( return_message, return_message_size, &return_message_errors);

    /*----------------------------------------------------------
    Update errors
    ----------------------------------------------------------*/
    *errors = return_message_errors;

    /*----------------------------------------------------------
    Verify message
    ----------------------------------------------------------*/
    if ( *errors == RX_NO_ERROR )
        {
        /*----------------------------------------------------------
        Calculate and verify CRC and key
        ----------------------------------------------------------*/
        if ( formatted_array.crc != calculate_crc( return_message, ( formatted_array.size + HEADER_BYTE_COUNT ) ) )
            {
            message->valid = false;
            *errors = RX_CRC_ERROR;
            }
        else if ( formatted_array.key != current_key )
            {
            message->valid = false;
            *errors = RX_KEY_ERR;
            }
        }
    else
        {
        /*----------------------------------------------------------
        Since errors were detected, mark as invalid
        ----------------------------------------------------------*/
        message->valid = false;
        }
    
    /*----------------------------------------------------------
    Update rx_message
    ----------------------------------------------------------*/
    message->size           = formatted_array.size;
    memcpy( message->message, formatted_array.message, formatted_array.size );

    /*----------------------------------------------------------
    Verify destination location
    ----------------------------------------------------------*/
    if( formatted_array.destination >= NUM_OF_MODULES )
        {
        message->destination = INVALID_LOCATION;
        }
    else
        {
        message->destination = ( location ) formatted_array.destination;
        }
    
    /*----------------------------------------------------------
    Verify source location
    ----------------------------------------------------------*/
    if( formatted_array.source >= NUM_OF_MODULES )
        {
        message->source = INVALID_LOCATION;
        }
    else
        {
        message->source = ( location ) formatted_array.source;
        }
    /*----------------------------------------------------------
    Verify key
    ----------------------------------------------------------*/
    if ( current_key != formatted_array.key && *errors == RX_NO_ERROR )
        {
        *errors = RX_KEY_ERR;
        }

    /*----------------------------------------------------------
    Set return value true
    ----------------------------------------------------------*/
    return true;
    }
else
    {
    /*----------------------------------------------------------
    Set return value false
    ----------------------------------------------------------*/
    return false;

    }

} /* get_message() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       send_message
*
*   DESCRIPTION:
*       procedure for sending messages in messageAPI format 
*       through LoRa
*
*********************************************************************/
lora_errors send_message
    (
    tx_message message                           /* message to send */
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
uint8_t message_array[ MAX_LORA_MSG_SIZE ];     /* array to send through LoRa */
lora_errors errors;                             /* lora related errors        */
uint8_t i;                                      /* interator                  */
uint8_t array_size;                             /* size of message_array[]    */
/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
errors      = RX_NO_ERROR;
i           = 0;
array_size  = 0;
memset( message_array, 0, sizeof( message_array ) );

/*----------------------------------------------------------
Verify message size
----------------------------------------------------------*/
if( message.size > MAX_MSG_LENGTH )
    {
    return RX_ARRAY_SIZE_ERR;
    }

/*----------------------------------------------------------
Convert tx_message to array

Byte 0 -- destination byte
Byte 1 -- source byte
Byte 2 -- pad (future expantion)
Byte 3 -- version/size byte (upper/lower bits)
Byte 4 -- key byte
Byte 5 -- start of data region
Byte X -- crc (last byte) 
----------------------------------------------------------*/
message_array[ DESTINATION_BYTE ] = ( uint8_t ) message.destination;
message_array[ SOURCE_BYTE ] = ( uint8_t ) message.source;
message_array[ SIZE_BYTE ] = ( API_VERSION << 4 ) + message.size;
message_array[ KEY_BYTE ] = current_key;

for( i = 0; i < message.size; i++ )
    {
    message_array[ i + DATA_START_BYTE ] = message.message[ i ];
    
    }

/*----------------------------------------------------------
Calulate CRC and put in last byte of array

we do not use crc byte in crc caculation so when passing
in size, we pass in ( array_size + HEADER BYTE COUNT )
----------------------------------------------------------*/
array_size = message.size + MINIMUM_MSG_LENGTH;

message_array[ array_size - 1 ] = calculate_crc( message_array, ( message.size + HEADER_BYTE_COUNT) );

/*----------------------------------------------------------
Send message
----------------------------------------------------------*/
errors = lora_send_message(message_array, array_size );

/*----------------------------------------------------------
Revert to rx continious mode
----------------------------------------------------------*/
if( ! lora_init_continious_rx() )
    {
    errors = RX_INIT_ERR;
    }

/*----------------------------------------------------------
return errors
----------------------------------------------------------*/
return errors;

} /* send_message() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       init_message
*
*   DESCRIPTION:
*       procedure for setting up message API
*
*********************************************************************/
lora_errors init_message
    (
    lora_config config_data                  /* SPI Interface info  */
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
lora_errors init_errors;

/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
init_errors = RX_NO_ERROR;

/*----------------------------------------------------------
Initilize static variables
----------------------------------------------------------*/
current_key = 0x00;

/*----------------------------------------------------------
Initilize port statics
----------------------------------------------------------*/
lora_port_init( config_data );

/*----------------------------------------------------------
Put into rx mode
----------------------------------------------------------*/
if( ! lora_init_continious_rx() )
    {
    init_errors = RX_INIT_ERR;
    }

return init_errors;

} /* init_message() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       update_key 
*
*   DESCRIPTION:
*       procedure for updating key used in messageAPI
*
*********************************************************************/
void update_key
    (
    uint8_t new_key                                      /* new key */
    )
{

current_key = new_key;

} /* update_key() */
