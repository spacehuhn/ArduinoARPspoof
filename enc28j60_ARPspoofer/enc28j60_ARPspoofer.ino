#include <EtherCard.h>

static byte mymac[] = { 0xc0,0xab,0x03,0x22,0x55,0x99 };

byte Ethernet::buffer[700];
static long timer;

bool connection = false;

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

void _connect(){
  if(!ether.dhcpSetup()){
    Serial.println("DHCP failed");
    connection = false;
  }else{
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("GW IP: ", ether.gwip);
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
  Serial.begin(115200);
  Serial.println("ready!");
  Serial.println("waiting for LAN connection...");
  
  if(ether.begin(sizeof Ethernet::buffer, mymac) == 0) Serial.println( "Failed to access Ethernet controller");

  while(!connection){
    _connect();
    delay(3000);
  }
  
}

void loop () {
  if(connection){
    ether.packetSend(48);
    Serial.print(".");
    delay(2000);
  }
}
