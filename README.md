# enc28j60_ARPspoofer
Kicks out everyone in your LAN with an ENC28J60 ethernet controller and Arduino.

![ENC28J60 ethernet shield + Arduino nano plugged in and working](https://raw.githubusercontent.com/spacehuhn/enc28j60_ARPspoofer/master/images/1.jpg)

## Contents
- [Introduction](#introduction)
  - [What it is](#what-it-is)
  - [How it works](#how-it-works)
  - [What an ENC28J60 is](#what-an-enc28j60-is)
  - [How to protect against it](#how-to-protect-against-it)
- [Disclaimer](#disclaimer)
- [Installation](#installation)
- [How to use it](#how-to-use-it)
- [License](#license)
- [Sources and additional links](#sources-and-additional-links)

## Introduction ##


### What it is

Using an Arduino with an ethernet controller, this device will perform an [ARP spoofing](https://en.wikipedia.org/wiki/ARP_spoofing) attack to disconnect every client device in your LAN.

### How it works

It will send an ARP reply every 2 seconds to everyone in the LAN that the gateway is at a random MAC adress. If the network or client doesn't provide any kind of protection against ARP spoofing, it will save the spoofed MAC adress and will be unable to communicate with the gateway.

### What an ENC28J60 is

The ENC28J60 is a cheap 10mbit SPI ethernet controller for Arduino. Because it has a very open and easy hackable library it's perfect for this project and you could even programm a man-in-the-middle attack or other funny stuff with it.

![ENC28J60 ethernet shield and an Arduino nano](https://raw.githubusercontent.com/spacehuhn/enc28j60_ARPspoofer/master/images/3.jpg)

### How to protect against it

Use a router, network switch or software that provides you protection against ARP spoofing.  
Note: I haven't tested it on such protected hardware yet.


## Disclaimer

Use it only for testing purposes on your own network!


## Installation

You will need an Arduino and (of course) an ENC28J60.  
If you buy an Arduino ethernet shield be sure it **doesn't** use a wiznet controller (e.g. w5100 or w5500), **this project will only work with an ENC28J60!**

**1. Wire everything up**

To do this you need to connect both the Arduino and the controller via their SPI pins. If you're unsure how to do this, you can google for the pinout of your Arduino and the ethernet controller. There are different versions of the ENC28J60 out there. I'm using a shield for the Arduino nano.  

**2. Install library**

You will need to add the ethercard library in Arduino.  
How to do this you'll find here: https://github.com/jcw/ethercard

**3. Upload the code**

Upload the ARP-spoofer code to the Arduino and your done :)


## How to use it

Power it over USB and plug in an ethernet cable, that's all!  

**Using the WebInterface:**  
If you set `web_en = true` in the code, the Arduino won't start the attack by itself.
You have to open its website and start the attack manually.  
The IP will be printed out in the serial monitor and the default password is `ARP`.  

Thanks to VoidTyphoon for implementing this feature.

## License

This project is licensed under the MIT License - see the [license file](LICENSE) file for details.


## Sources and additional links

ARP spoofing: https://en.wikipedia.org/wiki/ARP_spoofing  
ENC28J60: http://www.microchip.com/wwwproducts/en/en022889
