To compile:
    gcc cryptweetclient.c -o client
    gcc cryptweetserver.c -o server
    
To run:
    ./server PORTNUMBER            where PORTNUMBER is the desired port to run the server on
    ./client PORTNUMBER            where PORTNUMBER is the desired port to run the client on, must match the server
    
To use:
    For server admins, entering integers 1 - 3 for corresponding hashing algorithm once the server prompts
        1 - Sequential Index
        2 - Word Sum
        3 - 16 bit Internet Checksum
    
    For client users, entering integers 0 - 1 to either enter the message or exit
        0 - Exit program
        1 - Enter message
    
    
Choice of TCP:
    Using TCP ensures reliability, packets sent with TCP are tracked so no data is lost or corrupted in transit.
    Additionally, until the connection is established, packets only get to the OS layer, not the application layer.
    This increases security as one would have to be in between the endpoints to be able to forge IP addresses.
    Where as plain UDP does not keep state or have handshakes. This mean an attacker could easily send a spoofed packet.
    Since speed is not really a concern with these messages, TCP is clearly the better choice here.
    
    TCP is the most commonly used protocol on the Internet, and as a result should be better in terms of compatibility than UDP
    
    
Testing
    
    Testing was done at home and also on the university servers. The server would be run on the university servers and the client was tested at home and at university.
    All three hashing algorithm works
    But punctuations are lost after encryption or decryption
    