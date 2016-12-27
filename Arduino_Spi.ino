// Console base program for working with SPI memory

//Version 0.0.7

// the sensor communicates using SPI, so include the library:
#include <SPI.h>
#include <stdlib.h>    

const int chipSelectPin = 10;
const int WriteProtect = 9;
const String Version = "0.0.9";


void setup() {
  // initialize serial:
  Serial.begin(115200);
  pinMode(WriteProtect, OUTPUT);
  digitalWrite(WriteProtect,HIGH);
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  delay(5000);
  Serial.println("Serial SPI Console Program Version: " + Version);
  Serial.println("For help type ?");
  
}

void loop() 
{
  String s_Buffer;
  String cmd;
  String parameter[26];
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
    if (cmd.indexOf("RF") != -1)
    {
      Read_First(parameter[0].toInt());
    }
    if (cmd.indexOf("PP") != -1)
    {
      Write_Page();
    }
    if (cmd.indexOf("WREN") != -1)
    {
      SPI_WREN();
    }
    if (cmd.indexOf("WRDI") != -1)
    {
      SPI_WRDI();
    }
    
    if (cmd.indexOf("SE") != -1)
    {
      SPI_SE(parameter[0].toInt());
    }
    if (cmd.indexOf("BE") != -1)
    {
      SPI_BE(parameter[0].toInt());
    }
    if (cmd.indexOf("CE") != -1)
    {
      SPI_CE();
    }
    if (cmd.indexOf("DATA_WR") != -1)
    {
      SPI_Data_Write(parameter[0]);
    }
  }
}
void SPI_Data_Write(String Data)
{
  int L = 0;
  //DATA_WR,:10000FF000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0
  //DATA_WR,:1000000000AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA0
  //DATA_WR,:1000000000AA55AA55AA55AA55AA55AA55AA55AA55F8
  //DATA_WR,:1000000000AB65AB65AB65AB65AB65AB65AB65AB6580
  int databyte[16];
  SPI_WREN();
  delay(10);
  int checksum = 0;
  //digitalWrite(chipSelectPin, LOW);
  //SPI.transfer(0X02);
  String address = Data.substring(3,9);
  //Serial.println (address);
  char CH_Address[7];
  address.toCharArray(CH_Address,7);
  //Serial.println (CH_Address);
  int I_address = (int)strtol(CH_Address,NULL,16);
  byte address3 = I_address & 0xFF;
  byte address2 = (I_address & 0x00FF00) >> 8;
  byte address1 = (I_address & 0xFF0000) >> 16;
  //Serial.println (I_address);
  //Serial.println (I_address,HEX);
  String Data_str =Data.substring(11,43);
  //Serial.println (Data_str);  
  String checksum_str =Data.substring(43,45);
  char CH_checksum[3];
  checksum_str.toCharArray(CH_checksum,3);
  int checksumin = (int)strtol(CH_checksum,NULL,16);
  //Serial.println (checksum_str); 
  for (L =0; L<16;L++)
  {
    String temp = Data_str.substring((L*2),(L*2+2));
    //Serial.println(temp);
    char CH_data[3];
    temp.toCharArray(CH_data,3);
    databyte[L]=(int)strtol(CH_data,NULL,16);
    checksum=checksum+databyte[L];
  }
  checksum=checksum&0xFF;
  if(checksum==checksumin)
  {
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X02);
  SPI.transfer(address1);
  SPI.transfer(address2);
  SPI.transfer(address3);
  for (L = 0; L < 16;L++)
  {
    
    SPI.transfer(databyte[L]);
    Serial.println (databyte[L],HEX);
  }
  digitalWrite(chipSelectPin, HIGH);
  //SPI_WRDI();
  }
  else 
  {
    Serial.println (checksum);
    Serial.println (checksumin);
    Serial.println ("Checksum error");
  }
  Serial.println ("Done!");
}
void Write_Page()
{
  int L = 0;
  Serial.println ("Writing Data");
  Serial.println (L,HEX);
  SPI_WREN();
  delay(10);
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X02);
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(3);
  for (L = 0; L < 16;L++)
  {
    SPI.transfer(0xAA);
    Serial.println (L,HEX);
  }
  digitalWrite(chipSelectPin, HIGH);
  //SPI_WRDI();
  Serial.println ("Done!");
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
  int columns =16;
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
    int checksum =0;
    for(int k_count=0;k_count<4;k_count++)
    {
      SPI_Read_Address(0x03,k_count*1024+sector_address,address1,result,bytesToRead);
      //Serial.println(k_count*1024+sector_address);
      for (int x =0; x < rows;x++)
      {
        //Serial.print (k_count);
        //Serial.print(":");
        //Serial.print (sector_address);
        //Serial.print(":");
        //Serial.print (rows*16);
        Serial.print(":");
        
        checksum = 0;
        
        Serial.print(columns,HEX);
        char address_string[6];
        sprintf(address_string,"%.6X", k_count*1024+sector_address+x*columns);
        Serial.print(address_string);

        Serial.print("00");
        for (int y =0;y <columns;y++)
        {
          int value = result[(x*columns)+y];
          char r_string[2];
          sprintf (r_string,"%.2X",value);
          //Serial.print(",");
          //Serial.print (result[(x*rows)+y],HEX);
          //Serial.print(",");
          Serial.print(r_string);
          checksum=checksum+value;
        }
        char s_checksum[2];
        sprintf (s_checksum,"%.2X",checksum&0xFF);
        Serial.println(s_checksum);
        //Serial.println(" ");
      }
      //Serial.println(" ");
    }
    //Serial.println(" ");
  }
  Serial.println(":0000000001FF");
}
void Read_First (int sectorsToRead)
{
  int bytesToRead = 256;
  byte inByte = 0;           // incoming byte from the SPI
  byte result[bytesToRead];   // result to return
  int columns =16;
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
    int checksum =0;
    for(int k_count=0;k_count<1;k_count++)
    {
      SPI_Read_Address(0x03,k_count*bytesToRead+sector_address,address1,result,bytesToRead);
      //Serial.println(k_count*1024+sector_address);
      for (int x =0; x < rows;x++)
      {
        //Serial.print (k_count);
        //Serial.print(":");
        //Serial.print (sector_address);
        //Serial.print(":");
        //Serial.print (rows*16);
        Serial.print(":");
        
        checksum = 0;
        
        Serial.print(columns,HEX);
        char address_string[6];
        sprintf(address_string,"%.6X", k_count*1024+sector_address+x*columns);
        Serial.print(address_string);

        Serial.print("00");
        for (int y =0;y <columns;y++)
        {
          int value = result[(x*columns)+y];
          char r_string[2];
          sprintf (r_string,"%.2X",value);
          //Serial.print(",");
          //Serial.print (result[(x*rows)+y],HEX);
          //Serial.print(",");
          Serial.print(r_string);
          checksum=checksum+value;
        }
        char s_checksum[2];
        sprintf (s_checksum,"%.2X",checksum&0xFF);
        Serial.println(s_checksum);
        //Serial.println(" ");
      }
      //Serial.println(" ");
    }
    //Serial.println(" ");
  }
  Serial.println(":0000000001FF");
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
  
  address1 = (address & 0xFF0000) >> 16;

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

