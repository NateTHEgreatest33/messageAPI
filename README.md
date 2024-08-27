# messageAPI
messaging API built ontop of the loRa API -- Version 2.0


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

For the most up to date documentation please refer to the [Message API ICD](https://docs.google.com/document/d/130JMIxLKgSH4ArH4278pt9nv2rzlUnXZ5191O4RaR6M/edit#heading=h.vb53gr77h2fj)

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
2. current_location must be defined within project side files and is used to parse out only messages intended for the current module.
```
const location current_location = EXAMPLE_MODULE1;
```
3. The LoRa API needs to be configured prior to seting up MessageAPI

4. The data types used for transfer are of rx_message and tx_message type
> :warning: **Version 2.0 update **: rx message format was updated in version 2.0 to allow for multi-message rx per transaction. The single rx message format is now depricated!
```
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

typedef struct 
    {
    std::array<rx_message, MAX_MSG_RX> messages; /* rx messages     */
    uint8_t num_messages;                        /* number of msgs  */
    std::array<message_errors, MAX_MSG_RX> errors; /* errors/msg    */
    message_errors global_errors;                /* global errors   */
    } rx_multi;
```


__Usage:__

1. Before any communication can occour, the LoRa transciver must be setup to receive messages. this is done via the constructor of loraAPI
```
core::loraInterface loRa( spi_default ,console );
core::messageInterface messageAPI( loRa, console );
```

2. To send a message use send_message() which takes in a tx_message and returns T/F if it was successful
```
bool core::messageInterface::send_message
    (
    tx_message message                           /* message to send */
    );
```
3. To check and receive a message use get_multi_message(). this returns an object of type rx_multi which has a variabkle num_messages to determine how many messages were rx'ed.
```
rx_multi core::messageInterface::get_multi_message
    (
    void
    );
```

__Additional Notes:__

1. messageAPI conatins built in crc checking and updating but does not automate updating the message key (byte 4). the function update_key is provided to update the key which is compared agasnt incoming messages.
```
void core::messageInterface::update_key
    (
    uint8_t new_key                                      /* new key */
    );
```
