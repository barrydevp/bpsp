# Basic Publish Subscribe Protocol (BPSP)

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

