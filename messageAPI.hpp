/*********************************************************************
*
*   HEADER:
*       header file for messageAPI
*
*   Copyright 2024 Nate Lenze
*
*********************************************************************/
#ifndef MESSAGE_API_HPP
#define MESSAGE_API_HPP
/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h> 
#include <stdbool.h>

#include "sys_def.h"
#include "LoraAPI.hpp"

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
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_MSG_LENGTH ];      /* data buffer          */
    bool valid;                             /* data marked valid?   */
    } rx_message;

typedef struct                              /* tx message format    */
    {
    location destination;                   /* destination          */
    uint8_t size;                           /* size of message[]    */
    uint8_t message[ MAX_MSG_LENGTH ];      /* data buffer          */
    } tx_message;

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


typedef uint8_t message_errors;        /* Error Codes                */
enum 
    {
    MSG_NO_ERROR,                      /* NO RX error                */
    MSG_CRC_ERROR,                     /* RX CRC error               */
    MSG_INVALID_HEADER,                /* RX Invalid header          */
    MSG_ARRAY_SIZE_ERR,                /* message is too big for passed
                                         in array                    */
    MSG_DOUBLE,                        /* more than one message was 
                                         received at once            */
    MSG_SIZING,                        /* less than one message was 
                                         received at once            */
    MSG_KEY_ERR,                       /* Invalid key                */
    MSG_HW_ERROR                       /* Error in underlying HW 
                                          interface                  */
    }; 

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
                              CLASSES
--------------------------------------------------------------------*/
namespace core 
{

class messageInterface
    {
    public:
        messageInterface( core::loraInterface& l_ref, core::console& c_ref );

        ~messageInterface( void );

        bool send_message( tx_message message );

        bool get_message( rx_message *message, message_errors& errors );

        void update_key( uint8_t new_key );

    private:

        uint8_t calculate_crc( uint8_t message_array[], uint8_t size );

        lora_message covert_message( uint8_t message_array[], uint8_t size, message_errors& error_ptr );

        uint8_t p_current_key;

        bool p_init_successful;

        core::loraInterface& p_lora;
        
        core::console& p_console;
    };

} /* core namespace */

#endif