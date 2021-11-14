Basic Publish Subscribe Protocol (BPSP)
=================

# Structure of an BPSP Frame

## Overview
```
                                        BPSP Specification                                                
                                                                                                          
                                                                                                          
                                    0      1      2      3                                                
                                +--------------+-------+------+ -+                                        
                             0  |VARIABLES SIZE|OPCODE | FLAG |  |                                        
                                ---------------+-------+-------  |  Fixed Header (8) bytes                
                             4  |         DATA SIZE           |  |                                        
                                -------------------------------  |                                        
                          8...  |      VARIABLE HEADERS       |  |  Variable Header (VARIABLES SIZE) bytes
                                ------------------------------- -|                                        
                                |                             |  |                                        
                                |                             |  |                                        
                                |                             |  |                                        
                                |                             |  |                                        
 8 + nbytes(VARIABLES SIZE)...  |            DATA             |  |  Payload/Body (DATA SIZE) bytes        
                                |                             |  |                                        
                                |                             |  |                                        
                                |                             |  |                                        
                                |                             |  |                                        
                                |                             |  |                                        
                                +-----------------------------+ -+
```

## Data representation

### Bits

`0` `1`

### One Byte Integer

`uint8_t`

### Two Byte Integer

`uint16_t` represent in Big Endian

### Four Byte Integer

`uint32_t` represent in Big Endian

### Variable Byte Integer
> Deprecated

`uint64_t`

### Character

`char`

## BSPS Frame Format

### Fixed Header

Each BSPS Frame contains a Fixed Header as show below:
```
                                    0      1      2      3                                                
                                +--------------+-------+------+ -+                                        
                             0  |VARIABLES SIZE|OPCODE | FLAG |  |                                        
                                ---------------+-------+-------  |  Fixed Header (8) bytes                
                             4  |         DATA SIZE           |  |                                        
```

#### Variables Size

The Variables size is the Two Byte Integer (Unsigned value) that represents the size in byte of variable header.

#### Opcode

The Opcode is the One Byte Integer (Unsigned value) that represents the Operation (Command) of the frame.

#### FLAG

The Flag is the One Byte Integer with each bits represents the flag for the appropriate Operation.

#### Data Size

The Data Size is the Four Byte Integer (Unsigned value) that represents the size of payload/body, the number of bytes is limited at 2^32 bytes = 4 Gb for now, but we can increase in the future if need.

### Variable Header

The Variable Header is the key-value string that indicate the extension of the frame, each header key-value pair is represents as `"key""value";("key""value";(...))`.

For example the Authentication Frame contain the variable header for authenticate:
  `"x-usename""admin";"x-password""123456";`

The `key` should be lowercase string by standard, but no limitation or restriction for the `key` and `value` except that they must be string, for the `;` character we need escape by an backslash `\;`.

### Data

The sequence of bytes represent the data of the frame, you can use variable header to inform receiver the content-type of the data. eg: `"content-type""text"`

# Topics

## Format

In BPSP the term `topic` refers to an string that the broker use to match/filter messsages to appropriate subscriber for each connected client. 
The topic consists of one or more topic levels. 
Each topic level is seperated by a forward slash `/`. 
Each topic must contain ***at least one character*** and Topics are case-sensitive. For example `locationA/sensorA` and `LOCATIONa/SENSORa` are two different topics.

Example: `locationA/sensorA`

## Wildcards

When subscriber subscribe to a topic, it can subscribe to the exact topic string of a published message or it can use wildcards to subscribe to multiple topic message. 
Note that a wildcards can only use to subscribe topic not to publish a message. 
There are two different kinds of wildcards: ***single-level*** and ***multi-level***

### Single Level: +

Single level wildcard replace one topic level. For example: `locationA/+` topic will match `locationA/sensorA`, `locationA/sensorB` published messages but not `locationA/sensorA/uptime`.

### Multi Level: *

Multi level wildcard replace one or more topic level. For example: `locationA/*` topic will match `locationA` ?, `locationA/sensorA`, `locationA/sensorB` and `locationA/sensorA/uptime` published messages. 

## How topic was matched?

BPSP Topics is represents as a **Tree** structure, start at root node and following by 0 or more child node. 
Each topic is split into token by seperated character `/`, number of tokens is the total levels of topic. Eg: `locationA/sensorA/uptime` => `["locationA", "sensorA", "uptime", "/"]` has 4 levels. 
As you can see all topic will implicitly end with token "/". 

List of valid token:
- `/` end of topic
- `+` single-level
- `*` multi-level
- other... 

Each node is one of level in topic, consists one hash table for list next node level, one hash table for list subscribers (note that this hash table contains at least 2 entry, `+` and `*` entry). 
By structuring into **Tree** and using **Hash Table**, each published message only need to traverse at most total number of level of published topic, so it will be **O(1)** instead of  **O(n)** for all list subscribers. 
For example, the structure of Node Tree in Pseudocode and how theres topic `locationA/sensorA/*`, `locationA/sensorA/+`, `locationA/sensorA/uptime`, `locationB/sensorB` was fit in this tree. 
```c
/**
 *  Node topic in Tree 
 *
 */
struct topic__node {
    bpsp__subscriber* subs;     // all subscribers match until end current token

    topic__node* sl_node;  // single-level node
    topic__node* ml_node;  // multi-level node

    /** node hash table **/
    topic__hash_node [string]nodes;
};

struct topic__node root;

```

```
                                                          
                                    root                  
                            +------------------+          
                            |                  |          
                            |                  |          
                        locationA          locationB      
                    +-------                   -------+   
                    |                                 |   
                    |                                 |   
                 sensorA                           sensorB
     +-------------------------+                          
     |                |        |                          
     |                |        |                          
 wildcard(*)    wildcard(+)   uptime                      

```

