# Architecture and Firmware DESIGN
---

FIRMWARE TYPE : SoC Smart Watch<br>
FIRMWARE : OPEN101 SW NANO LIGHT

---
---

## DISPLAY DESIGN/ARCHITECTURE

```

---------------
| BOOT LOADER |
----------------------------
| ADDITIONAL SPLASH SCREEN |
----------------------------
| WATCH FACE |
---------------
| HOME SCREEN |
-----------------
| APP EXECUTION |
---------------
| APP LISTING |
---------------

--- DIFFERENTIATED BY SCREENS ---

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



--- MENU LISTING ---

POWER
 |-SHUTDOWN
 |-RESTART
 |-HIBERNATE


MENU
 |-SETTINGS
 |	|-WATCH INFO
 |	|   |-CPU
 |  |   |-FIRMWARE
 |  |   |-STORAGE
 |  |   |-MEMORY
 |  |   |-BATTERY
 |  |   |-LEGAL INFO
 |  |  
 |	|-DISPLAY
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
 |	|-ANALOG
 |	|-DIGITAL
 |	|-DUAL
 |	|-CREATIVE
 |
 |-CALENDER
 |-STOPWATCH
 |-ALARM
 |-CALCULATOR
 |-GAMES



----------
MORE TO GO
----------
```



---
### OPEN101 SW NANO LIGHT