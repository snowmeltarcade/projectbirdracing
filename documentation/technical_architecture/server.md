# Server

The server controls all game play and simulations. Players connect to the server through the client.

## Class Design

The main orchestrating class is `Game`. This will hold instances of all main modules and will manage the game loop.

The overall class structure of the server will be as follows:

![Server Class Structure Overview](images/server_class_structure_overview.drawio.png)