void SPI_WREN( )
{
  digitalWrite(WriteProtect,HIGH);
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X06);
  digitalWrite(chipSelectPin, HIGH);
}

void SPI_WRDI( )
{
  
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X04);
  digitalWrite(chipSelectPin, HIGH);
  digitalWrite(WriteProtect,HIGH);
}
void SPI_WRSR( )
{
  
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X01);
  digitalWrite(chipSelectPin, HIGH);
  digitalWrite(WriteProtect,HIGH);
}
void SPI_SE( long int address)
{
  byte address3 = address & 0xFF;
  byte address2 = (address & 0x00FF00) >> 8;
  byte address1 = (address & 0xFF0000) >> 16;

  SPI_WREN();
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X20);
  SPI.transfer(address1);
  SPI.transfer(address2);
  SPI.transfer(address3);
  digitalWrite(chipSelectPin, HIGH);
  digitalWrite(WriteProtect,HIGH);
  Serial.println("SE Done");
}
void SPI_BE( long int address)
{
  byte address3 = address & 0xFF;
  byte address2 = (address & 0x00FF00) >> 8;
  byte address1 = (address & 0xFF0000) >> 16;

  SPI_WREN();
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X52);
  SPI.transfer(address1);
  SPI.transfer(address2);
  SPI.transfer(address3);
  digitalWrite(chipSelectPin, HIGH);
  digitalWrite(WriteProtect,HIGH);
  Serial.println("BE Done");
}
void SPI_CE( )
{
  SPI_WREN();
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0X60);
  digitalWrite(chipSelectPin, HIGH);
  digitalWrite(WriteProtect,HIGH);
  Serial.println("CE Done");
}

