# Arduino ARP-Spoof
Kicks out everyone in your LAN with Arduino and an ENC28J60 ethernet controller.

![ENC28J60 ethernet shield + Arduino nano plugged in and working](https://raw.githubusercontent.com/spacehuhn/enc28j60_ARPspoofer/master/images/1.jpg)  

**Support me and my projects on [Patreon!](https://www.patreon.com/spacehuhn)**  
[<img width="200" alt="Support me on Patreon" src="https://raw.githubusercontent.com/spacehuhn/ArduinoARPspoof/master/images/patreon.png">](https://www.patreon.com/spacehuhn)  


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

Using an Arduino with an ethernet controller, this device will perform an [ARP spoofing](https://en.wikipedia.org/wiki/ARP_spoofing) attack to block the communication from every client device in your LAN to the gateway.  

### How it works

It will constanly send ARP replies to every device in the LAN and tell them that the gateway is at a random MAC adress.  
The gateway is basically the link between your local network and the internet. By telling everyone it's at an adress that doesn't exist, nobody will be able to communicate to it anymore and by that, loose the connection.  

### What an ENC28J60 is

The ENC28J60 is a cheap 10mbit SPI ethernet controller for Arduino. Because it has a very open and easy hackable library, it's perfect for this project and you could even programm a man-in-the-middle attack or other funny stuff with it.

![ENC28J60 ethernet shield and an Arduino nano](https://raw.githubusercontent.com/spacehuhn/enc28j60_ARPspoofer/master/images/2.jpg)

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
Ho to do that is explained here: https://github.com/jcw/ethercard  

**3. [Optional] Change some settings**

In the beginning of the sketch are a few settings declared that you can change.  
```
// ===== Settings ===== //
//#define webinterface /* <-- uncomment that if you want to use the webinterface */
//#define debug /* <-- uncomment that if you want to use get a serial output */
#define led 13
#define auth_password "ARP"
int packetRate = 20; //packets send per second
static uint8_t mymac[] = { 0xc0, 0xab, 0x03, 0x22, 0x55, 0x99 };
```  

**4. Upload the code**

Compile & upload the sketch to your Arduino and you are done :)  


## How to use it

Power it over USB and plug in an ethernet cable, that's all!  

**Using the WebInterface:**  
If you uncommented `webinterface` in the code, the Arduino won't start the attack by itself.
You have to open its website and start the attack manually.  
The IP will be printed out in the serial monitor and the default password is `ARP`.  

![WebInterface](https://raw.githubusercontent.com/spacehuhn/enc28j60_ARPspoofer/master/images/3.jpg)

## License

This project is licensed under the MIT License - see the [license file](LICENSE) for details.

## Sources and additional links

ARP spoofing: https://en.wikipedia.org/wiki/ARP_spoofing  
ENC28J60: http://www.microchip.com/wwwproducts/en/en022889
