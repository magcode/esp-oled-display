# esp-oled-display
An ESP8266 / SSD1322 based IOT OLED Display

It shows inside and outside temperature, current time and whatever status text you like.

# Hardware & Wiring
Hardware needed:
NodeMCU
OLED Display 3.12" 256*64 SSD1322

The display must be soldered for "4 SPI"


Wiring
```
NodeMCU           - Display
----------------------------------------
D1 GPIO5          - PIN 15 / RESET
D2 GPIO4          - PIN 14 / DC
D7 HMOSI (GPIO13) - PIN 5  / SDIN (D1)
D8 HCS   (GPIO15) - PIN 16 / CS	
D5 HSCLK (GPIO14) - PIN 4  / SCLK (D0)
3.3V              - PIN 2
GND               - PIN 1
```


# Building
I use VSC/Platform IO

Make sure `#define U8G2_16BIT` is uncommented in `/lib/U8g2/src/clib/u8g2.h`

# Controlling via MQTT

Send a JSON structure to the topic defined in `Secrets.h`
```
{
  inside:23, 
  outside:18, 
  time:"23:33", 
  data:"Whatever", 
  contrast:255
}
```

You can disable the display by sending
```
{
  disable:true, 
}
```
