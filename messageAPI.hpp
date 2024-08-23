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

#define MAX_MSG_RX ( 10 ) /* min message size = 6 bytes, fifo size = 64 
                             thus, the maxium msg's in fifo is 10.6 or 
                             rounded to 10                           */
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


typedef struct {
    std::array<MAX_MSG_RX, rx_message> messages;
    uint8_t num_messages;
    std::array<MAX_MSG_RX, message_errors> errors;
    message_errors global_errors;
} rx_multi;

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

typedef struct 
{
std::array<MAX_MSG_RX, uint8_t> start_idx;
std::array<MAX_MSG_RX, uint8_t> end_idx;
std::array<MAX_MSG_RX, uint8_t> message_size;
uint8_t num_msg;
} multi_msg_parser;
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

        rx_multi get_multi_message( void );

        void update_key( uint8_t new_key );

    private:

        uint8_t calculate_crc( uint8_t message_array[], uint8_t size );

        lora_message covert_message( uint8_t message_array[], uint8_t size, message_errors& error_ptr );

        multi_msg_parser lora_prepper( uint8_t message_array[], uint8_t size );

        uint8_t p_current_key;

        bool p_init_successful;

        core::loraInterface& p_lora;
        
        core::console& p_console;
    };

} /* core namespace */

#endif