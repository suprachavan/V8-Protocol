# V8-Protocol
Implementation of V8 protocol at data link layer in C++
## Project Summary
This project deals with implementation of V-8 protocol in a small network consisting of four nodes and three links formed by a star topology. 

An F-8 frame is constructed in which the message string to be transmitted is wrapped up. The checksum for the string message is also calculated by performing Ex-OR addition of the successive bytes and this checksum is also appended to the payload. 

This message is then transmitted over the network from one host to the other and reply is obtained in return.

The sender and receiver nodes attach themselves to link and get assigned unique ids, while one link attaches to two nodes via the same id.
After displaying both nodes' & link's contents using contents(), both nodes' snd() function is called which in turn calls xmt() and rcv() to facilitate exchange of string message.

For more details view the `readme.txt` file in this repository `https://github.com/suprachavan/V8-Protocol/blob/master/README.txt`
