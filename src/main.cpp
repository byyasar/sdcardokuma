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

uint8_t buffer[256] = {'\0'};
// int index = 0;
int index2 = 0;
const int chipSelect = 10;
SdFat sd;
uint16_t yeniZaman = 0;
uint16_t eskiZaman = 0;
bool durum = false;
bool durum2 = false;

void testdrawbitmap(uint8_t bitmap[])
{
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}
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
          Serial.print(F("data:"));Serial.print(index2); Serial.println(data);
          buffer[index2] = strtol(data, NULL, 16);
          index2++;
          if (index2 == 256)
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
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (sd.begin(chipSelect))
  {
    // sdKartOku();
    Serial.println("SD ok");
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
}

void loop()
{
  yeniZaman = millis();

  if (yeniZaman - eskiZaman > 5000)
  {
    eskiZaman = yeniZaman;
    
    if(!durum2){sdKartOku();
     Serial.println(F("sd okundu"));
    }
   
    if (durum == true&&durum2==true)
    {
      testdrawbitmap(buffer);
    }
    else
    {
      Serial.println(F("SD kart okuma yapilamadi"));
    }
  }
}
