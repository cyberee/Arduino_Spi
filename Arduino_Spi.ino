// Console base program for working with SPI memory

//Version 0.0.6

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

const int chipSelectPin = 10;
const String Version = "0.0.7";


void setup() {
  // initialize serial:
  Serial.begin(115200);
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  Serial.println("Serial SPI Console Program Version: " + Version);
  Serial.println("For help type ?");
  
}

void loop() 
{
  String s_Buffer;
  String cmd;
  String parameter[6];
  int p_Count =0;
  int index;
  // if there's any serial available, read it:
  while (Serial.available() > 0) 
  {
    s_Buffer = Serial.readString();
    //Serial.println(s_Buffer);
    index = s_Buffer.indexOf(",");
    //Serial.print("First , index = ");
    //Serial.println( index);
    cmd=s_Buffer.substring(0,index);
    //Serial.println("Command = " + cmd);
    if(index != -1)
    {
      s_Buffer=s_Buffer.substring(index+1);
      Serial.println(s_Buffer);
      p_Count=0;
      index = s_Buffer.indexOf(",");
      while(index != -1)
      {
        parameter[p_Count]=s_Buffer.substring(0,index);
        p_Count++;
        s_Buffer=s_Buffer.substring(index+1);
        index = s_Buffer.indexOf(",");
      }
      parameter[p_Count]=s_Buffer;
      p_Count++;
    }
    //Serial.print("P Count = ");
    //Serial.println(p_Count);    
    
    for(int x=0;x<p_Count;x++)
    {
      Serial.println(parameter[x]);
    }
    if (cmd.indexOf("RDID") != -1)
    {
      Read_ID();
    }
    if (cmd.indexOf("RDSR") != -1)
    {
      Read_SR();
    }   
    if (cmd.indexOf("REMS") != -1)
    {
      Read_EMS();
    } 
    if (cmd.indexOf("RES") != -1)
    {
      Read_ES();
    } 
    if (cmd.indexOf("READ") != -1)
    {
      Read_Data(parameter[0].toInt());
    }
  }
}


void Read_ID ( )
{
  int bytesToRead = 3;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return

  SPI_Read(0x9F,result,bytesToRead);
  Serial.print ("Manufacturer ID =");
  Serial.println (result[0],HEX);
  Serial.print ("Memory Type =");
  Serial.println (result[1],HEX);
  Serial.print ("Memory Density =");
  Serial.println (result[2],HEX);
}
void Read_Data (int sectorsToRead)
{
  int bytesToRead = 1024;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return
  int columns =32;
  int rows = bytesToRead/columns;
  byte address1 =0;
  Serial.println ("Data =");
  for (int sector = 0;sector < sectorsToRead;sector++)
  {
    if (sector > 15)
    {
      address1 = 1;
      
    }
    else
    {
      address1 = 0;
    }
    int sector_address=sector*4096;
    for(int k_count=0;k_count<4;k_count++)
    {
      SPI_Read_Address(0x03,k_count*1024+sector_address,address1,result,bytesToRead);
      //Serial.println(k_count*1024+sector_address);
      for (int x =0; x < rows;x++)
      {
        for (int y =0;y <columns;y++)
        {
          Serial.print (result[(x*rows)+y],HEX);
          Serial.print(",");
        }
        Serial.println(" ");
      }
      //Serial.println(" ");
    }
    //Serial.println(" ");
  }
  
}
void Read_SR( )
{
  int bytesToRead = 2;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return

  SPI_Read(0x05,result,bytesToRead);
  Serial.print ("Status = ");
  Serial.println (result[0],HEX);
}

void Read_EMS ( )
{
  int bytesToRead = 5;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return
  // take the chip select low to select the device:

  SPI_Read(0x90,result,bytesToRead);
  Serial.print ("Manufacturer ID =");
  Serial.println (result[3],HEX);
  Serial.print ("Device ID =");
  Serial.println (result[4],HEX);

}
void Read_ES( )
{
  int bytesToRead = 4;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return
  // take the chip select low to select the device:

  SPI_Read(0xAB,result,bytesToRead);
  Serial.print ("Electronic ID = ");
  Serial.println (result[3],HEX);
}

void SPI_Read(byte cmd, byte data[], int bytesToRead)
{
  byte inByte = 0;           // incoming byte from the SPI
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(cmd);
  for (int x = 0; x < bytesToRead; x++)
  {
    data[x] = SPI.transfer(0x00);
  }
  digitalWrite(chipSelectPin, HIGH);  
}
void SPI_Read_Address(byte cmd, long int address,byte address1, byte data[], int bytesToRead)
{
  byte inByte = 0;           // incoming byte from the SPI
  // take the chip select low to select the device:
  byte address3 = address & 0xFF;
  byte address2 = (address & 0x00FF00) >> 8;
  //byte address1 = (address/65536L);

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(address1);
  SPI.transfer(address2);
  SPI.transfer(address3);
//  Serial.print(address,HEX);
//  Serial.print(":");
//  Serial.print(address1,HEX);
//  Serial.print(":");
//  Serial.print(address2,HEX);
//  Serial.print(":");
//  Serial.println(address3,HEX);
  for (int x = 0; x < bytesToRead; x++)
  {
    data[x] = SPI.transfer(0x00);
  }
  digitalWrite(chipSelectPin, HIGH);  
}
