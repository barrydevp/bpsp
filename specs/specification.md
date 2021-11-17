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

The `key` should be lowercase string by standard, but no limitation or restriction for the `key` and `value` except that they must be string, for the `"` character we need escape by an backslash `\"`.

For example the Authentication Frame contain the variable header for authenticate: `"x-usename""admin";"x-password""123456";`

### Data

The sequence of bytes represent the data of the frame, you can use variable header to inform receiver the content-type of the data, eg: `"content-type""text";`.

# BPSP Control Frame

## Opcode

<table>
<thead><tr><th>Opcode</th><th>Value</th><th>Sent By</th><th>Description</th></tr></thead>
<tbody>
<tr><td>INFO</td><td>1</td><td>Server</td><td>Sent to client after initial TCP connection to inform about server.</td></tr>
<tr><td>CONNECT</td><td>2</td><td>Client</td><td>Sent to server to specify connection information after received INFO.</td></tr>
<tr><td>PUB</td><td>3</td><td>Client</td><td>Publish a message to a topic.</td></tr>
<tr><td>SUB</td><td>4</td><td>Client</td><td>Subscribe to a topic.</td></tr>
<tr><td>UNSUB</td><td>5</td><td>Client</td><td>Unsubscribe to a topic.</td></tr>
<tr><td>MSG</td><td>6</td><td>Server</td><td>Delivers a message to responsible subscriber.</td></tr>
<tr><td>+OK</td><td>7</td><td>Server</td><td>Positive acknowledges.</td></tr>
<tr><td>-ERR</td><td>8</td><td>Server</td><td>Negative acfknowledges may cause client to disconnect.</td></tr>
</tbody>
</table>

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
There are two different kinds of wildcards: ***single-level*** and ***multi-level***.

### Single Level: `+`

Single level wildcard replace one topic level. For example: `locationA/+` topic will match `locationA/sensorA`, `locationA/sensorB` but not `locationA/sensorA/uptime` published messages.

### Multi Level: `*`

Multi level wildcard replace one or more topic level. For example: `locationA/*` topic will match `locationA` ?, `locationA/sensorA`, `locationA/sensorB` and `locationA/sensorA/uptime` published messages. 

## How topic was matched?

BPSP Topics is represents as a **Tree** structure, start at root node and following by 0 or more child node. 
Each topic is split into token by seperated character `/`, number of tokens is the total levels of topic. Eg: `locationA/sensorA/uptime` => `["locationA", "sensorA", "uptime", "/"]` has 4 levels. 
As you can see all topic will implicitly end with token `/`. 

List of valid token:
- `/` end of topic
- `+` single-level
- `*` multi-level
- other... 

Each node is one topic level consists one hash table point to next node topic level, one hash table point to list subscribers, single-level `+` node and multi-level `*` node. 
By structuring into **Tree** and using **Hash Table**, each published message only need to traverse at most total number of level of published topic, so it will be **O(1) constant time** instead of  **O(n)** for traverse all list subscribers. 
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

