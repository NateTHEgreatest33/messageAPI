### TODO LIST:
- [x] port orginal code
- [x] REMOVED: INST parser and # of messages return instead of T/F
- [x] create better way of defining modules? maybe an add module function to create a dynamic array of modules?
- [x] LoRa read/write working
- [x] LoRa TX/RX working
- [x] get verified working
- [x] create stable v1.0
- [ ] implement current module into code + remove src/dest from tx/rx messages
- [x] python rPi companion framework
- [ ] module test framework
- [x] system test framework using python test bench
- [ ] implement v1.1

# messageAPI
messaging API built ontop of the loRa API


Message format:
Byte 0 | Byte 1 |Byte 2 |Byte 3 |Byte 4 |Byte 5 |Byte 6 |Byte 7 |Byte 8 |Byte 9 |Byte 10 |Byte 11 |Byte 12 |Byte 13 |Byte 14 |Byte 15 |
------------ | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | -------------
Destination | Source | pad (future updates) | version/ data size | key | data | data | data | data | data | data | data | data | data | data | CRC

* Destination: destination module where packet has been sent from
* Source: source module where packet has been sent from
* Pad: Added room for future updates
* Version/Data Size:
  * Version: version of message API. Helps reciving end know how to interpt packet
  * Data Size: size of data bytes (can range from 0 to 10)
* Key: Key value. all keys initially start at 0x00 and can be manually updated via update_key(). if a message received has a missmatched key it is marked as invalid.
* Data: data transmitted
* CRC: crc8 caculated via byte 0 to the last data byte


How to use the message API:

__Setup:__

1. Modules must be defined with the location enum as follows:
```
typedef uint8_t location;          /* source/destination data       */   
enum 
    {
    EXAMPLE_MODULE1,               /* example module 1              */
    EXAMPLE_MODULE2,               /* example module 2              */
    NUM_OF_MODULES,                /* number of modules             */
    MODULE_NONE,                   /* no module                     */
    INVALID_LOCATION               /* invalid module address        */
    }; 
```
2. LoRa Pin and SPI setup defintions are setup using the type lora_config
```
typedef struct 
    {
    uint32_t SSI_BASE;                    /* SPI interface selected */
    CS_port SSI_PORT;                     /* SPI pin selected       */
    uint8_t  SSI_PIN;                     /* PI port selected       */             
    } lora_config;                        /* SPI interface info     */
```
3. The data types used for transfer are of rx_message and tx_message type
```
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
```


__Usage:__

1. Before any communication can occour, the LoRa transciver must be setup to receive messages. this is done using the init function. The data passed in is of lora_config type and this function returns data type lora_errors
```
lora_errors init_message
    (
    lora_config config_data                  /* SPI Interface info  */
    );
```

2. To send a message use send_message() which takes in a tx_message and returns an error variable of type lora_errors
```
lora_errors send_message
    (
    tx_message message                           /* message to send */
    );
```
3. To check and receive a message use get_message(). this returns a boolean true or false depending if a message has been recived. if true, the message will be placed into the providied rx_message variable. The errors variable can be updated even if no message is recived (ie. issues w/ SPI or message sizing).
```
bool get_message
    (
    rx_message *message,       /* pointer to store message received */
    lora_errors *errors        /* pointer to store errors received  */
    );
```

__Additional Notes:__

1. messageAPI conatins built in crc checking and updating but does not automate updating the message key (byte 4). the function update_key is provided to update the key which is compared agasnt incoming messages.
```
void update_key
    (
    uint8_t new_key                                      /* new key */
    );
```
