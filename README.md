# enc28j60_ARPspoofer
Kicks out everyone in your LAN with an enc28j60 ethernet controller and Arduino.

## Contents
- [Introduction](#introduction)
  - [What it is](#what-it-is)
  - [How it works](#how-it-works)
  - [What an ENC28j60 is](#what-an-enc28j60-is)
  - [How to protect against it](#how-to-protect-against-it)
- [Disclaimer](#disclaimer)
- [Installation](#installation)
- [How to use it](#how-to-use-it)
- [License](#license)
- [Sources and additional links](#sources-and-additional-links)

## Introduction ##

### What it is

Using an arduino with an ethernet controller, this device will perform an ARP spoofing or ARP poisoning attack to diconnect every client device in your LAN.

### How it works

It will send an ARP reply every 2 seconds to everyone in the LAN that the gateway IP is at some random MAC adress. If the network or client doesn't provide any kind of protection against ARP spoofing, it will save the spoofed MAC adress and will be unable to communicate to the gateway.

### What an ENC28J60 is

The ENC28J60 is a cheap 10mbit SPI ethernet controller for arduino. Because it has an very open and easy hackable library it's perfect for this project and you could even programm a man-in-the-middle attack or other fun stuff with it.

### How to protect against it

Use a router, network switch or software that provides you protection against ARP spoofing. 
Note: I haven't test it on such protected hardware yet.

## Disclaimer

Use it only for testing purposes on your own devices!

## Installation

You will need an Arduino and (of course) an ENC28J60. You also have to wire it up, for that you need to connect both via their SPI pins. If you're unsure how to wire it up, you can google for the pinout of your arduino and the ethernet controller. There are different versions of the ENC28J60 out there. I'm useing a shield for the arduino nano.




Links: https://en.wikipedia.org/wiki/ARP_spoofing
https://github.com/jcw/ethercard
http://www.microchip.com/wwwproducts/en/en022889
