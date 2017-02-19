#include <enc28j60.h>
#include <EtherCard.h>
#include <net.h>

static byte mymac[] = { 0xc0,0xab,0x03,0x22,0x55,0x99 };

byte Ethernet::buffer[700];
static long timer;

bool connection = false;
bool toggle_status = false;
bool tmp_status = true;
int arp_count = 0;
char pwholder;

////////////////////////
//ENABLE WEB INTERFACE// 
//     true = ON      //
//    false = OFF     //
////////////////////////
bool web_en = true;

//Set password here
char* auth_password = "ARP";
//Set DEBUG status
bool debug = false;


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

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void _connect(){
  if(!ether.dhcpSetup()){
    Serial.println("DHCP failed");
    connection = false;
  }else{
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("GateWay IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
    
    //set gateway IP
    for(int i=0;i<4;i++) _data[28+i] =  ether.gwip[i];
  
    //set fake MAC
    for(int i=0;i<6;i++) _data[6+i] = _data[22+i] = mymac[i];
  
    //fill buffer
    for(int i=0;i<48;i++) ether.buffer[i] = _data[i];
    
    connection = true;
  }
}

void setup () {
  pinMode(13, OUTPUT); 
  Serial.begin(115200);
  Serial.println("ready!");
  Serial.println("waiting for LAN connection...");
  if(ether.begin(sizeof Ethernet::buffer, mymac) == 0){
    Serial.println( "Failed to access Ethernet controller");
  }

  while(!connection){
    _connect();
    delay(1000);
  }
  
  
}

void loop () {
if(web_en){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  tmp_status = false;
  if(pos) {
    Serial.println();
    boolean password_valid = true; 
    
    // is it a POST request?
    if(strstr((char *)Ethernet::buffer + pos, "POST /") != 0) {
      
      if(debug)Serial.println("New POST request!");
      
      // search and verify the password
      char password[20];      
      char* password_position = strstr((char *)Ethernet::buffer + pos, "&pwd=");
      if(password_position != 0) {
        strcpy(password, password_position + 5);
        if(debug)Serial.print("Found password=");
        if(debug)Serial.println(password);
        if(strcmp(password, auth_password) == 0){ 
          if(debug)Serial.println("Valid password :)");
          
        }else {
          if(debug)Serial.println("Wrong password :(");
          password_valid = false;
          pwholder = "";
        }
      }
      
      // search for ON= or OFF= command
      if(password_valid) {
        pwholder = password;
        // OFF command
        if(strstr((char *)Ethernet::buffer + pos, "OFF=") != 0) {
          if(toggle_status){
            Serial.println("ARP OFF");
            toggle_status = false;
            tmp_status = true;
          }
        
        // ON command
        } else if(strstr((char *)Ethernet::buffer + pos, "ON=") != 0) {
          Serial.println("ARP ON");
          toggle_status = true;
          tmp_status = true;          
        } else if(debug)Serial.println("Unknown command :(");
      }
    }else{
      tmp_status = true;
    }      
    
    // Output HTML page        
    BufferFiller bfill = ether.tcpOffset();
    bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"
      "<html><head><title>ARP Panel</title></head>"
      "<body><form method=\"POST\">"));
    // Enable / disable buttons based on the output status
    if(toggle_status == true) bfill.emit_p(PSTR("</p><div><button style=\"width: 200px; display:none;\" name=\"ON\" disabled>Turn ON</button><br><button style=\"width: 200px\" name=\"OFF\">Turn OFF</button></div>"));
    else bfill.emit_p(PSTR("</p><div><button style=\"width: 200px\" name=\"ON\">Turn ON</button><br><button style=\"width: 200px; display:none;\" name=\"OFF\" disabled>Turn OFF</button></div>"));

    // A wrong password was entered?
    if(password_valid == true){ 
      bfill.emit_p(PSTR("<div><input style=\"width: 200px\" type=\"password\" value=\"\" name=\"pwd\"></div></form></body>"));
    }else bfill.emit_p(PSTR("<div><input style=\"width: 200px\" type=\"password\"  name=\"pwd\">Wrong password</div></form></body>"));
    
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    bfill.emit_p(PSTR("Current Uptime: $D$D:$D$D:$D$D<br>"), h/10, h%10, m/10, m%10, s/10, s%10);
    bfill.emit_p(PSTR("Current ARP Count: $D<br>"),arp_count);
    bfill.emit_p(PSTR("Free RAM: $D bytes<br>"), freeRam()); 
    /* //PROBLEMS WITH DISPLAYING IP'S
    bfill.emit_p(PSTR("MY IP: $D"), ether.myip);  
    bfill.emit_p(PSTR("Gateway IP: $D"), ether.gwip);  
    bfill.emit_p(PSTR("DNS IP: $D"), ether.dnsip);
    */     
    ether.httpServerReply(bfill.position());
  }else{
      tmp_status = true;
      if(connection){
      if(toggle_status){
        if(tmp_status) {
        digitalWrite(13, HIGH); // Turn on STATUS LED
        
        //Reset Buffer
        for(int i=0;i<48;i++) ether.buffer[i] = _data[i];
        
        ether.packetSend(48);
        if(debug)Serial.println("APR PACKET SENT.                  ");
        ++arp_count;
        //delay(1000); //Disabled due to lag with disabling on web interface, This is due to arduino not being multithread.
        digitalWrite(13, LOW);
        tmp_status = false;
        }
      }
    }else{
      digitalWrite(13, LOW);  // No Connection, turn off STATUS LED
    }
   } 
}else{
  if(connection){
    digitalWrite(13, HIGH); // Turn on STATUS LED
    ether.packetSend(48);
    Serial.println("APR PACKET SENT.                  ");
    delay(1500);
    digitalWrite(13, LOW);
  }else{
    digitalWrite(13, LOW);  // No Connection, turn off STATUS LED
  }
}
}
