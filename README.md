#TODO LIST:
- [x] port orginal code
- [ ] get verified working
- [ ] create stable v1.0
- [ ] python rPi companion framework
- [ ] module test framework
- [ ] system test framework using python test bench
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
2. LoRa Pin and SPI setup defintions can be found and modified in loRaAPI.c
```
#define SPI_SELECTED            ( SSI0_BASE )          /* SPI 0 selected    */
#define DATA_PIN                ( 0x08 )               /* pin 3             */
#define DATA_PORT_GROUP         ( GPIO_PORTA_DATA_R )  /* pin register A    */
```



__Usage:__

1. Before any communication can occour, the LoRa transciver must be setup to receive messages. this is done using the init function:
```
init_message()
```

2. To send a message use send_message()
```
lora_errors send_message
    (
    tx_message message                           /* message to send */
    );
```
3. To check and receive a message use get_message()
```
bool get_message
    (
    rx_message *message,       /* pointer to store message received */
    lora_errors *errors        /* pointer to store errors received  */
    );
```
