/*********************************************************************
*
*   HEADER:
*       system defintions for configurable network. All modules must
*       be defined within this file in order to messageAPI to work.
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

#include "LoRa/LoraAPI.h"

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
    EXAMPLE1_MODULE,               /* example module (ADD HERE)     */
    NUM_OF_MODULES,                /* number of modules             */
    MODULE_NONE,                   /* no module                     */
    INVALID_LOCATION               /* invalid module address        */
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
                              PROCEDURES
--------------------------------------------------------------------*/

/* sys_def.h */
