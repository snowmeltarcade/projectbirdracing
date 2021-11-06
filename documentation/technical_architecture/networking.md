# Networking

- [Networking](#networking)
  - [Packets](#packets)
    - [Data Types Encodings](#data-types-encodings)
  - [Message Types](#message-types)
    - [Client -> Server - Verify Version](#client---server---verify-version)
    - [Server -> Client - Verify Version Response](#server---client---verify-version-response)
    - [Client -> Server - Connect Request](#client---server---connect-request)
    - [Server -> Client - Load World](#server---client---load-world)
    - [Client -> Server - World Loaded](#client---server---world-loaded)
    - [Server -> Client - Update Player Information](#server---client---update-player-information)
    - [Server -> Client - Update Avatar Information](#server---client---update-avatar-information)
    - [Server -> Client - Update World Information](#server---client---update-world-information)
    - [Server -> Client - Update Time](#server---client---update-time)
    - [Client -> Server - Update Player Details](#client---server---update-player-details)

TCP and UDP streams are provided. All required messages are sent over TCP and informational messages are sent over UDP.

## Packets

Messages are sent in packets with set structures. The structure is defined by the message type and it agreed upon by both the server and the client at code compile time. This means that the server and client must be of compatible versions. A server/client configuration using incompatible versions will result in undefined behavior.

The message type also dictates if the message is counted as required or informational. This state is called the message `importance`.

Packets will be compressed using [LZ4](https://en.wikipedia.org/wiki/LZ4_(compression_algorithm)) Currently, though, there is no need to the packets, or add a hash of the packet's data to the message.

| Field | Data Type | Size In Bytes |
| --- | --- | --- |
| Message Type | Integer | 2 |
| Compressed Data Size In Bytes | Integer | 4 |
| Decompressed Data Size In Bytes | Integer | 4 |
| Data Buffer | Byte Array | `data_size_in_bytes` |

### Data Types Encodings

As the data buffer in a packet is compressed, data types are written byte-for-byte into the data buffer. Afterwards, the data buffer is compressed.

Data types are encoded using big endian and are decoded into whatever endian the client architecture requires.

Data types are encoded as follows:

| Data Type | Size In Bytes | Encoding |
| --- | --- | --- |
| Boolean | 1 | `true` != 0, `false` == 0 |
| Byte | 1 | Byte for byte |
| Integer | 4 | Byte for byte |
| Big Integer | 8 | Byte for byte |
| Float | 4 | Byte for byte |
| String | 4 + `len(string)` | Size of string + string data byte for byte |

## Message Types

The messages that are send and received are defined below.

### Client -> Server - Verify Version

Sends the client version to the server.

| Attribute | Value |
| --- | --- |
| Message Type | `0` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| Version Major | Byte |
| Version Minor | Byte |
| Version Build | Byte |

### Server -> Client - Verify Version Response

Indicates if the client is of a compatible version to the server.

| Attribute | Value |
| --- | --- |
| Message Type | `1` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| Is Confirmed | Boolean |

### Client -> Server - Connect Request

Requests connection to the server.

| Attribute | Value |
| --- | --- |
| Message Type | `2` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| Username | String |

### Server -> Client - Load World

Instructs client to load a world.

| Attribute | Value |
| --- | --- |
| Message Type | `3` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| World Type | Integer |
| Current Server Time | Big Integer |

### Client -> Server - World Loaded

Informs server that a world has been loaded.

| Attribute | Value |
| --- | --- |
| Message Type | `4` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| World Type | Integer |

### Server -> Client - Update Player Information

Updates the player's information on the client with the data from the server.

| Attribute | Value |
| --- | --- |
| Message Type | `5` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| Avatar X Position | Float |
| Avatar Y Position (interpreted as the Z position on the client for 3D renderers) | Float |

TODO: Add other needed player attributes here, such as currency, avatar appearance etc...

### Server -> Client - Update Avatar Information

Updates the player's information on the client with the data from the server.

| Attribute | Value |
| --- | --- |
| Message Type | `6` |
| Importance | `Informational` |

| Field | Data Type |
| --- | --- |
| Avatar X Position | Float |
| Avatar Y Position (interpreted as the Z position on the client for 3D renderers) | Float |
| Action Index | Integer |

TODO: Add other needed avatar attributes here, such as health, appearance etc...

### Server -> Client - Update World Information

Updates the world information on the client with the data from the server.

| Attribute | Value |
| --- | --- |
| Message Type | `7` |
| Importance | `Required` |

| Field | Data Type |
| --- | --- |
| Number of Tiles in Ledger | Integer |
| Ledger | `number_of_tiles_in_ledger` * `sizeof(Tile)` |

A `Tile` is defined as:

| Field | Data Type |
| --- | --- |
| Tile X Index | Integer |
| Tile Y Index | Integer |
| Tile Type | Integer |

### Server -> Client - Update Time

Updates the client with the current server time.

| Attribute | Value |
| --- | --- |
| Message Type | `8` |
| Importance | `Informational` |

| Field | Data Type | Notes |
| --- | --- | --- |
| World Type | Integer | Used to validate the correct world is being refereed to |
| Current Server Time | Big Integer | |

### Client -> Server - Update Player Details

Updates the server with the latest player details as specified by the client.

| Attribute | Value |
| --- | --- |
| Message Type | `9` |
| Importance | `Informational` |

| Field | Data Type |
| --- | --- | --- |
| Avatar X Position | Float |
| Avatar Y Position | Float |
| Action Index | Integer |
