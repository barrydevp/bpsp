# Basic Publish Subscribe Protocol (BPSP)

## Structure of an BPSP Frame

### Overview
                                                                                                                          
                                                    BPSP Specification                                                    
                                                                                                                          
                                                                                                                          
                                                0      1      2      3                                                    
                                            +--------------+-------+------+ -+                                            
                                         0  |VARIABLES SIZE|OPCODE | FLAG |  |                                            
                                            ---------------+-------+-------  |  Fixed Header (8 + nbytes(DATA SIZE)) bytes
                                      4...  |         DATA SIZE           |  |                                            
                                            -------------------------------  |                                            
                  4 + nbytes(DATA SIZE)...  |      VARIABLE HEADERS       |  |  Variable Header (OFF * 4 - 8) bytes       
                                            ------------------------------- -|                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
 4 + nbytes(DATA SIZE + VARIABLES SIZE)...  |            DATA             |  |  Payload/Body (SIZE - OFF * 4) bytes       
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            |                             |  |                                            
                                            +-----------------------------+ -+                                             

### Data representation

#### Bits

#### One Byte Integer

#### Two Byte Integer

#### Variable Byte Integer

#### Character

### BSPS Frame Format

#### Fixed Header

Each BSPS Frame contains a Fixed Header as show below:

                                         0  |VARIABLES SIZE|OPCODE | FLAG |  |                                            
                                            ---------------+-------+-------  |  Fixed Header (8 + nbytes(DATA SIZE)) bytes
                                      4...  |         DATA SIZE           |  |                                            

##### Variables Size

The Variables size is the Two Byte Integer (Unsigned value) that represents the size in byte of variable header.

##### Opcode

The Opcode is the One Byte Integer (Usigned value) that represents the Operation (Command) of the frame.

##### FLAG

The Flag is the One Byte Integer with each bits represents the flag for the appropriate Operation.

##### Data Size

The Data Size is the Variable Byte Integer that represents the size of payload/body, the number of bytes is not limited for now, but we should use at most 4 bytes to represents data size at maximum 2^28 bytes.

#### Variable Header

The Variable Header is the key-value string that indicate for the extension of the frame, each header key-value pair is represents as `"key""value";("key""value";(...))`.
For example the Authentication Frame contain the variable header for authenticate:
  `"x-usename""admin";"x-password""123456";`
The `key` should be lowercase string by standard, but no limitation or restriction for the `key` and `value` except that they must be string, for the `;` character we need escape by an backslash `\;`.

#### Data

The sequence of bytes represent the data of the frame, you can use variable to inform receiver the content-type of the data. eg: "content-type""text"

