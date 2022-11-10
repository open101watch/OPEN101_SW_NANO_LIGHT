# Architecture and Firmware DESIGN
---

FIRMWARE TYPE : SoC Smart Watch<br>
FIRMWARE : OPEN101 SW NANO LIGHT

---
---

## DISPLAY DESIGN/ARCHITECTURE

```

--- SCREEN/LAYER Layout ---

TOP-->BOTTOM Execution Layer

                                 TOP
----------------------------     ||
|       BOOT LOADER        |     ||
----------------------------     ||
| ADDITIONAL SPLASH SCREEN |     ||
----------------------------     ||
|        WATCH FACE        |     ||
----------------------------     ||
|       HOME SCREEN        |     ||
----------------------------     ||
|      APP EXECUTION       |     ||
----------------------------     ||
|       APP LISTING        |     ||
----------------------------     \/
                                BOTTOM

--- DIFFERENTIATED BY SCREENS ---

TOP ----------------------<........>---------------------------- BOTTOM

------------      ------------      ------------            ------------
| SCREEN 0 | <--> | SCREEN 1 | <--> | SCREEN 2 | <<------>> | SCREEN X |
------------      ------------      ------------            ------------


--- SCREEN FUNCTIONALITY ---

------------
| SCREEN 0 | --> BOOTLOADER
------------

------------
| SCREEN 1 | --> NOTIFICATION/DATA/CHARGING SPLASH SCREEN
------------

------------
| SCREEN 2 | --> WATCH FACE
------------

------------
| SCREEN 3 | --> HOME SCREEN
------------

------------
| SCREEN 4 | --> APP EXECUTION
------------

------------
| SCREEN 5 | --> APPLICATION LISTING [POWER + MENU]
------------

```

---

```

--- APPLICATION LISTING ---

POWER
 |-SHUTDOWN
 |-RESTART
 |-HIBERNATE


MENU
 |-SETTINGS
 |  |-WATCH INFO
 |  |   |-CPU
 |  |   |-FIRMWARE
 |  |   |-STORAGE
 |  |   |-MEMORY
 |  |   |-BATTERY
 |  |   |-LEGAL INFO
 |  |
 |  |-DISPLAY
 |  |   |-RESOLUTION
 |  |   |-COLOR TYPE
 |  |   |-OTHERS
 |  |
 |  |-APPS
 |  |-SYSTEM
 |      |-DATE TIME
 |      |   |-SET DATE
 |      |   |-SET TIME
 |      |   
 |      |-CONNECTION
 |      |   |-WIFI
 |      |       |-WIFI STAT
 |      |       |-AP INFO
 |      |       |-AP CREDS
 |      |
 |      |-DEVICE ID
 |      |-POPUP
 |      |-CHARGING
 |      |-RESET
 |
 |-WATCH FACE
 |  |-ANALOG
 |  |-DIGITAL
 |  |-DUAL
 |  |-CREATIVE
 |
 |-CALENDER
 |-STOPWATCH
 |-ALARM
 |-CALCULATOR
 |-GAMES

```

---

```
--- BUTTONS ---

BTN0    BTN1

BTN3    BTN2

-> BTN-ALLIGNMENT

    0: TOP LEFT
    1: TOP RIGHT
    2: BOTTOM RIGHT
    3: BOTTOM LEFT

-> BTN-CLICK SUPPORT

    All Buttons are Multifunctional supporting
    0. None
    1. Single Click
    2. Double Click
    3. Tripple Click
    4. Quadruple Click
    5. Hold
    6. Single Click and Hold

    BTN(0,1,2,3,...n) => All are Multi Functional Keys
        Supporting assignment of mult Functions & Feedbacks


----------
MORE TO GO
----------
```



---
### OPEN101 SW NANO LIGHT DESIGN