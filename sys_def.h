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

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef uint8_t location;          /* source/destination data. add       
                                      new modules to begining of
                                      enum, as code uses 
                                      'NUM_OF_MODULES' defined 
                                      below                         */
enum 
    {
    RPI_MODULE,                    /* raspberry pi module           */
    TIVA_MODULE,                   /* tiva launchpad module         */
    PICO_MODULE,                   /* pi pico module                */
    NUM_OF_MODULES,                /* number of modules             */
    
    MODULE_ALL,                    /* all modules                   */                   
    MODULE_NONE,                   /* no module                     */
    INVALID_LOCATION               /* invalid module address        */
    }; 


/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/
extern const location current_location;   /* current module needs to be
                                         defined within project files
                                         & is based on location 
                                         enum                       */

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
