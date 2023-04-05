# REQUIREMENT

Experimental Purpose: Familiarize and implement the basic principles of network monitoring.

Experimental Environment: Linux/Windows

Experimental Content: Write a program to monitor network traffic using C/C++ language (socket function must be used) and analyze intercepted packets.

Experimental Requirements: Submit an experimental report (see attachment); submit the source code. The file naming format for submission is: student ID + name + sniffer.


# IMPL
1. bind a sockfd with AF_PACKET to get full packet data(contain Eth)
2. associated this sockfd with interface and add it with promiscuous mode
-   Promiscuous mode is a feature that can be enabled on a network interface in Linux to allow the interface to receive all network traffic, even if the traffic is not intended for the interface.
3. Accept packets cyclically and resolve them.
-   eth portion
-   ip portion
-   tcp portion

# EFFECT

```shell
Source IP: 192.168.18.160
Destination IP: 192.168.18.1
Source Port: 16
Destination Port: ec32
Payload Size: 5b4
Payload Content: 
00000000  0e 20 80 f1 67 55 a7 82 92 0a 8f 3b 20 54 85 f8  |. ..gU.....; T..|
00000010  ca e3 49 a5 ca ba 10 2e 83 31 6b 7a 06 9a 20 c2  |..I......1kz.. .|
00000020  86 98 6d af de ca e6 6d 33 b2 80 0d 15 85 eb 4b  |..m....m3......K|
00000030  92 05 dd e0 da 45 5c df 2a 83 37 50 c6 bf 64 89  |.....E\.*.7P..d.|
00000040  44 13 f6 92 c1 31 ce d7 c0 d2 f5 ce f2 c0 cc 1d  |D....1..........|
00000050  0a 95 e0 a8 87 e9 5e 98 b2 e4 b9 4c 2a 59 9e 12  |......^....L*Y..|
00000060  c6 cc 99 c2 e5 4a 86 42 e6 8f 87 b5 7b 70 99 d4  |.....J.B....{p..|
00000070  35 04 99 b4 d7 f2 a1 bc 51 8a 81 c2 b6 42 40 98  |5.......Q....B@.|
00000080  7c a4 15 d3 0c 4d 4d cc ac ba 81 05 ac c7 d2 74  ||....MM........t|
00000090  74 1e 54 0f ee b9 36 42 58 8a 9f 37 da 23 87 0b  |t.T...6BX..7.#..|
```