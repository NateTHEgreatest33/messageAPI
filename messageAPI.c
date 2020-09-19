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

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
#define API_VERSION         ( 1 )       /* message API v1                  */

#define MINIMUM_MSG_LENGTH  ( 6 )       /* minium size of empty message    */

#define MAX_MSG_LENGTH      ( 10 )      /* maximum size of message         */

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
static uint8_t current_key; 
/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/

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
memset( return_msg, 0, sizeof( return_msg ) );
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
*       x
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
/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
return size;
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
memset( return_message, 0, sizeof( return_message ) );
return_message_size     = 0;
return_message_errors   = RX_TIMEOUT;

/*----------------------------------------------------------
Check is message has been received, if not exit
----------------------------------------------------------*/
if( lora_get_message( &return_message, MAX_LORA_MSG_SIZE, &return_message_size, &return_message_errors ) )
    {
    /*----------------------------------------------------------
    Update errors
    ----------------------------------------------------------*/
    *errors = return_message_errors;

    /*----------------------------------------------------------
    Convert message
    ----------------------------------------------------------*/
    formatted_array = covert_message( return_message, return_message_size, &return_message_errors);

    /*----------------------------------------------------------
    Verify message
    ----------------------------------------------------------*/
    if ( *errors == RX_NO_ERROR )
        {
        /*----------------------------------------------------------
        Calculate and verify CRC and key
        ----------------------------------------------------------*/
        if ( formatted_array.crc != calculate_crc( &return_message, ( formatted_array.size + HEADER_BYTE_COUNT ) ) )
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
        message->destination    = ( location ) formatted_array.destination;
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
        message->source    = ( location ) formatted_array.source;
        }
    /*----------------------------------------------------------
    Verify key
    ----------------------------------------------------------*/
    if ( current_key != formatted_array.key )
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
uint8_t message_array[ MAX_LORA_MSG_SIZE ];
lora_errors errors;
uint8_t i;
uint8_t array_size;
/*----------------------------------------------------------
Initilize local variables
----------------------------------------------------------*/
memset( message_array, 0, sizeof( message_array ) );
errors = RX_NO_ERROR;
i = 0;
array_size = 0;

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
in size, we pass in array_size - 1
----------------------------------------------------------*/
array_size = message.size + MINIMUM_MSG_LENGTH;

message_array[ array_size - 1 ] = calculate_crc( message_array, ( array_size - 1 ) );

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
    void
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