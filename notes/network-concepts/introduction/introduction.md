# Notes on Beej's Guide to Network Concepts - Chapter 2

> "The big idea of networking is that we're going to get arbitrary bytes of data exchanged
> between two (or more) computers on the Internet or [Local Access Network (LAN)]."

What this means depends on the application or domain, but what's essential is how we:

- Identify the source and destination of our data
- How we maintain the integrity of our data
- And how we route our data from one individual computer to another (or multiple).

## Some Types of Networks

### Circuit Switched

- The internet **does not** use this tpe of network, but it is important conceptually:
- This network operates similarily to an old-school telephone operator, where you would:
  1. Pick up a reciever, and turn a crank to generate a signal
  2. The signal would be picked up by the telephone exchange office and a human operator would direct you to wherever you wanted to go by plugging in wires into a switchboard to physically connect you to the destination.
- This scaled poorly, and even when replaced with electromecanical relays, you would still have these problems:
  - Dedicated circuit was needed for every call.
  - Multiple people could not use the same line at the same time.

### Packet Switched

- In a packet switched network, all the data you want to send is split up into packets of varying size.
- Each packet is sent over the line and then the computer reconstructs the data from each packet.
- What is beneficial about this method is that:
  - You don't need a dedicated circuit between all possible connections.
  - Multiple computers can use the same wire to send packets.
  - A wire can be used to its full capacity
- Similar to a snail mail system, packets on the network go through various "post offices" (routers) on their way to the destination.
- Each router examines the packet's "address" (IP address) and determines the best path to forward it based on the current network conditions.

#### An analogy:

- Think of a jigsaw puzzle where each piece is sent separately to the recipient.
- Each piece might travel through different paths and arrive at different times, but once all the pieces are collected, the recipient can assemble them to recreate the complete picture.

## Client/Server Architecture
