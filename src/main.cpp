#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define LOGO_HEIGHT 32
#define LOGO_WIDTH 128
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define EEPROM_I2C_ADDRESS 0x50 // EEPROM I2C Address
const byte interruptPin = 2;    // kesme pini

int page = 0;
const int maxaddress = 128;
byte ram_array[maxaddress];
byte readVal = 0;
// uint8_t buffer[256] = {'\0'};
//  int index = 0;
int index2 = 0;
const int chipSelect = 10;
SdFat sd;
uint16_t yeniZaman = 0;
uint16_t eskiZaman = 0;
bool durum = false;
bool durum2 = false;
// byte Image [256];
void testdrawbitmap(uint8_t bitmap[])
{
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}
void writeEEPROM(int address, byte val, int i2c_address)
{
  // Begin transmission to I2C EEPROM
  Wire.beginTransmission(i2c_address);

  // Send memory address as two 8-bit bytes
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB

  // Send data to be stored
  Wire.write(val);

  // End the transmission
  Wire.endTransmission();

  // Add 5ms delay for EEPROM
  delay(50);
}
byte readEEPROM(int address, int i2c_address)
{
  // Define byte for received data
  byte rcvData = 0xFF;

  // Begin transmission to I2C EEPROM
  Wire.beginTransmission(i2c_address);

  // Send memory address as two 8-bit bytes
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB

  // End the transmission
  Wire.endTransmission();

  // Request one byte of data at current memory address
  Wire.requestFrom(i2c_address, 1);

  // Read the data and assign to variable
  rcvData = Wire.read();

  // Return the data as function output
  return rcvData;
}
void test_eeprom(int page)
{
  // Print to Serial Monitor
  //  Serial.println("Start Writing...");

  //   display.clearDisplay();
  //   display.setTextSize(1);             // Normal 1:1 pixel scale
  //   display.setTextColor(SSD1306_WHITE);        // Draw white text
  //   display.setCursor(0, 0);            // Start at top-left corner
  //   display.println(F("Writing to EEPROM..."));
  //   display.display();

  //   for (int address = page*maxaddress; address < (page*maxaddress)+maxaddress; address++){
  //     writeEEPROM(address, ram_array[address-(page*maxaddress)], EEPROM_I2C_ADDRESS);
  //     Serial.print("ADDR = ");
  //     Serial.print(address);
  //     Serial.print("   data:");
  //     Serial.println(ram_array[address], HEX);
  //     delay(1);
  //   }

  //   // Print to Serial Monitor
  //   Serial.println("Writing Finished!");
  //   delay(2000);    // Delay 2 Seconds

  // Print to Serial Monitor
  //Serial.println("Begin Reading...");

  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(F("Reading from EEPROM.."));
  display.display();

  // Run until maximum address is reached
  for (int address = page * maxaddress; address < (page * maxaddress) + maxaddress; address++)
  {
    // Read value from EEPROM
    readVal = readEEPROM(address, EEPROM_I2C_ADDRESS);

    // Print to Serial Monitor
    //   Serial.print("ADDR = ");
    //   Serial.print(address);
    //    Serial.print("   data:");
    //   Serial.println(readVal, HEX);
    // Serial.print("ADDRsss = ");
    //  Serial.println(address-(page*maxaddress));
    ram_array[address - (page * maxaddress)] = readVal;
    delay(1);
  }

  // Print to Serial Monitor
//  Serial.println("Reading Finished!");
  testdrawbitmap(ram_array);
} // end test_eeprom

void sdKartOku()
{
  File dataFile = sd.open("kodlama.txt");
  // if the file is available, write to it:
  if (dataFile)
  {
    char dizi[4] = {'\0'};
    int diziindex = 0;

    while (dataFile.available())
    {
      char c = dataFile.read();
      if (c != ' ' && c != '{' && c != ',' && c != '\n' && c != '\r' && c != '}')
      {
        dizi[(diziindex) % 4] = c;
        if ((diziindex) % 4 == 3)
        {
          char data[4] = {'\0'};
          sprintf(data, "%c%c%c%c", dizi[0], dizi[1], dizi[2], dizi[3]);
          // Image[index2] = strtol(data, NULL, 16);
          writeEEPROM(index2, ram_array[index2 - (page * maxaddress)], EEPROM_I2C_ADDRESS);
          // Serial.print(F("data:"));
          // Serial.print(index2);
          // Serial.println(data);
          //ram_array[index2] = strtol(data, NULL, 16);
          index2++;
          if (index2 == maxaddress)
          {
            break;
          }
          diziindex = 0;
        }
        else
        {
          diziindex++;
        }
      }
    }
    dataFile.close();
    durum = true;
    durum2 = true;
  }
}
void blink()
{
   display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(F("Sd kart okunuyor.."));
  display.display();

 
  //Serial.println(F("sdden eeproma yaz"));
  
  sdKartOku();
  //delay(500);
  // memcpy_P(ram_array, Image, maxaddress);

  // for (int address = page*maxaddress; address < (page*maxaddress)+maxaddress; address++){
  //   writeEEPROM(address, ram_array[address-(page*maxaddress)], EEPROM_I2C_ADDRESS);
  //   Serial.print("ADDR = ");
  //   Serial.print(address);
  //   Serial.print("   data:");
  //   Serial.println(ram_array[address], HEX);
  //   delay(1);
  // }

  // // Print to Serial Monitor
  // Serial.println("Writing Finished!");
  // delay(2000);    // Delay 2 Seconds
  test_eeprom(page);
}

void setup()
{
  Wire.begin();
   if (sd.begin(chipSelect))
  {
    // sdKartOku();
    Serial.println(F("SD ok"));
  }

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    // for (;;)
    //   ; // Don't proceed, loop forever
  }
  else
  {
    Serial.println(F("SSD1306 ok"));
    display.clearDisplay();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Hello, world!"));
    display.display();
  }
  // memcpy_P(ram_array, Image, maxaddress);// copy the image from flash memory to RAM
  delay(50);
   test_eeprom(page);
  //blink();
}

void loop()
{
  // yeniZaman = millis();

  // if (yeniZaman - eskiZaman > 5000)
  // {
  //   eskiZaman = yeniZaman;

  //   if (!durum2)
  //   {
  //     //sdKartOku();
  //     Serial.println(F("sd okundu"));
  //   }

  //   if (durum == true && durum2 == true)
  //   {
  //     testdrawbitmap(buffer);
  //   }
  //   else
  //   {
  //     Serial.println(F("SD kart okuma yapilamadi"));
  //   }
  // }
}
