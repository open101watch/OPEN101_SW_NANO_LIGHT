// [I2C OLED]
// I2C GND <--> Microcontroller GND
// I2C Vcc <--> Microcontroller 3.3V/5.0V

// [Arduino UNO]
// I2C SCK <--> Arduino UNO A5
// I2C SDA <--> Arduino UNO A4

// [ESP8266]
// I2C SCK <--> ESP8266 D1
// I2C SDA <--> ESP8266 D2

// [ESP32]
// I2C SCK <--> ESP32 D22
// I2C SDA <--> ESP32 D21

#include "user_interface.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MD5Builder.h"

/// PROJECT INTERNAL HEADERS
#include "src\\libraries\\cryptography.cpp"
CRYPTOGRAPHY crypto;
#include "src\\libraries\\time.cpp"
TIME Time;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SSD1306_NO_SPLASH

///// TYPE Defination
typedef struct char32 { char c[32]; } char32;



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int c_screen_ID; // Current Screen ID
int pre_screen_ID; // Previous Screen ID
// int current_WF[3] = {0, 1, 0}; // {(0: Digital WF/1:Analog WF), WF_id, active_status}
// int temp_WF[3]; // {(0: Digital WF/1:Analog WF), WF_id, active_status}
int total_watch_face[3]={2,2,0}; //0:Total Digital WF | 1:Analog WF | 2:Dual WF
int WiFi_config[8][6]={{192, 168, 101, 41}, {80}}; // {{WiFi IP}, <WiFi Open Port>, WiFi MAC Address, WiFi Status, dns, gateway, subnet}
byte WiFi_MAC[6]; 
char WiFi_credentials[3][32]={"OPEN101 SW NANO LIGHT", "sys41x4"};//{Device SSID, Device PASSWORD}
const char* const WiFi_modes[] = { "NULL", "STA", "AP", "STA+AP" };
const char* const WiFi_phymodes[] = { "", "B", "G", "N" };


// DATE TIME
int time_array[] = {53,59,11,0,0,10,0,10,2022,0}; // ss,mm,hh,meridiem{0:AM,1:PM},<HR-TYPE>,dd,ww,month,yyyy,isLeapYear
int tmp_date_time[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // {D,D,M,M,Y,Y,Y,Y,<HR-TYPE>,H,H,M,M,S,S,<AM/PM>,<position>}
int tmp_calender_date[3]; //{DD,MM,YYYY}
char week_days[7][10]={"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
//// App Listing
// int apps_to_list[] = {0,1,2};
// int app_list_active_status[] = {0,0,1};

// // INDEX : Screen TYPE
// // 0 : Info Splash Screen
// // 1 : Battery Splash Screen
// // 2 : Watch Face Screen
// // 3 : Home Screen
// // 4 : Power Apps Screen
// // 5 : Menu Apps Screen
// // 6 : Menu Options Screen
// // Initial Values : {0,0,0,0,0,0,0}
// // Switching values to 1 will active the Screen Layers
// // Screen Checks are done from left to right 0-->6
// // On Boot : it is checked if the device is charging
// //            if so then screen_id[1]=1
// //            and it will show battery splash screen
// //           if not then screen_id[2]=1
// //            and it will show Watch Face Screen
// int screen_id[7] = {0,0,0,0,0,0,0};

// Battery Variables
int battery_input_pin = 0; //A0 | Analog


// Temporary
int timezone = 7 * 3600;
int dst = 0;

// int time_array[] = {0,0,0,0,0,0,0,0,0}; // ss,mm,hh,meridiem{0:AM,1:PM},dd,ww,month,yyyy,isLeapYear
// int time_array[] = {53,58,11,0}; // ss,mm,hh,{0:AM,1:PM}
// int time_array[] = {53,59,11,0,0,0,1,2022,0}; // ss,mm,hh,meridiem{0:AM,1:PM},dd,ww,month,yyyy,isLeapYear
int month30[] = {4,6,9,11}; // November, April, June, and September
int month31[] = {1,3,5,7,8,10,12}; // January, March, May, July, August, October, and December

// Charging Fill Circle
int charge_fill_radi[] = {0, 0, 50}; // {uncharged_battery_%, normal/reverse, charged_battery_%}

// Special Splash Screen required variables
// int charging_splash_screen_4_circle[] ={0,0};
int charging_splash_screen_4_circle = 0;
int charging_splash_screen_5_circle = 0;

// https://diyusthad.com/image2cpp

static const uint8_t  PROGMEM logo_bmp [] = {
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

static const uint8_t  PROGMEM bt_icon_8x8 [] = {
  0x9c, 0x53, 0x33, 0x3c, 0x3c, 0x33, 0x53, 0x9c
};

static const uint8_t  PROGMEM charging_16x16 [] = {
  0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x60, 0x00, 0xe0, 0x01, 0xc0, 0x07, 0x80, 0x0f, 0xc0,
  0x03, 0xe0, 0x01, 0xc0, 0x03, 0x80, 0x07, 0x00, 0x06, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00,
};

static const uint8_t  PROGMEM settings_32x32 [] = {
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x02, 0x07, 0xe0, 0x40,
  0x07, 0x0f, 0xf0, 0xe0, 0x0f, 0xbf, 0xfd, 0xf0, 0x1f, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xf0,
  0x07, 0xff, 0xff, 0xe0, 0x03, 0xfe, 0x7f, 0xc0, 0x07, 0xf0, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xe0,
  0x0f, 0xc0, 0x03, 0xf0, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0x80, 0x01, 0xff,
  0xff, 0x80, 0x01, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0x0f, 0xc0, 0x03, 0xf0,
  0x07, 0xe0, 0x07, 0xe0, 0x07, 0xf0, 0x0f, 0xe0, 0x03, 0xfe, 0x7f, 0xc0, 0x07, 0xff, 0xff, 0xe0,
  0x0f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf8, 0x0f, 0xbf, 0xfd, 0xf0, 0x07, 0x0f, 0xf0, 0xe0,
  0x02, 0x07, 0xe0, 0x40, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00
};










///////////////////////
// --- WATCH FACES ---
///////////////////////

class Digital_WF {
  public:
//    total_watch_face[0]=2; //0: Total Number of programmed Digital Watch Face

    void WF_0(int x, int y, int w, int h, int hh, int international_clk_format, int active){


      // international_clk_format => 0 : 12-hr clock
      //                             1 : 24-hr clock

      display.setTextColor(active^1, active);
      display.setTextSize(1);
      
      char *time;

      if(time_array[4]==1){
        display.setCursor(x,y+(h/2)-10);
      }
      else{
        display.setCursor(x+8,y+(h/2)-10);
      }
      
      asprintf(&time, "%02d:%02d:%02d", time_array[2], time_array[1], time_array[0]);
      display.print(time);
      free(time);


      // If Current Clock is selected as 12-HR Clock then
      if(time_array[4]==1){

        if(time_array[3]>=1){
          display.print("PM");
        }
        else{
          display.print("AM");
        }
      }


      // Print Date

      char *date;

      display.setCursor(x,y+(h/2)+2);
      
      
      asprintf(&date, "%02d.%02d.%04d", time_array[5], time_array[7], time_array[8]);
      display.print(date);
      free(date);

    }


    void WF_1(int x, int y, int w, int h, int hh, int international_clk_format, int active){


      // international_clk_format => 0 : 12-hr clock
      //                             1 : 24-hr clock

      display.setTextColor(active^1, active);

      char *time;
      display.setTextSize(2);
      display.setCursor(x,y+24);
      asprintf(&time, "%02d:%02d", hh, time_array[1]);
      display.print(time);
      free(time);


      // Display Second
      display.setTextSize(1);
      if(time_array[4]==1){
        display.setCursor(x+(w/2)-20,y+48);
      }
      else{
        display.setCursor(x+(w/2)-8,y+48);

      }

      char *ss;
      asprintf(&ss, "%02d", time_array[0]);
      display.print(ss);
      free(ss);

      if(time_array[4]==1){
        display.setCursor(x+(w/2)+4,y+48);

        if(time_array[3]>=1){
          display.print("PM");
        }
        else{
          display.print("AM");
        }
      }


      // Print Date

      char *date;

      display.setCursor(x,y+(h/2)+2);
      
      
      asprintf(&date, "%02d.%02d.%04d", time_array[5], time_array[7], time_array[8]);
      display.print(date);
      free(date);

      
    }
};


///////////////////////
// --- ANALOG FACES ---
///////////////////////

class Analog_WF {

  private:

    void draw_second_hand(int x, int y, int r, int active){
      // (x,y) => coordinate of center point of the circle
      // display second hand
      float angle = time_array[0]*6 ;
      angle=(angle/57.29577951) ; //Convert degrees to radians
      int x3=(x+(sin(angle)*(r)));
      int y3=(y-(cos(angle)*(r)));
      display.drawLine(x, y,x3,y3,active^1);
    }

    void draw_minute_hand(int x, int y, int r, int active){
      // display minute hand
      float angle = time_array[1] * 6 ;
      angle=(angle/57.29577951) ; //Convert degrees to radians
      int x3=(x+(sin(angle)*(r-3)));
      int y3=(y-(cos(angle)*(r-3)));
      display.drawLine(x, y,x3,y3,active^1);
    }

    void draw_hour_hand(int x, int y, int r, int active){
      // (x,y) => coordinate of center point of the circle

      // display hour hand
      float angle = time_array[2] * 30 + int((time_array[1] / 12) * 6 );
      angle=(angle/57.29577951) ; //Convert degrees to radians
      int x3=(x+(sin(angle)*(r-11)));
      int y3=(y-(cos(angle)*(r-11)));
      display.drawLine(x, y,x3,y3,active^1);
    }

    void draw_hour_ticks(int x, int y, int r, int active){
      // (x,y) => coordinate of center point of the circle
      //hour ticks
      for( int z=0; z < 360;z= z + 30 ){
        //Begin at 0° and stop at 360°
        float angle = z ;
        
        angle=(angle/57.29577951) ; //Convert degrees to radians
        int x2=(x+(sin(angle)*r));
        int y2=(y-(cos(angle)*r));
        int x3=(x+(sin(angle)*(r-5)));
        int y3=(y-(cos(angle)*(r-5)));
        display.drawLine(x2,y2,x3,y3,active^1);
      }
    }

  public:

//    total_watch_face[1]=2; //0: Total Number of programmed Digital Watch Face


    void WF_comm0(int x, int y, int r, int active){

      // (x,y) => coordinate of center point of the circle
      display.drawCircle(x, y, r, active^1);

      draw_hour_ticks(x, y, r, active); // Draw Hour Ticks

      // Draw Second Hand
      draw_second_hand(x, y, r, active);
      
      // Draw Minuite Hand
      draw_minute_hand(x, y, r, active);
      
      // Draw Hour Hand
      draw_hour_hand(x, y, r, active);
    }

    void WF_0(int x, int y, int w, int h, int international_clk_format, int active){
      // (x,y) => coordinate of top left point of Analog Watch Face Frame

      // international_clk_format => 0 : 12-hr clock
      //                             1 : 24-hr clock
      int r=w/2;

      WF_comm0(x+(w/2), y+(h/2), r-1, active);
    }

    void WF_1(int x, int y, int w, int h, int international_clk_format, int active){
      // (x,y) => coordinate of top left point of Analog Watch Face Frame

      // international_clk_format => 0 : 12-hr clock
      //                             1 : 24-hr clock
      int r=w/2;

      display.setCursor(x+(w/2), y);
      display.print("12");

      WF_comm0(x+(w/2), y+(h/2), r-1, active);
    }
};



///////////////////////
// --- WATCH FACES ---
///////////////////////
Digital_WF D_WF;
Analog_WF A_WF;
int current_WF[3] = {0, 1, 0};// {0, 1, 0}; | To be Stored In EEPROM Section after every Watch Face Change
int temp_WF[3]; // {(0: Digital WF/1:Analog WF), WF_id, active_status}
// current_WF[0]==0? Digital Watch Face
// current_WF[0]==1? Analog Watch Face
void watch_face(int screen_type, int id, int international_clk_format){

  // time_array : Temporary Variable for calculating current time {ss,mm,hh,...}
  int hh;
  if(screen_type==0){
    if(international_clk_format==0){
      
      // 12 hr FORMAT
      if(time_array[2]==0){
        hh=12;
      }
      else if(time_array[2]<12){
        hh=time_array[2]; 
      }
      else if(time_array[2]>12){
        hh=time_array[2]-12; 
      }
    }
    else{
      hh = time_array[2];
    }


    if(id==0){
      D_WF.WF_0(0, 0, 64, 128, hh, international_clk_format, 0);
    }
    else if(id==1){
      D_WF.WF_1(0, 0, 64, 128, hh, international_clk_format, 0);
    }
  }
  else if(screen_type==1){
    if(id==0){
      A_WF.WF_0(0, 0, 64, 128, international_clk_format, 0);
    }
    else if(id==1){
      A_WF.WF_1(0, 0, 64, 128, international_clk_format, 0);
    }
  }

};


class BTN_FUNC{

  private:

    // btn_id => INDEX of {BTN0, BTN1, BTN2, BTN3}

    unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

    // l_btn_S : last button state | {BTN0, BTN1, BTN2, BTN3}
    int l_btn_S[4]={1,1,1,1};   // the previous reading from the input pin

    // btn_S : Button State | {BTN0, BTN1, BTN2, BTN3}
    int btn_S[4]; //saving state of the switch

    // tap_C : Tap Counter | {BTN0, BTN1, BTN2, BTN3}
    int tap_C[4]; //for saving no. of times the switch is pressed

    // T_diff : Time Difference | {BTN0, BTN1, BTN2, BTN3}
    int T_diff[4]; //for saving the time in between each press and release of the switch

    // l_debounce_T : Last Debounce Time | {BTN0, BTN1, BTN2, BTN3}
    unsigned long l_debounce_T[4]={0,0,0,0};  // the last time the output pin was toggled

    // Follow UP Variables | {BTN0, BTN1, BTN2, BTN3}
    int flag0[4];
    int flag1[4];

    // presstime : Save Press Time | {BTN0, BTN1, BTN2, BTN3}
    long double presstime[4]; //for saving millis at press
    // releasetime : Save Press Time | {BTN0, BTN1, BTN2, BTN3}
    long double releasetime[4]; //for saving millis at release

    

  public:

    // D3-GPIO0-FLASH [POWER]
    // D6-GPIO12 [Action]
    // D7-GPIO13 [BACK]
    // D5-GPIO14 [FORWARD]

    // 0 1
    // 3 2

    // BTN PIN ASSISMENT
    // Assigned GPIO PIN | {BTN0_GPIO_PIN, BTN1_GPI1_PIN, BTN2_GPI2_PIN, BTN3_GPI3_PIN}
    int btn_gpio[4]={0,12,13,14};

    // 0 : Btn InActive
    // 1 : Single Tap
    // 2 : Double Tap
    // 3 : Triple Tap
    // 4 : 4 Tap
    // 5 : Hold
    // 6 : Single Tap & Hold
    int btn_action_count[4];

    
  
    void btn_reset(){
      for(int i=0;i<4;i++){
        btn_action_count[i]=0;
      }
    }


    void btn_state_chk(){
      // Check for all 4 BUTTON STATE and ACTION Count
      for(int i=0;i<4;i++){
        multi_func(i);
        // Serial.print(btn_func.btn_action_count[i]);
      };
    }

    void multi_func(int btn_id){
      int reading = digitalRead(btn_gpio[btn_id]);


      if (reading != l_btn_S[btn_id]) {
        // reset the debouncing timer
        l_debounce_T[btn_id] = millis();
      }

      if ((millis() - l_debounce_T[btn_id]) > debounceDelay) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != btn_S[btn_id]) {
          btn_S[btn_id] = reading;
        }
      }
      //Serial.println(buttonState);

      //when switch is pressed
      if (btn_S[btn_id] == 0 && flag1[btn_id] == 0)
      {
        presstime[btn_id] = millis(); //time from millis fn will save to presstime variable
        flag0[btn_id] = 0;
        flag1[btn_id] = 1;
        tap_C[btn_id]++; //tap counter will increase by 1
        //delay(10); //for avoiding debouncing of the switch
      }
      //when sw is released
      if (btn_S[btn_id] == 1 && flag0[btn_id] == 0)
      {
        releasetime[btn_id] = millis(); //time from millis fn will be saved to releasetime var
        flag0[btn_id] = 1;
        flag1[btn_id] = 0;

        T_diff[btn_id] = releasetime[btn_id] - presstime[btn_id]; //here we find the time gap between press and release and stored to timediff var
        //Serial.println(timediff);
        //delay(10);
      }

      if ((millis() - presstime[btn_id]) > 400 && btn_S[btn_id] == 1) //wait for some time and if sw is in release position
      {
        if (tap_C[btn_id] == 1) //if tap counter is 1
        {
          if (T_diff[btn_id] >= 400) //if time diff is larger than 400 then its a hold
          {
            Serial.println("Hold");
            Serial.println(btn_gpio[btn_id]);
            btn_action_count[btn_id]=5;
    //        hold(); //fn to call when the button is hold
          }
          else //if timediff is less than 400 then its a single tap
          {
            Serial.println("single tap");
            Serial.println(btn_gpio[btn_id]);
            btn_action_count[btn_id]=1;
          }
        }
        else if (tap_C[btn_id] == 2 ) //if tapcounter is 2
        {
          if (T_diff[btn_id] >= 400) // if timediff is greater than  400 then its single tap and hold
          {
            Serial.println("single tap and hold");
            Serial.println(btn_gpio[btn_id]);
            btn_action_count[btn_id]=6;
          }
          else // if timediff is less than 400 then its just double tap
          {
            Serial.println("double tap");
            Serial.println(btn_gpio[btn_id]);
            btn_action_count[btn_id]=2;
          }
        }
        else if (tap_C[btn_id] == 3) //if tapcounter is 3 //then its triple tap
        {
          Serial.println("triple tap");
          Serial.println(btn_gpio[btn_id]);
          btn_action_count[btn_id]=3;
        }
        else if (tap_C[btn_id] == 4) //if tapcounter is 4 then its 4 tap
        {
          Serial.println("four tap");
          Serial.println(btn_gpio[btn_id]);
          btn_action_count[btn_id]=4;
        }
        tap_C[btn_id] = 0;
      }
      l_btn_S[btn_id] = reading;
    }
};


class FIRMWARE{
  public:
    char* getSketchMD5(){
      static char md5_sig_buff[33];

      ESP.getSketchMD5().toCharArray(md5_sig_buff, 33); // Length of md5 hash is 32      
      
      return md5_sig_buff;
    }

};


FIRMWARE firmware;


///////////////////////
// Charging Functions//
///////////////////////

class Charging_SS {
  private:


//      static const uint8_t  PROGMEM charging_icon_0_16x16[] = {
//        0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x60, 0x00, 0xe0, 0x01, 0xc0, 0x07, 0x80, 0x0f, 0xc0,
//        0x03, 0xe0, 0x01, 0xc0, 0x03, 0x80, 0x07, 0x00, 0x06, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00,
//      };

    int anim_outer_circle_covered = 0; // Particularly for Anim_2
    // int temp_charging_face_id;
    // int     


    void animate_charging_SS(int bat_charge){

      // Increase count after 1 frame image generation
      // Must Be included in every animate function

      if(bat_charge!=100 || charge_fill_radi[0]!=100){
        if(charge_fill_radi[1]==0){
          charge_fill_radi[0]++;
        }
        else if(charge_fill_radi[1]==1){
          charge_fill_radi[0]--;
        }  
      }
      


    }

    void D_CLOCK0(int x, int y){
      // Digital Clock to show in Charging Splash Screen
      // Current Time
      display.setTextSize(1);
      char *time;

      if(time_array[4]==1){
        display.setCursor(x,y);
      }
      else{
        display.setCursor(x+8,y);
      }

      asprintf(&time, "%02d:%02d:%02d", time_array[2], time_array[1], time_array[0]);
      display.print(time);
      free(time);


      // If Current Clock is selected as 12-HR Clock then
      if(time_array[4]==1){

        if(time_array[3]>=1){
          display.print("PM");
        }
        else{
          display.print("AM");
        }
      }
      
      
    }

    void bat_percent_text0(int x, int y, int text_size, int charge_percent){
      // Battery % Show
      display.setTextSize(text_size);
      display.setCursor(x,y);
      
      char *bat_percent;
      asprintf(&bat_percent, "%d%s", charge_percent, "%");
      display.print(bat_percent);
      free(bat_percent);

    }
    void charging_text(int x, int y, int text_size){
      // Display Charging Text
      display.setTextSize(text_size);
      display.setCursor(x,y);
      display.print("CHARGING");
    }

    // Animations

    void Anim_0(int x, int y, int l, int active, int charge_percent){
      // Bottom Animation for charging %


      if (charge_fill_radi[0]==100){
        charge_fill_radi[1]=1;
      }
      else if (charge_fill_radi[0]==charge_percent || charge_fill_radi[0]<=0){
        charge_fill_radi[1]=0;
      }
      display.fillCircle(32, 88, (charge_fill_radi[0]*(l/2))/100, active^1);

      animate_charging_SS(charge_percent);
    }

    void Anim_1(int x, int y, int charge_logo_coord_y, int l, int active, int charge_percent){

      if (charge_fill_radi[0]==100){
        charge_fill_radi[1]=1;
      }
      else if (charge_fill_radi[0]==charge_percent || charge_fill_radi[0]<=0){
        charge_fill_radi[1]=0;
      }
      display.fillCircle(32, 88, (charge_fill_radi[0]*(l/2))/100, active^1);

      // Charging Status ICON | Inside Circle Animation
      if (charge_fill_radi[0]<=1){
        display.drawBitmap(25, charge_logo_coord_y, charging_16x16, 16, 16, active^1);
      }
      else{
        display.drawBitmap(25, charge_logo_coord_y, charging_16x16, 16, 16, active);
      }

      animate_charging_SS(charge_percent);

    }

    void Anim_2(int x, int y, int charge_logo_coord_y, int l, int active, int charge_percent){

      if (charge_fill_radi[0]==100){
        charge_fill_radi[1]=1;
        if(anim_outer_circle_covered==1){
          anim_outer_circle_covered=0;
        }
        else{
          anim_outer_circle_covered=1;
        }
      }
      else if (charge_fill_radi[0]==charge_percent || charge_fill_radi[0]<=0){
        charge_fill_radi[1]=0;
      }

      if(charge_percent>=100){
        anim_outer_circle_covered=0;
      }
      if(anim_outer_circle_covered==1){
        display.fillCircle(32, 88, 31, active^1);
      }
      display.fillCircle(32, 88, (charge_fill_radi[0]*31)/100, active^anim_outer_circle_covered^1);

      // Charging Status ICON | Inside Circle Animation
      if (charge_fill_radi[0]<=1){
        display.drawBitmap(25, charge_logo_coord_y, charging_16x16, 16, 16, active^1);
      }
      else{
        if(anim_outer_circle_covered==1){
          display.drawBitmap(25, charge_logo_coord_y, charging_16x16, 16, 16, active^1);
        }
        else{
          display.drawBitmap(25, charge_logo_coord_y, charging_16x16, 16, 16, active);
        }
        
      }
      animate_charging_SS(charge_percent);

    }

  public:

    int charging_face_conf[3] = {6, 4, 0}; // {<Total_charging_faces>, <Current_charging_face_id>, <temp_charging_face_id>}

    void F_0(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 0
      
      bat_percent_text0(16, 8, 2, charge_percent);
      
      // Charging Status ICON
      display.drawBitmap(24, 28, charging_16x16, 16, 16, 1);
     
     // Digital Clock
      D_CLOCK0(0, 48);

      // Circle Animation
      Anim_0(x+(w/2), y+(h/2)+(h/4), w, 0, charge_percent);
  }

  void F_1(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 1
      
      bat_percent_text0(8, 16, 3, charge_percent);
     
     // Digital Clock
      D_CLOCK0(0, 48);

      // Circle Animation
      Anim_1(x+(w/2), y+(h/2)+(h/4), 81, w, 0, charge_percent);
  }


  void F_2(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 2
    bat_percent_text0(16, 8, 2, charge_percent);

    charging_text(8, 32, 1);

    // Digital Clock
    D_CLOCK0(0, 48);

    // Circle Animation
    Anim_1(x+(w/2), y+(h/2)+(h/4), 81, w, 0, charge_percent);
  }


  void F_3(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 3
    bat_percent_text0(8, 16, 3, charge_percent);

    // Digital Clock
    D_CLOCK0(0, 48);

    // Circle Animation
    Anim_2(x+(w/2), y+(h/2)+(h/4), 81, w, 0, charge_percent);
  }

  void F_4(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 4
    bat_percent_text0(16, 8, 2, charge_percent);

    charging_text(8, 32, 1);

    // Digital Clock
    D_CLOCK0(0, 48);

    // Circle Animation
    Anim_2(x+(w/2), y+(h/2)+(h/4), 81, w, 0, charge_percent);
  }

  void F_5(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 4
    bat_percent_text0(8, 16, 3, charge_percent);

    // Digital Clock
    D_CLOCK0(0, 48);

    // Circle Animation
    Anim_1(x+(w/2), h, 104, w, 0, charge_percent);
  }

  void F_6(int x, int y, int w, int h, int charge_percent){   
    // Charging Splash Screen 4
    bat_percent_text0(16, 8, 2, charge_percent);

    charging_text(8, 32, 1);

    // Digital Clock
    D_CLOCK0(0, 48);

    // Circle Animation
    Anim_1(x+(w/2), h, 104, w, 0, charge_percent);
  }
};


Charging_SS C_SS; // Define Variable for storing the class variables


void splash_screen(int screen_type, int id, int charge_percent){
  // screen_type -> N : Notification Splash Screen
  //                C : 0: Charging Splash Screen
  //                A : Alarm Splash Screen

  if(screen_type==0){
    if(id==0){
      C_SS.F_0(0, 0, 64, 128, charge_percent);
    }
    else if(id==1){
      C_SS.F_1(0, 0, 64, 128, charge_percent);
    }
    else if(id==2){
      C_SS.F_2(0, 0, 64, 128, charge_percent);
    }
    else if(id==3){
      C_SS.F_3(0, 0, 64, 128, charge_percent);
    }
    else if(id==4){
      C_SS.F_4(0, 0, 64, 128, charge_percent);
    }
    else if(id==5){
      C_SS.F_5(0, 0, 64, 128, charge_percent);
    }
    else if(id==6){
      C_SS.F_6(0, 0, 64, 128, charge_percent);
    }
  }
}



class HOME_PAGE{
  public:
    void page(int x, int y, int length, int active){
      
    }
};



/////////////////////////////
// --- LOGO GENERATION --- //
/////////////////////////////

class Logo_Gen {
// Vector Images designing
  public:
    void back(int x, int y, int length, int active){
      // Default length: 63
      // BACK BUTTON
      display.fillRect(x, y, length, length+1, active);
      display.setTextColor(active^1);
      display.setCursor(x+(length/8),y+(length/2)-2);
      display.print("BACK");
    }


    void settings(int x, int y, int length, int active){
      // Default length: 63
      display.fillRect(x, y, length, length+1, active);
        // display.clearDisplay();
        display.setTextColor(active^1);
        display.setTextSize(1);

      if(length>=59){
        // display.setCursor(x+(length/8),y+(length/2)-(length/16));
        // display.print("SETTINGS");
        
        display.drawBitmap(x+(length/4), y+(length/8), settings_32x32, 32, 32, active^1);
      }
      else{
        display.setCursor(x+(length/8),y+(length/4));
        display.print("SETT");
        display.setCursor(x+(length/8),y+(length/2));
        display.print("INGS");
      }
    }

    void calculator(int x, int y, int length, int active){
      // Default Length: 63
      display.fillRect(x, y-1, length, length+1, active);
      display.setTextColor(active^1);
      display.setTextSize(1);

      display.drawRect(x+(length/4)-(length/8), y, length-(length/4)+2, length-(length/4), active^1);

      if(length>=59){
        display.setCursor((length/4)-(length/16),y+(length/16));
        display.print("1+0+1=2");

        display.fillRect(x+(length/8), y+(length/4), length-(length/4)+2, length-(length/4)-(length/4), active^1);

        display.setTextColor(active);
        display.setCursor((length/4)-(length/16),y+(length/4)+(length/32));
        display.print("6 7 8 9");
        display.setCursor((length/4)-(length/16),y+(length/2)-(length/16));
        display.print("2 3 4 5");
        display.setCursor((length/4)-(length/16),y+(length/2)+(length/8));
        display.print("C E 0 1");
      }
      else{
        display.setCursor(x+(length/4),y+(length/4)-4);
        display.print("+ -");
        display.setCursor(x+(length/4),y+(length/4)+2);
        display.print("x %");
      }
    }

    void calender(int x, int y, int length, int active){
      // Default Length: 63
      display.fillRect(x, y, length+1, length+1, active);
      display.setTextColor(active);
      display.setTextSize(1);

      // display.drawRect(x+(length/4)-10, y, length-(length/4)+10, length-(length/4), 1);
      display.fillRect(x+(length/8)-2, y+2, length-(length/4)+(length/8)+2, length-(length/8), active^1);

      if(length>=59){
        display.setCursor((length/4)-(length/8),y+(length/16)+1);
        display.print("SEPTEMBER");
      }
      
      display.fillRect(x+(length/4)-(length/8), y+(length/4), length-(length/4)+(length/16)+2, length-(length/4)-(length/4)+2, active);

      display.setTextColor(active^1);

      display.setCursor(x+(length/2)-2,y+(length/4)+(length/16));
      display.print("THU");

      display.setTextSize(length/21);
      display.setCursor(x+(length/4)-(length/16),y+(length/4)+(length/16));
      display.print("5");
    }


    void alarm_clock(int x, int y, int length, int active){
      // display.clearDisplay();
      // // Create Background Graphics
      // Create Alarm Clock Fins

      display.fillRect(x, y-1, length, length+1, active);

      display.fillCircle(x+(length/4)-(length/16)+2, y+(length/8)+1, (length/8)+1, active^1);
      display.fillCircle(x+length-(length/4)+(length/16)+2, y+(length/8)+1, (length/8)-1, active^1);
      
      display.fillCircle(x+(length/2), y+(length/2), (length/2)-(length/8)+2, active);

      display.fillRect(x+(length/8), y+length-(length/16)-1, length-(length/4), length/16, active^1);
      // display.display();

      // Show Default Analog Face
      // A_WF_0(x+(length/2), y+(length/2), (length/2)-(length/8), active);
      A_WF.WF_comm0(x+(length/2), y+(length/2), (length/2)-(length/8), active);
      
      
    }

    void stopwatch(int x, int y, int length, int active){
      // display.clearDisplay();
      // // Create Background Graphics
      // Create Alarm Clock Fins

      display.fillRect(x, y, length, length+2, active);

      display.fillRect(x+(length/2)-(length/8), y+2, length/4, (length/16)+1, active^1);
      display.fillRect(x+(length/2)-(length/18), y+3, (length/16)+2, length/8, active^1);

      // Show Default Analog Face
      A_WF.WF_comm0(x+(length/2), y+(length/2), (length/2)-(length/8), active);

      
      
    }

    void shutdown(int x, int y, int length, int active){

      display.fillRect(x, y, length, length+1, active);

      display.fillCircle(x+(length/2), y+(length/2), (length/2)-2, active^1);
      display.fillCircle(x+(length/2), y+(length/2), (length/2)-8-2, active);

      display.fillRect(x+(length/4), y+2, length/2, (length/2)-2, active);
      display.fillRect(x+(length/2)-(length/8), y+2, length/4, length/2, active^1);

    }

    void restart(int x, int y, int length, int active){

      display.fillRect(x, y, length, length, active);

      display.fillCircle(x+(length/2), y+(length/2), (length/2)-2, active^1);
      display.fillCircle(x+(length/2), y+(length/2), (length/2)-10, active);

      display.fillRect(x+(length/4), y, length/2, length/2, active);

      display.fillTriangle(x+(length/2)+(length/4), y, x+(length/2)+(length/4), y+(length/2)-(length/8), x+(length/2)-4, y+(length/4)-3, active^1);
    }

    void hibernate(int x, int y, int length, int active){

      display.fillRect(x, y, length, length, active);

      display.fillCircle(x+(length/4)+4, y+(length/2), (length/2)-2, active^1);
      display.fillCircle(x+4, y+(length/2), (length/2)-2, active);

      display.fillRect(x-(length/2), y, length/2, length, 0);
    }

};



/////////////////////////
// APP CONTENT LISTING //
/////////////////////////

class App_content_option{
  private:
    int item_box_h = 14; // item bounded box height
    
  public:
    // int current_item_id = 0; //[0,1,2,3,...]

    // APP Options
    // 0: SETTINGS OPTIONS
    
    char page[40][20] = {

    "ON","OFF",

    "Watch Info", "Display", "Apps", "System",

    "CPU", "FIRMWARE", "STORAGE", "MEMORY", "BATTERY", "LEGAL INFO",

    "Resolution", "Color Type", "OTHERS",

    "SHUTDOWN", "RESTART", "HIBERNATE", 

    "SETTINGS", "WATCH FACE", "CALENDER", "STOPWATCH", "ALARM", "CALCULATOR", "GAMES",

    "Date Time", "Connection", "Device ID", "POPUP", "CHARGING", "Reset",

    "Digital", "Analog", "Dual",

    "WiFi",

    "WiFi Stat", "AP INFO", "AP Creds",

    "SET DATE", "SET TIME"

    }; // Settings APP Options
    // int page_bound[5]={0,6,10,13};// 0: 0-7 | 1: 9-10

    int page_layouts[50]={0,1,  2,5,  6,11,  12,14,  15,17,  18,24,  25,30,  31,33, 34,34, 35,37,  38,39};

    int current_page_layout[3];//{page_num, page_index_start, item_number}
    int previous_page_layout[3];//{page_num, page_index_start, item_number}
    int pre_previous_page_layout[3];//{page_num, page_index_start, item_number}

    // int current_page_bound[2];// [0]: index start | [1]: Index End
    int current_page_id;

    // APP OPTION FUNCTIONS

    void create_page_layout(int n, int page_num, int item_num){
      if(n==0){
        current_page_layout[0]=page_num;
        current_page_layout[1]=page_layouts[current_page_layout[0]*2];
        current_page_layout[2]=item_num;
      }
      else if(n==1){
        for(int i=0;i<3;i++){
          previous_page_layout[0]=page_num;
          previous_page_layout[1]=page_layouts[current_page_layout[0]*2];
          previous_page_layout[2]=item_num;
        }
      }
    }

    void list_bg(int x, int y, int w, int h, int item_highlight_id){
      // int item_box_h = 14;
      int box_cnt = 0;

      for(int i=y;i<=h;i+=item_box_h){
        if(box_cnt==item_highlight_id){
          display.fillRect(x, i, w, item_box_h, 1);
        }
        else{
          display.drawRect(x, i, w, item_box_h, 1); // Bounded Box arround options
        }
        box_cnt++;
      }
    }

    void list_items(int x, int y){
      // option_array => 2d char array [[max_char:20],[max_char:20],...max_char:n]
      // n[max] = 8; n[min]=1;
      // Draw Bounded boxes
      list_bg(0,14,63, 114, current_page_layout[2]);

      display.setTextSize(1);
      int d=y+2;

      for(int i=current_page_layout[1];i<=page_layouts[(current_page_layout[0]*2)+1];i++){
        if(i==current_page_layout[1]+current_page_layout[2]){
          display.setTextColor(0);  
        }
        else{
          display.setTextColor(1);
        }
        
        display.setCursor(x+2,d+2);
        char *option;
        asprintf(&option, "%.10s", page[i]); // 10 char max per line
        display.print(option);
        free(option);
        d+=item_box_h;
      }

    }


    void view_page(int page_num, int item_highlight_id){
      // option_array => 2d char array [[max_char:20],[max_char:20],...max_char:n]
      // n[max] = 8; n[min]=1;
      // Draw Bounded boxes

      int x=0, y=14;

      list_bg(0,14,63, 114, item_highlight_id);

//      int n=sizeof(option_array)/sizeof(option_array[0]); // count Number of options
      // int n0=page_bound[page_id]; //=8; // MAX option items per page/screen
      // int n1=page_bound[page_id+1];


      // current_page_layout[0] = 0;
      // current_page_layout[1] = app_item_list.page_layouts[current_page_layout[0]][0];
      // current_page_layout[1] = app_item_list.page_layouts[current_page_layout[0]][1];
      // current_page_layout[3] = app_item_list.current_item_id;

      display.setTextSize(1);
      int d=y+2;

      for(int i=page_layouts[page_num*2];i<=page_layouts[(page_num*2)+1];i++){
        if(i==page_layouts[page_num*2]+item_highlight_id){
          display.setTextColor(0);  
        }
        else{
          display.setTextColor(1);
        }
        
        display.setCursor(x+2,d+2);
        char *option;
        asprintf(&option, "%.10s", page[i]); // 10 char max per line
        display.print(option);
        free(option);
        d+=item_box_h;
      }

    }
};


App_content_option app_item_list;



void list_app_contents(int movement){
  // screen_type => 0: settings app options
  // internal variable for current position id, current_item_id <- app_options
  // movement => 0: Stay in current position
  //             1: increment app_options.current_item_id by 1
  //             -1: decrement app_options.current_item_id by 1


  if(movement==1){
      app_item_list.current_page_layout[2]++;
  }
  else if(movement==-1){
    app_item_list.current_page_layout[2]--;
  }
  if(app_item_list.current_page_layout[2]>app_item_list.page_layouts[(app_item_list.current_page_layout[0]*2)+1]-app_item_list.current_page_layout[1]){
    app_item_list.current_page_layout[2]=0;
  }
  if(app_item_list.current_page_layout[2]<0){
    app_item_list.current_page_layout[2]=app_item_list.page_layouts[(app_item_list.current_page_layout[0]*2)+1]-app_item_list.current_page_layout[1];
  }
  int x=0, y=14;

  app_item_list.list_items(x,y);
  
};




BTN_FUNC btn_func;



// App option Listing
Logo_Gen logo_gen;

class App_option{

  public: 
    void drawCentreString(const String &buf, int x, int y)
    {   
        display.setTextColor(1);
        display.setTextSize(1);
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
        display.setCursor(x - w / 2, y);
        display.print(buf);
    }

    // public accessable variables
    int previous_app_GID; // Previous APP GROUP ID
    int current_app_GID; // Current APP GROUP ID
    int previous_app_id = 1; // Previous APP ID | GROUP ID should be mentioned at first
    int current_app_id = 1; // Current APP ID | GROUP ID should be mentioned at first
    int power_apps_count = 4; // POWER APPS COUNT | Including Back app icon | {<'P':0>, <0,1,2...>} => {<GROUP ID>, <APP ID>}
    int menu_apps_count = 6; // MENU APPS COUNT | Including Back app icon | {<'M':1>, <0,1,2...>} => {<GROUP ID>, <APP ID>}


    // show App options


    void list_apps(int screen_type, int current_item){

      // Total app count calculation
      int total_items;
      if( screen_type==0){
        // Selection for POWER APPS
        total_items = power_apps_count;
      }
      else if(screen_type==1){
        // Selection for MENU APPS
        total_items = menu_apps_count;
      }

      // App Movement
      if(current_item==(total_items)){
        current_app_id=1;
      }
      else if(current_item==-1){
        current_app_id=total_items-1;
      }
      int apps_to_list[3] = {current_item-1,current_item,current_item+1};

      int app_list_active_status[3] = {0,1,0};
//      app_list(3, apps_to_list, app_list_active_status, screen_type, 0, 0, 63, 127);
  }

    
};


App_option app_options;


void screen_switch(int btn_id, int btn_state, int screen_id){
  if(btn_func.btn_action_count[btn_id]==btn_state){
    c_screen_ID=screen_id;
  }
}




class RUN_APP{

  private:

    
  
  public:
    // char *string_to_print; // Used only for displaying string with black BG

    // {level, page_num} | {0,0}: "Watch Info", "Display", "Apps", "System", "Sound", "Faces",
    // {level, page_num} | {1,1}: "CPU", "FIRMWARE", "STORAGE", "MEMORY", "BATTERY",
    // {level, page_num} | {1,2}: "Resolution", "Color Type", "OTHERS",
    int settings_item_list_levels;
    int running_final_application = 0;
    int option_movement=0; // 0: current position | 1: current_position+1 | -1: current_position-1
    // 
    // 0: frame_update | Used for dynamic data visual handling
    // 1: frame_skips | Display only 1 frame | Used for static data visual handling 
    int app_display_handling[2]; //{app_id, fram_display_type}
    int list_items_display_state; //0: display list_items | else no display
    // 0: POWER MENU
    // 1: MENU


    //////////////////////
    /// APPS Execution ///
    //////////////////////

    void shutdown(){
      // Execute Shutdown
      running_final_application=1;
      display.clearDisplay();
      print_string("Powering  Off\n\nWithin 5  Seconds");
      display.display();
      delay(5000);
      display.clearDisplay();
      display.display();
      ESP.deepSleep(0);
    }

    void restart(){
      // Execute Restart
      running_final_application=1;
      display.clearDisplay();
      print_string("Restarting\n\nWithin 5  Seconds");
      display.display();
      delay(5000);
      display.clearDisplay();
      display.display();
      ESP.restart();
    }

    void hibernate(){
      // Execute Hibernation
      running_final_application=1;
      display.clearDisplay();
      print_string("Hibernating\n\nWithin 5  Seconds");
      display.display();
      // Write State in EEPROM Space | Later will be replaced with temporary storage
      EEPROM.write(0, crypto.bin2hex("10000000"));
      EEPROM.commit();
      delay(5000);
      display.clearDisplay();
      display.display();
      ESP.deepSleep(0);
    }

    void cpu_info(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%s\n\n%s\n%d\n\n%s\n%d\n\n%s\n%d\n\n%s\n%d MHz", 
      "SDK", ESP.getSdkVersion(),
      "CHIP ID", ESP.getChipId(),
      "CHIP SPEED", ESP.getFlashChipSpeed(),
      "CHIP MODE", ESP.getFlashChipMode(),
      "CPU FREQ", ESP.getCpuFreqMHz()
      );
      
      print_string(string_to_print);
      free(string_to_print);

    }

    void firmware_info(){
      running_final_application=1;
      char *string_to_print;
      // char md5_sig_buff[33];

      // ESP.getSketchMD5().toCharArray(md5_sig_buff, 33); // Length of md5 hash is 32
      Serial.println(firmware.getSketchMD5());
      asprintf(&string_to_print, "%s\n\n%s\n%s\n%s\n%s\n\n%s\n%s",
      "OPEN101 SW NANO LIGHT",
      "FIRMWARE", "v1.0",
      "LAST UPDATE", "22.10.2022",
      "MD5 SIGNATURE", firmware.getSketchMD5()
      );

      print_string(string_to_print);
      free(string_to_print);

    }


    void storage_info(){
      running_final_application=1;
      char *string_to_print;
      
      asprintf(&string_to_print, "%s\n\n%s\n%d\n%s\n%d",
      "OPEN101 SW NANO LIGHT",
      "ALLOCATED", ESP.getSketchSize(),
      "REMAINING", ESP.getFreeSketchSpace()
      );

      print_string(string_to_print);
      free(string_to_print);

    }


    void device_id(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%s", 
      "DEVICE ID", "2a97516c354b68848cdbd8f54a226a0a55b21ed138e207ad6c5cbb9c00aa5aea"
      );
      
      print_string(string_to_print);
      free(string_to_print);
    }

    void wifi_stat(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%s\n%s:%s\n%s:%d\n%s:%d\n%s:%d\n%s:%d",
        "MODE", WiFi_modes[WiFi.getMode()],
        "PHY MODE", WiFi_phymodes[(int) wifi_get_phy_mode()],
        "CHANNEL", wifi_get_channel(),
        "AP ID", wifi_station_get_current_ap_id(),
        "Status", wifi_station_get_connect_status(),
        "Auto Con", wifi_station_get_auto_connect()
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void wifi_ap_info(){
      running_final_application=1;
      char *string_to_print;
      WiFi.macAddress(WiFi_MAC);
      asprintf(&string_to_print, "%s\n%d\n\n%s\n%.10s\n%s\n%.10s\n\n%s\n%x:%x:%x:%x:%x:%x\n%s\n%d.%d.%d.%d\n%s\n%d", 
       "WiFi State", WiFi.status(),
       "WiFi SSID", WiFi_credentials[0],
       "WiFi PASS", WiFi_credentials[1],
       "MAC Addr", WiFi_MAC[0], WiFi_MAC[1], WiFi_MAC[2], WiFi_MAC[3], WiFi_MAC[4], WiFi_MAC[5],
       "IP Address", WiFi_config[0][0], WiFi_config[0][1], WiFi_config[0][2], WiFi_config[0][3],
       "Host Port", WiFi_config[1][0]
      
      );
      
      print_string(string_to_print);
      free(string_to_print);
    }

    void wifi_creds(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%s\n\n%s\n%s",
        "WiFi SSID", WiFi_credentials[0],
        "WiFi PASS", WiFi_credentials[1]
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void display_resolution_info(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%dx%d px\n%s\n%s",
        "Resolution", SCREEN_HEIGHT, SCREEN_WIDTH,
        "Display Library", "SSD1306"
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void display_colortype_info(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s\n%s\n\n%s\n%s",
        "Colour Type", "MONOCHROME",
        "Display Library", "SSD1306"
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void default_more_info(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s:\n%s\n\n%s:\n%s\n\n%s:\n%s %s",
        "MORE INFO", "https://sys41x4.github.io",
        "Developer", "Arijit Bhowmick [sys41x4]",
        "Contact", "open101watch@gmail.com", "sys41x4@gmail.com"
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void default_legal_info(){
      running_final_application=1;
      char *string_to_print;
      asprintf(&string_to_print, "%s:\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s:\n%s",
        "MORE INFO", "https://sys41x4.github.io",
        "WATCH NAME", "OPEN101 SW NANO LIGHT", 
        "Copyright", "GPLv3",
        "ORIGIN", "INDIA",
        "Developer", "Arijit Bhowmick"
      );

      print_string(string_to_print);
      free(string_to_print);
    }

    void digital_WF(){

      btn_WF_relocate(2, 1, 0, 1);
      btn_WF_relocate(3, 1, 0, -1);
      btn_tempWF_to_currentWF_set(2, 5);

      watch_face(temp_WF[0], temp_WF[1], temp_WF[2]);

    }

    void analog_WF(){

      btn_WF_relocate(2, 1, 1, 1);
      btn_WF_relocate(3, 1, 1, -1);
      btn_tempWF_to_currentWF_set(2, 5);

      watch_face(temp_WF[0], temp_WF[1], temp_WF[2]);

    }


    void charging_Face(){

      btn_CF_relocate(2, 1, 1);
      btn_CF_relocate(3, 1, -1);
      btn_tempCF_to_currentCF_set(2, 5);

      splash_screen(0, C_SS.charging_face_conf[2], 50);

    }

    void dual_WF(){

    }

    void set_time(){
      running_final_application=1;

      int x=0,y=0;
      int initial_index=8;

     
      display.setTextSize(1);
      display.setTextColor(1);
      display.setCursor(x,y);
      // HEADING
      display.println("SET TIME");
      display.drawLine(x, y+10,SCREEN_HEIGHT,y+10,1);

      // SET DATE | PRINT DATE SECTION
      for(int i=0;i<=8;i++){
        display.setTextColor(1);
        display.setTextSize(1);

        if(i==0){
          display.println("\nHOUR CLOCK\n");
        }
        if(i==7 && tmp_date_time[8]==1){
          display.println("\nMERIDIEM");
        }

        // // Partitioners
        // if(i==3 || i==5){
        //   display.setTextColor(1);
        //   display.setTextSize(1);
        //   // display.setCursor(x+(i-9)*8,y+8*4);
        //   display.print(":");
        // } 

        // Highlighting settings

        if(tmp_date_time[16]==i+8){
          display.setTextColor(0, 1); // 'inverted' text colour
        }


        if(i==0){
          // SET HR Format
          // display.setCursor(x,y+8*3);
//          display.setTextSize(2);
          if(tmp_date_time[8]==0){
            display.println("24\n");
          }
          else{
            display.println("12\n");
          }
        }
        if(i>=1 && i<=6){
          // SET TIME
          // if(i==2 || i==5){
          //   // display.print(tmp_date_time[i]);
          //   display.setCursor(x+(i-9)*2*8,y+8*4);
          // }
          // else{
          //   display.setCursor(x+(i-9)*8,y+8);
          // }
//          display.setTextSize(2);
          display.print(tmp_date_time[i+8]);

        }
        if(i==7 && tmp_date_time[8]==1){
          // IF 12-HR FORMAT is set
          // display.setCursor(x+(i-9)*8,y+8);
//          display.setTextSize(2);
          if(tmp_date_time[15]==0){
            display.println("\nAM");
          }
          else{
            display.println("\nPM");
          }
        }


        if(i==8){

          display.drawLine(x, SCREEN_WIDTH-10,SCREEN_HEIGHT,SCREEN_WIDTH-10,1);
          display.setCursor((SCREEN_HEIGHT/2)-8,(SCREEN_WIDTH-8));

          if(tmp_date_time[16]==-1){
            display.setTextColor(0, 1); // 'inverted' text colour
          }
          display.setTextSize(1);
          display.print("OK");
        }

        display.setTextColor(1);
        if(i==2){
          display.println(" HOUR");
        }
        if(i==4){
          display.println(" MINUTE");
        }
        if(i==6){
          display.println(" SECOND");
        }


        
      }


      if(btn_func.btn_action_count[2]==1){

        if(tmp_date_time[16]==-1){
          tmp_date_time[16]=initial_index;
        }
        else{
         if(tmp_date_time[16]==initial_index || tmp_date_time[16]==initial_index+7){
            tmp_date_time[tmp_date_time[16]]=tmp_date_time[tmp_date_time[16]]^1;
          }
          else{
            if(tmp_date_time[tmp_date_time[16]]>=9){
                tmp_date_time[tmp_date_time[16]]=0;
            }
            else{
              tmp_date_time[tmp_date_time[16]]++;
            }
          } 
        }
        
      }
      else if(btn_func.btn_action_count[3]==1){

        if(tmp_date_time[16]==-1){
          tmp_date_time[16]=initial_index+6+tmp_date_time[8];
        }
        else{
          if(tmp_date_time[16]==initial_index || tmp_date_time[16]==initial_index+7){
            tmp_date_time[tmp_date_time[16]]=tmp_date_time[tmp_date_time[16]]^1;
          }
          else{
            if(tmp_date_time[tmp_date_time[16]]<=0){
                tmp_date_time[tmp_date_time[16]]=9;
            }
            else{
              tmp_date_time[tmp_date_time[16]]--;
            }
          } 
        }  
      }


      else if(btn_func.btn_action_count[2]==2){
        if(tmp_date_time[16]>=initial_index+6+tmp_date_time[8]){
            tmp_date_time[16]=8;
        }
        else{
          tmp_date_time[16]++;
        }
      }
      else if(btn_func.btn_action_count[3]==2){
        if(tmp_date_time[16]<=initial_index){
          tmp_date_time[16]=initial_index+6+tmp_date_time[8];
//            tmp_date_time[16]=15;
        }
        else{
          tmp_date_time[16]--;
        }
      }


      else if(btn_func.btn_action_count[2]==3){
        tmp_date_time[16]=initial_index+6+tmp_date_time[8];
        
      }
      else if(btn_func.btn_action_count[3]==3){
        tmp_date_time[16]=initial_index;
      }

      else if(btn_func.btn_action_count[2]==5){
        if(tmp_date_time[16]==-1){
          // Check if the provided format and time data
          // is correct (Time Data & Format Check)
          // If So then
          // Update current dateTime and restart device
          // Else Show Incorrect Time data Warning & show current progress in the app

          // tmp_date_time[16]=-1;
          if(Time.verify_clock_format((tmp_date_time[initial_index+1]*10)+tmp_date_time[initial_index+2], 
                                 (tmp_date_time[initial_index+3]*10)+tmp_date_time[initial_index+4],
                                 (tmp_date_time[initial_index+5]*10)+tmp_date_time[initial_index+6],
                                 tmp_date_time[initial_index]
                                )==1
            ){

            // is verification success then

            // Assign to current time_array | seconds
            time_array[0] = (tmp_date_time[initial_index+5]*10)+tmp_date_time[initial_index+6];

            // Assign to current time_array | minute
            time_array[1] = (tmp_date_time[initial_index+3]*10)+tmp_date_time[initial_index+4];

            // Assign to current time_array | Hour
            time_array[2] = (tmp_date_time[initial_index+1]*10)+tmp_date_time[initial_index+2];

            time_array[4] = tmp_date_time[initial_index];

            if(tmp_date_time[initial_index]==1){
              time_array[3] = tmp_date_time[initial_index+7];
            }

            // After assigning to current time_array display message about successfully change
            display.clearDisplay();
            print_string("Successfully Updated Current Time :)");
            display.display();
            delay(2000);


          }

          else{
            // If the Supplied format is incorrect then
            display.clearDisplay();
            print_string("Incorrect Time Format :(");
            display.display();
            delay(2000);
          }

        }
        else{
          tmp_date_time[16]=-1;
        }
      }

    }


    void set_date(){
      running_final_application=1;

      int x=0,y=0;
      int initial_index=0;

     
      display.setTextSize(1);
      display.setTextColor(1);
      display.setCursor(x,y);
      // HEADING
      display.println("SET DATE\n\n");
      display.drawLine(x, y+10,SCREEN_HEIGHT,y+10,1);

      // SET DATE | PRINT DATE SECTION
      for(int i=0;i<=8;i++){
        display.setTextColor(1);
        display.setTextSize(1);


        if(i==8){

          display.println("\n");
          //// print Week Day
          if(Time.verify_date_format(
              (tmp_date_time[initial_index]*10)+tmp_date_time[initial_index+1], 
              (tmp_date_time[initial_index+2]*10)+tmp_date_time[initial_index+3], 
              (tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7]
            )==1
            ){

              display.println(week_days[Time.get_week_day(
                (tmp_date_time[initial_index]*10)+tmp_date_time[initial_index+1], 
                (tmp_date_time[initial_index+2]*10)+tmp_date_time[initial_index+3], 
                (tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7]
              )]);

              if(Time.chk_leap_year(
                (tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7]
                )==1){
    
                  display.println("\nLEAP YEAR");
              }
          }

          
        
        }


        // Highlighting settings

        if(tmp_date_time[16]==i){
          display.setTextColor(0, 1); // 'inverted' text colour
        }



        if(i>=0 && i<=7){
          // SET DATE

          display.print(tmp_date_time[i]);

        }
//         


        if(i==8){

          display.drawLine(x, SCREEN_WIDTH-10,SCREEN_HEIGHT,SCREEN_WIDTH-10,1);
          display.setCursor((SCREEN_HEIGHT/2)-8,(SCREEN_WIDTH-8));

          if(tmp_date_time[16]==-1){
            display.setTextColor(0, 1); // 'inverted' text colour
          }
          display.setTextSize(1);
          display.print("OK");
        }

        display.setTextColor(1);
        if(i==1){
          display.println(" DAY");
        }
        if(i==3){
          display.println(" MONTH");
        }
        if(i==7){
          display.println(" YEAR");
        }


        
      }


      if(btn_func.btn_action_count[2]==1){

        if(tmp_date_time[16]==-1){
          tmp_date_time[16]=initial_index;
        }
        else{
         if(tmp_date_time[tmp_date_time[16]]>=9){
              tmp_date_time[tmp_date_time[16]]=0;
          }
          else{
            tmp_date_time[tmp_date_time[16]]++;
          }
        }
        
      }
      else if(btn_func.btn_action_count[3]==1){

        if(tmp_date_time[16]==-1){
          tmp_date_time[16]=initial_index+7;
        }
        else{
          if(tmp_date_time[tmp_date_time[16]]<=0){
              tmp_date_time[tmp_date_time[16]]=9;
          }
          else{
            tmp_date_time[tmp_date_time[16]]--;
          }
        }
      }


      else if(btn_func.btn_action_count[2]==2){
        if(tmp_date_time[16]>=initial_index+7){
            tmp_date_time[16]=initial_index;
        }
        else{
          tmp_date_time[16]++;
        }
      }
      else if(btn_func.btn_action_count[3]==2){
        if(tmp_date_time[16]<=initial_index){
          tmp_date_time[16]=initial_index+7;
//            tmp_date_time[16]=15;
        }
        else{
          tmp_date_time[16]--;
        }
      }


      else if(btn_func.btn_action_count[2]==3){
        tmp_date_time[16]=initial_index+7;
        
      }
      else if(btn_func.btn_action_count[3]==3){
        tmp_date_time[16]=initial_index;
      }

      else if(btn_func.btn_action_count[2]==5){
        if(tmp_date_time[16]==-1){
          // Check if the provided format and time data
          // is correct (Time Data & Format Check)
          // If So then
          // Update current dateTime and restart device
          // Else Show Incorrect Time data Warning & show current progress in the app

          // tmp_date_time[16]=-1;
          if(Time.verify_date_format(
              (tmp_date_time[initial_index]*10)+tmp_date_time[initial_index+1], 
              (tmp_date_time[initial_index+2]*10)+tmp_date_time[initial_index+3], 
              (tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7]
            )==1
            ){

            // is verification success then

            // Assign to current time_array | DAY
            time_array[5] = (tmp_date_time[initial_index]*10)+tmp_date_time[initial_index+1];

            // Assign to current time_array | MONTH
            time_array[7] = (tmp_date_time[initial_index+2]*10)+tmp_date_time[initial_index+3];

            // Assign to current time_array | YEAR
            time_array[8] = (tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7];

            // Assign to current time_array | YEAR
            time_array[9] = Time.chk_leap_year((tmp_date_time[initial_index+4]*1000)+(tmp_date_time[initial_index+5]*100)+(tmp_date_time[initial_index+6]*10)+tmp_date_time[initial_index+7]);

            // After assigning to current time_array display message about successfully change
            display.clearDisplay();
            print_string("Successfully Updated Current DATE :)");
            display.display();
            delay(2000);


          }

          else{
            // If the Supplied format is incorrect then
            display.clearDisplay();
            print_string("Incorrect DATE Format :(");
            display.display();
            delay(2000);
          }

        }
        else{
          tmp_date_time[16]=-1;
        }
      }

    }



    void show_calender(){
      running_final_application=1;
      display.setTextSize(1);
      int x=0,y=0;
      display.setRotation(2); // Rotate 180 Degree Clockwise
      display.setCursor(x,y);
      
      // Print header/heading
      display.setTextColor(0, 1); // 'inverted' text colour
      display.print("Su");

      display.setTextColor(1);
      display.println(" Mo Tu We Th Fr Sat\n");

      int month_start_day = Time.get_week_day(1, tmp_calender_date[1], tmp_calender_date[2]);
      
      if(month_start_day>0){
        display.setTextColor(0,0);
        
        for(int i=0;i<month_start_day;i++){
          char *tmp;
          asprintf(&tmp, "%d ",(i+1)*10);
          display.print(tmp);
          free(tmp);
        }
      }
      

      


      for(int i=1;i<=Time.get_total_days_of_month(tmp_calender_date[1], tmp_calender_date[2]);i++){
        display.setTextColor(1);
        
        if(i==tmp_calender_date[0]){
          display.setTextColor(0, 1); // 'inverted' text colour
        }
        if(i==time_array[5] && tmp_calender_date[1]==time_array[7] && tmp_calender_date[2]==time_array[8]){
          display.setTextColor(0, 1); // 'inverted' text colour
        }

        
        char *month_day;
        if(i>=10){
           asprintf(&month_day, "%d",i);
        }
        else{
          asprintf(&month_day, " %d",i);
        }
        display.print(month_day);
        free(month_day);
        display.setTextColor(1);
        display.print(" ");
      }
      

      display.drawLine(x, y+10,SCREEN_WIDTH,y+10,1);

      

      if(btn_func.btn_action_count[2]==1){
        if(tmp_calender_date[0]>=Time.get_total_days_of_month(tmp_calender_date[1], tmp_calender_date[2])){
          tmp_calender_date[0]=1;
        }
        else{
          tmp_calender_date[0]++;
        }
      }

      if(btn_func.btn_action_count[3]==1){
        if(tmp_calender_date[0]<=1){
          tmp_calender_date[0]=Time.get_total_days_of_month(tmp_calender_date[1], tmp_calender_date[2]);
        }
        else{
          tmp_calender_date[0]--;
        }
        
      }

      if(btn_func.btn_action_count[2]==2){
        if(tmp_calender_date[1]>=12){
          tmp_calender_date[1]=1;
        }
        else{
          tmp_calender_date[1]++;
        }

      }

      if(btn_func.btn_action_count[3]==2){
        if(tmp_calender_date[1]<=1){
          tmp_calender_date[1]=12;
        }
        else{
          tmp_calender_date[1]--;
        }
      }

      if(btn_func.btn_action_count[2]==3){
        tmp_calender_date[2]++;
      }

      if(btn_func.btn_action_count[3]==3){
        if(tmp_calender_date[2]<=1){
          tmp_calender_date[2]=time_array[8];
        }
        else{
          tmp_calender_date[2]--;
        }
      }

      if(btn_func.btn_action_count[2]==4){
        display.setCursor(0,0);
        display.setTextColor(0, 1); // 'inverted' text colour
        display.setTextSize(2);

        char *string_to_print;
        asprintf(&string_to_print, "%.02d:%.02d:%.04d\n%.02d:%.02d:%.04d",
          time_array[5], time_array[7], time_array[8],
          tmp_calender_date[0], tmp_calender_date[1], tmp_calender_date[2]
        );

        display.println(string_to_print);
        free(string_to_print);
        display.display();
        delay(3000); // Delay for 3 seconds | for user's msg visibility

        
      }

      if(btn_func.btn_action_count[3]==4){

        tmp_calender_date[0]=time_array[5];
        tmp_calender_date[1]=time_array[7];
        tmp_calender_date[2]=time_array[8];
      }



      if(btn_func.btn_action_count[3]==5){
        display.setRotation(1); // Rotate 90 Degree Clockwise
      }

    }


    void alarm(){
      running_final_application=1;
      print_string("Will be released in Future Updates \nPLEASE STAY CONNECTED\n\n:)");
    }

    void calculator(){
      running_final_application=1;
      print_string("Will be released in Future Updates \nPLEASE STAY CONNECTED\n\n:)");
    }

    void games(){
      running_final_application=1;
      print_string("Will be released in Future Updates \nPLEASE STAY CONNECTED\n\n:)");
    }

    /////////////////////////////////////////////

    void print_string(const String &buf){
      display.setTextColor(1);
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print(buf);
      // asprintf(&string_to_print,"");
    }


    void app_execute(int app_id){
      if(app_id==0){
        shutdown();
      }
      else if(app_id==1){
        restart();
      }
      else if(app_id==2){
        hibernate();
      }
      else if(app_id==3){
        cpu_info();
      }
      else if(app_id==4){
        firmware_info();
      }
      else if(app_id==5){
        digital_WF();
      }
      else if(app_id==6){
        analog_WF();
      }
      else if(app_id==7){
        dual_WF();
      }
      else if(app_id==8){
        device_id();
      }
      else if(app_id==9){
        wifi_stat();
      }
      else if(app_id==10){
        wifi_ap_info();
      }
      else if(app_id==11){
        wifi_creds();
      }
      else if(app_id==12){
        display_resolution_info();
      }
      else if(app_id==13){
        display_colortype_info();
      }
      else if(app_id==14){
        default_more_info();
      }
      else if(app_id==15){
        default_legal_info();
      }
      else if(app_id==16){
        set_time();
      }
      else if(app_id==17){
        set_date();
      }
      else if(app_id==18){
        show_calender();
      }
      else if(app_id==19){
        storage_info();
      }
      else if(app_id==20){
        charging_Face();
      }
      else if(app_id==21){
        alarm();
      }
      else if(app_id==22){
        calculator();
      }
      else if(app_id==23){
        games();
      }


      // pre_screen_ID=c_screen_ID;
    }
    

    void btn_tempWF_to_currentWF_set(int btn_id, int btn_state){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        for(int i;i<3;i++){
          current_WF[i]=temp_WF[i];
        }

        print_string("CURRENT\nWATCH FACE\nCHANGED :)");
        display.display();
        delay(2000);
      }
    }

    void btn_temp_WF_set(int btn_id, int btn_state, int WF_type, int WF_id, int active){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        temp_WF[0]=WF_type;
        temp_WF[1]=WF_id;
        temp_WF[2]=active;
      }
    }



    void btn_tempCF_to_currentCF_set(int btn_id, int btn_state){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        C_SS.charging_face_conf[1]=C_SS.charging_face_conf[2];

        print_string("CURRENT\nCharging FACE\nCHANGED :)");
        display.display();
        delay(2000);
      }
    }

    void btn_temp_CF_set(int btn_id, int btn_state, int WF_id){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        C_SS.charging_face_conf[2]=WF_id;
      }
    }

    void btn_page_reset(int btn_id, int btn_state){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        app_item_list.current_page_layout[0]=-1;
        app_item_list.current_page_layout[1]=-1;
        app_item_list.current_page_layout[2]=-1;
      }

    }

    void btn_switch2prev_screen_ID(int btn_id, int btn_state){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        c_screen_ID=pre_screen_ID;
        running_final_application=0;
        list_items_display_state=0;
      }
    }

    void btn_app_exe_info_set(int btn_id, int btn_state, int app_id, int frame_refresh_type){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        // app_execute(app_id);
        pre_screen_ID=c_screen_ID;
        c_screen_ID=4;
        app_display_handling[0]=app_id;
        app_display_handling[1]=frame_refresh_type;
        
      }
    }

    void btn_page_hook(int btn_id, int btn_state, int page_num, int item_id){

      if(btn_func.btn_action_count[btn_id]==btn_state){
        app_item_list.create_page_layout(0, page_num, item_id);
      }
    }

    int page_compare(int page_num, int item_id){

      if(app_item_list.current_page_layout[0]==page_num && 
         app_item_list.current_page_layout[2]==item_id
        ){
        return 1;
      }
      else{
        return 0;
      }
    }

    int page_common_back(int page_num){

      if(app_item_list.current_page_layout[0]==page_num){
        return 1;
      }
      else{
        return 0;
      }
    }

    void display_list_items_frame(){
      if(list_items_display_state==0){
        display.clearDisplay();
          app_item_list.view_page(app_item_list.current_page_layout[0], app_item_list.current_page_layout[2]);
        display.display();
        list_items_display_state=1;
      }
    }

    void btn_switch_list_display_state(int btn_id, int btn_state, int display_state){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        list_items_display_state=display_state;
      }
    }

    void btn_WF_relocate(int btn_id, int btn_state, int WF_type, int movement){
      if(btn_func.btn_action_count[btn_id]==btn_state){

        temp_WF[0]=WF_type;

        if(movement==1){
          temp_WF[1]++;
        }
        else if(movement==-1){
          temp_WF[1]--;
        }
        if(temp_WF[1]>=total_watch_face[WF_type]){
          temp_WF[1]=0;
        }
        if(temp_WF[1]<0){
          temp_WF[1]=total_watch_face[WF_type]-1;
        }

        // // Display item list in next frame
        // list_items_display_state=0;
      }

    }



    void btn_CF_relocate(int btn_id, int btn_state, int movement){
      if(btn_func.btn_action_count[btn_id]==btn_state){


        if(movement==1){
          C_SS.charging_face_conf[2]++;
        }
        else if(movement==-1){
          C_SS.charging_face_conf[2]--;
        }
        if(C_SS.charging_face_conf[2]>=C_SS.charging_face_conf[0]){
          C_SS.charging_face_conf[2]=0;
        }
        if(C_SS.charging_face_conf[2]<0){
          C_SS.charging_face_conf[2]=C_SS.charging_face_conf[0]-1;
        }

      }

    }

    void btn_item_relocate(int btn_id, int btn_state, int movement){
      if(btn_func.btn_action_count[btn_id]==btn_state){
        if(movement==1){
        app_item_list.current_page_layout[2]++;
        }
        else if(movement==-1){
          app_item_list.current_page_layout[2]--;
        }
        if(app_item_list.current_page_layout[2]>app_item_list.page_layouts[(app_item_list.current_page_layout[0]*2)+1]-app_item_list.current_page_layout[1]){
          app_item_list.current_page_layout[2]=0;
        }
        if(app_item_list.current_page_layout[2]<0){
          app_item_list.current_page_layout[2]=app_item_list.page_layouts[(app_item_list.current_page_layout[0]*2)+1]-app_item_list.current_page_layout[1];
        }

        // Display item list in next frame
        list_items_display_state=0;
      }

    }

    void run(){

      int btn_id, btn_state, page_num, item_id;
      // app_options.current_app_GID==0 | POWER MENU
      // app_options.current_app_GID==1 | MENU APPS
      // app_options.current_app_GID==2 | SYSTEM SUPPORT


      // BACK Conditions | Common for All items in particular Page

      if(page_common_back(4)==1){
        screen_switch(3,5,3);
      }
      else if(page_common_back(5)==1){
      // Back method | Common for all initial Menu Items
        screen_switch(3,5,3);
      }
      else if(page_common_back(1)==1){
        btn_page_hook(3, 5, 5, 0);
      }
      else if(page_common_back(3)==1){
        btn_page_hook(3, 5, 1, 1);
      }
      else if(page_common_back(7)==1){
        btn_page_hook(3, 5, 5, 1);
      }
      else if(page_common_back(6)==1){
        btn_page_hook(3, 5, 1, 3);
      }
      else if(page_common_back(2)==1){
        btn_page_hook(3, 5, 1, 0);
      }
      else if(page_common_back(8)==1){
        btn_page_hook(3, 5, 6, 1);
      }
      else if(page_common_back(9)==1){
        btn_page_hook(3, 5, 8, 0);
      }
      else if(page_common_back(10)==1){
        btn_page_hook(3, 5, 6, 0);
      }



      // Condition for Shutdown
      if(page_compare(4,0)==1){


        btn_app_exe_info_set(2, 5, 0, 1);
        // screen_switch(3,5,3);


      }
      // Run When "Restart is pressed"
      else if(page_compare(4,1)==1){
        
        btn_app_exe_info_set(2, 5, 1, 1);
        // screen_switch(3,5,3);

        // btn_item_relocate(2, 1, 1);
        // btn_item_relocate(3, 1, -1);
        
        // display_list_items_frame();

        // btn_page_reset(3, 5);
        

      }
      // Run When "Hibernate is pressed"
      else if(page_compare(4,2)==1){
        
        
        btn_app_exe_info_set(2, 5, 2, 1);
        // screen_switch(3,5,3);



        // btn_item_relocate(2, 1, 1);
        // btn_item_relocate(3, 1, -1);
        
        // display_list_items_frame();

        // btn_page_reset(3, 5);


      }
      


      // Settings items
      // Run When "Settings option is pressed"
      else if(page_compare(5,0)==1){
        btn_page_hook(2, 5, 1, 0);
      }



      // Run When "Settings:Watch Info option is pressed"
      
      else if(page_compare(1,0)==1){

        btn_page_hook(2, 5, 2, 0);

      }
      // Run When "Settings:Display option is pressed"
      else if(page_compare(1,1)==1){

        btn_page_hook(2, 5, 3, 0);
        // btn_page_hook(3, 5, 5, 0);

        // btn_item_relocate(2, 1, 1);
        // btn_item_relocate(3, 1, -1);
        
        // display_list_items_frame();

      }
      else if(page_compare(1,3)==1){

        btn_page_hook(2, 5, 6, 0);
        // btn_page_hook(3, 5, 5, 0);

        // btn_item_relocate(2, 1, 1);
        // btn_item_relocate(3, 1, -1);
        
        // display_list_items_frame();

      }
      



      
      else if(page_compare(2,0)==1){

        // btn_page_hook(0, 5, 4, 0);
        // btn_page_hook(2, 5, 4, 0);

        // btn_page_hook(3, 5, 1, 0);

        btn_app_exe_info_set(2, 5, 3, 1);

        // btn_item_relocate(2, 1, 1);
        // btn_item_relocate(3, 1, -1);
        
        // display_list_items_frame();
 
      }
      else if(page_compare(2,1)==1){
        btn_app_exe_info_set(2, 5, 4, 1);
      }
      else if(page_compare(2,2)==1){
        btn_app_exe_info_set(2, 5, 19, 1);
      }
      else if(page_compare(2,5)==1){
      
        btn_app_exe_info_set(2, 5, 15, 1);
        
      }
    


      
      else if(page_compare(3,0)==1){

        // btn_page_hook(3, 5, 1, 1);

        btn_app_exe_info_set(2, 5, 12, 1);

      }
      else if(page_compare(3,1)==1){

        btn_app_exe_info_set(2, 5, 13, 1);

      }
      else if(page_compare(3,2)==1){

        btn_app_exe_info_set(2, 5, 14, 1);

      }
      

        
      else if(page_compare(6,0)==1){

        btn_page_hook(2, 5, 10, 0);

        // btn_app_exe_info_set(2, 5, 3, 1);
           
      }
      else if(page_compare(6,1)==1){
        btn_page_hook(2, 5, 8, 0);
      }
      else if(page_compare(6,2)==1){
        btn_app_exe_info_set(2, 5, 8, 1);
      }
      else if(page_compare(6,4)==1){
        C_SS.charging_face_conf[2]=0;
        btn_app_exe_info_set(2, 5, 20, 0);

      }
      


      else if(page_compare(8,0)==1){
        // btn_app_exe_info_set(2, 5, 8, 1);
        btn_page_hook(2, 5, 9, 0);
        // btn_page_hook(3, 5, 6, 1);
      }



      else if(page_compare(9,0)==1){
        btn_app_exe_info_set(2, 5, 9, 1);
        
      }
      else if(page_compare(9,1)==1){
        btn_app_exe_info_set(2, 5, 10, 1);
        
      }
      else if(page_compare(9,2)==1){
        btn_app_exe_info_set(2, 5, 11, 1);
        
      }




      else if(page_compare(10,0)==1){
        btn_app_exe_info_set(2, 5, 17, 2);
        tmp_date_time[16]=0;
      }
      else if(page_compare(10,1)==1){
        btn_app_exe_info_set(2, 5, 16, 2);
        tmp_date_time[16]=8;
      }

      // Watch Face items
      // Run When "Watch Face option is pressed"


      else if(page_compare(5,1)==1){
        
        // screen_switch(3,5,3); // Back to HOME
        // btn_page_hook(0, 5, 4, 0);
        btn_page_hook(2, 5, 7, 0);
      }


      
      else if(page_compare(7,0)==1){

        // btn_page_hook(3, 5, 5, 4);
        
        btn_temp_WF_set(2, 5, 0, 0, current_WF[2]);
        btn_app_exe_info_set(2, 5, 5, 0);
           
      }
      else if(page_compare(7,1)==1){

        // btn_page_hook(3, 5, 5, 4);

        btn_temp_WF_set(2, 5, 1, 0, current_WF[2]);
        btn_app_exe_info_set(2, 5, 6, 0);
           
      }
      else if(page_compare(7,2)==1){

        // btn_page_hook(3, 5, 5, 4);

        // btn_app_exe_info_set(2, 5, 3, 1);
           
      }
      
      // Calender App

      else if(page_compare(5,2)==1){
        
        tmp_calender_date[0]=time_array[5];
        tmp_calender_date[1]=time_array[7];
        tmp_calender_date[2]=time_array[8];
        
        btn_app_exe_info_set(2, 5, 18, 2);
      }


      // Stopwatch App

      else if(page_compare(5,3)==1){
        // Currently working on it
      }

      // Alarm App

      else if(page_compare(5,4)==1){
        // Currently working on it
      }

      // Calculator App

      else if(page_compare(5,5)==1){
        // Will be released in future updates
        btn_app_exe_info_set(2, 5, 22, 1);
      }

      // Games App

      else if(page_compare(5,6)==1){
        // Will be released in future updates
        btn_app_exe_info_set(2, 5, 23, 1);
      }



      ///////////////////////////
      // Common on all
      if(running_final_application!=1){
        // Update 1 dtatic frame for any btn click
        btn_switch_list_display_state(0,5,0);
        btn_switch_list_display_state(2,5,0);
        btn_switch_list_display_state(3,5,0);


        btn_page_hook(0, 5, 4, 0);
        

        btn_item_relocate(2, 1, 1);
        btn_item_relocate(3, 1, -1);
        
        display_list_items_frame();
      }

      
    }
};

RUN_APP run_app;


///////////////////
// Screen Checks //
///////////////////

class SCREEN_Checks {

  public:

    // INDEX : Screen TYPE
    // 0 : Info Splash Screen
    // 1 : Battery Splash Screen
    // 2 : Watch Face Screen
    // 3 : Home Screen
    // 4 : 0: Power Apps Screen | 1: Menu Apps Screen
    // 5 : Apps Full Screen
    // 6 : Run App
    // 7 : Basic Screen Print | BG_Colour:B | T_Colour:W | Supplying no string will create a blank screen
    // Initial Values : {0,0,0,0,0,0,0}
    // Switching values to 1 will active the Screen Layers
    // Screen Checks are done from left to right 0-->6
    // Screen Display are done from right to left 6-->0
    // On Boot : it is checked if the device is charging
    //            if so then screen_id[1]=1
    //            and it will show battery splash screen
    //           if not then screen_id[2]=1
    //            and it will show Watch Face Screen
    // int screen_status[8] = {0,0,0,0,0,0,0};
    



    


    void active_screen(){
      
      if(c_screen_ID==1){
        display.clearDisplay();
        // bat_voltage = analogRead(0);
        splash_screen(0, C_SS.charging_face_conf[1], (analogRead(0)*100)/1024);
        display.display();



        run_app.btn_page_hook(0, 5, 4, 0);
        screen_switch(0, 5, 6);
        screen_switch(2, 1, 2);
      }

      else if(c_screen_ID==2){
        display.clearDisplay();
        watch_face(current_WF[0], current_WF[1], current_WF[2]);
        display.display();

        run_app.btn_page_hook(0, 5, 4, 0);
        run_app.btn_page_hook(2, 5, 7, 0);
        screen_switch(0, 5, 6);
        // On 2-Hold forward to Digital/Analog/Dual WF Options
        screen_switch(2, 5, 6);
        screen_switch(2, 1, 3);
        screen_switch(3, 1, 1);

        // Holding BTN-3 will show the Menu Page
        screen_switch(3, 5, 6);
        run_app.btn_page_hook(3, 5, 5, 0);

        if(run_app.list_items_display_state!=0){
          run_app.list_items_display_state=0;
        }
        

      }
      else if(c_screen_ID==3){

        display.clearDisplay();
        watch_face(0, 0, current_WF[2]);
        display.display();

        run_app.btn_page_hook(0, 5, 4, 0);
        run_app.btn_page_hook(2, 1, 5, 0);

        screen_switch(0, 5, 6);

        screen_switch(2, 1, 6);

        screen_switch(3, 1, 2);

        if(run_app.list_items_display_state!=0){
          run_app.list_items_display_state=0;
        }
          
        // screen_switch(3, 1, 2);
      }
      else if(c_screen_ID==4){
        // APP Execute
//        if(run_app.app_display_handling[1]==-1){
//          
//        }
        if(run_app.app_display_handling[1]==0){
          // For Dynamic App Settings
          display.clearDisplay();
          run_app.app_execute(run_app.app_display_handling[0]);
          display.display();
        }
        else if(run_app.app_display_handling[1]==1){
          // For Static App Settings
          display.clearDisplay();
          run_app.app_execute(run_app.app_display_handling[0]);
          display.display();
          run_app.app_display_handling[1]=-1;
        }
        else if(run_app.app_display_handling[1]==2 || run_app.app_display_handling[1]==-2){
          if(run_app.app_display_handling[1]==2){
            display.clearDisplay();
            run_app.app_execute(run_app.app_display_handling[0]);
            display.display();
            run_app.app_display_handling[1]=-2;
          }

          // Will run only if any button is pressed
          for(int i=0;i<4;i++){
            if(btn_func.btn_action_count[i]>=1){
              run_app.app_display_handling[1]=2;
              display.clearDisplay();
              run_app.app_execute(run_app.app_display_handling[0]);
              display.display();
            }
          }
          
        }


        run_app.btn_page_hook(0, 5, 4, 0);
        screen_switch(0, 5, 6);

        run_app.btn_switch2prev_screen_ID(0, 5);
        run_app.btn_switch2prev_screen_ID(3, 5);

      }

      else if(c_screen_ID==6){
        Serial.println(app_item_list.current_page_layout[0]);
        Serial.println(app_item_list.current_page_layout[1]);
        Serial.println(app_item_list.current_page_layout[2]);

        run_app.run();  

        
        
      }
    }
};



SCREEN_Checks screen_chks;
void setup() {

  EEPROM.begin(512);  //Initialize EEPROM
  

  // PIN MODE DECLERATION
  // INPUT_PULLUP : | DEFAULT PIN STATE 1 | SWITCH END SHOULD BE CONNECTED TO GND
  

  for(int i=0;i<4;i++){
    pinMode(btn_func.btn_gpio[i], INPUT_PULLUP);
  };

  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.setRotation(1); // rotate 90 degree

  

  
  if(crypto.dec2bin(EEPROM.read(0))>=10000000){
    EEPROM.write(0, 0x00);
    EEPROM.commit();
    Serial.println("No Boot Logo | Like Wakeup");
    
  }
  else{
//    delay(1000); // Pause for 1 seconds
    // Clear the buffer.
    display.clearDisplay();
    display.setTextColor(1);
    // Draw bitmap on the screen
    
    display.drawBitmap(0, 32, logo_bmp, 64, 64, 1);
    display.display();
    delay(2000);
  }
  


  // Initial Assign / function start
  // screen_chks.screen_status[2]=1; // Show Watch Face
  c_screen_ID=2; // Setting Current Screen to Watch Face

}



class POWER_SETTING{
  public:
    int sleep_mode;
    //0: Light wake
    //1: Modem Sleep
    //2: Light Sleep
    //3: Deep Sleep

    void sleep(int S_MODE){
      if(S_MODE==0){
        gpio_pin_wakeup_enable(btn_func.btn_gpio[0], GPIO_PIN_INTR_HILEVEL);
      }
      else if(S_MODE==1){
        // gpio_pin_wakeup_enable(btn_func.btn_gpio[0], GPIO_PIN_INTR_LOLEVEL);
        Serial.println("Initialized Modem Sleep");
      }
      else if(S_MODE==2){
        wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
        wifi_fpm_open();
        gpio_pin_wakeup_enable(btn_func.btn_gpio[0], GPIO_PIN_INTR_LOLEVEL);
        wifi_fpm_do_sleep(0xFFFFFFF);
      }
      else if(S_MODE==3){
        ESP.deepSleep(0);
      }
    }

};

POWER_SETTING power_settings;

// Temporary Time Calculation Function
void update_time_per_sec(){

  // Interrupt program every 1 sec [delay(1000)] to update second by 1

  if(time_array[2]==23 && time_array[1]==59 && time_array[0]==59){
    time_array[2]=0; // MAX hour value -> 23 | MIN minuite value -> 0
    time_array[1]=0; // MAX minuite value -> 59 | MIN minuite value -> 0
    time_array[0]=-1; // MAX seconds value -> 59 | MIN minuite value -> -1
    time_array[3]=0; // AM
    time_array[4]++; // Increase dd by 1
  }
  else if(time_array[2]==11 && time_array[1]==59 && time_array[0]==59){
    time_array[1]=0; // MAX minuite value -> 59 | MIN minuite value -> 0
    time_array[0]=-1; // MAX seconds value -> 59 | MIN minuite value -> -1
    time_array[3]=1; // PM
    time_array[2]++; // Increase hh by 1
  }
  else if(time_array[1]==59 && time_array[0]==59){
    time_array[1]=0; // MAX minuite value -> 59 | MIN minuite value -> 0
    time_array[0]=-1; // MAX sec value -> 59 | MIN sec value -> -1
    time_array[2]++; // Increase hh by 1
  }
  else if(time_array[0]==59){
    time_array[0]=-1; // MAX sec value -> 59 | MIN sec value -> -1
    time_array[1]++; // Increase mm by 1 
  }
  time_array[0]++; // Increase ss by 1
  
}



///////////////
// Status Bar//
///////////////
void status_bar(int x, int y, int w){
  display.fillRect(x, y, w, 12, 1);
}




//BTN_FUNC btn_func;

int bat_voltage;
//int movement = 0;


void loop(){
  btn_func.btn_state_chk();
  screen_chks.active_screen();
  update_time_per_sec();

  // After fetching data according to the Logics
  // the button action should be put in InActive state
  btn_func.btn_reset();
}


void loop0() {
   // display.clearDisplay();
// //  default_watch_face();
//   // watch_face_2();
// //  watch_face_3();
//     // watch_face_4();
//     //  Default Analog Watch Face Testing
//   // default_analog_watch_face();
//   // charging_splash_screen();
//   // charging_splash_screen_2();
// //   charging_splash_screen_3();
// //   charging_splash_screen_4();
//   // charging_splash_screen_5();
// //   charging_splash_screen_6();
//   // charging_splash_screen_7();


//   // // Icon Testing
//   // display.drawBitmap(0, 31, setting_icon_64x64, 64, 64, 1);

//   // // Vector Image formation testing
//   // setting_wheel_icon(0,15,63, 0);
//   // calculator_icon(0,15,63, 0);
//   // calender_icon(0,15,63, 0);
//   // alarm_clock_icon(0,15,63, 1);
//   // stop_watch_icon(0,15,63,1);

//   // Status Bar
//   // status_bar(0,0,63);

//   // App Listing

//   // int apps_to_list[] = {5,6,7};
//   // int app_list_active_status[] = {1,1,1};
//   // app_list(3, apps_to_list, app_list_active_status, 0, 0, 63, 127);

//   // App Options Listing Background
//   // app_option_list_background(0,14,63, 114);

//   // App Option List
//   // app_option_list(option_array ,0,14,114);

//   // Power Options
//   // shut_down_icon(0, 14, 63, 1);
//   // restart_icon(0, 14, 63, 1);
//   // hibernate_icon(0, 14, 63, 1);

//   // display.display();

//   // Temporary Clock Simulation
//   // delay(1000);
//   // update_time_per_sec();

//   // Graphics and Screen Switch testing
// //   screen_switch();
// PIN MODE DECLERATION

   // Check for all 4 BUTTON STATE and ACTION Count
    for(int i=0;i<4;i++){
      btn_func.multi_func(i);
      // Serial.print(btn_func.btn_action_count[i]);
    };
    

//  delay(2000);
//  if(btn_2_s==1){
//    // app_options.current_app_id++;
//    list_app_contents(0, 1); // Listing SETTINGS OPTIONS
//
////    Serial.println(pagerState);
//  }
//  else if(btn_3_s==1){
//    // app_options.current_app_id--;
//    list_app_contents(0, -1); // Listing SETTINGS OPTIONS
//  }
//  if(btn_1_s==1){
//    app_options.show_FS_app_icon('M', app_options.current_app_id);
//    app_item_list.current_item_id=0;
//  }
//  if(btn_1_s==0){
//    app_options.list_apps('M', app_options.current_app_id);
//    
//  }

  // In Watch Face Screen
  if(btn_func.btn_action_count[0]==5){
    app_item_list.create_page_layout(0, 4, 0);
    c_screen_ID=6;

  }

  else if(c_screen_ID==2){
    if(btn_func.btn_action_count[0]==5){
      app_options.current_app_id=1;
      c_screen_ID=4; // Show Power Apps
      
    }
    else if(btn_func.btn_action_count[2]==1){
      app_options.current_app_id=1;
      c_screen_ID=3; // Show HOME
    }
  }
  // In HOME SCREEN
  
  else if(c_screen_ID==3){


//     if(btn_func.btn_action_count[0]==5){
      
//       app_options.current_app_id=1;
//       app_options.current_app_GID=0; // 0: POWER App List

//       c_screen_ID=4; // Show App List
      
      
//     }
//     else if(btn_func.btn_action_count[2]==1){
      
//       app_item_list.create_page_layout(0, 5, 0);
//       for(int i=0;i<3;i++){
//         app_item_list.previous_page_layout[i]=0;
//       }
// //      app_item_list.create_page_layout(1, 5, 0);
//       // app_item_list.current_page_layout[0] = 0;
//       // app_item_list.current_page_layout[1] = app_item_list.page_layouts[0][2];
//       // app_item_list.current_page_layout[2] = app_item_list.page_layouts[0][3];
//       // app_item_list.current_page_layout[3] = 0;
//       run_app.running_final_application=0;
//       c_screen_ID=6;

//       // Cancel button action
//       btn_func.btn_action_count[2]=0;
//      
//    }
//    else if(btn_func.btn_action_count[3]==5){
//      c_screen_ID=2; // Show Watch Face
//    }
  }
  // In App List Screen
  // App Item Listing
  else if(c_screen_ID==6){
    // App Icon Full Screen
    /// On Button Click Function
    // if(btn_func.btn_action_count[3]==5 &&
    //   (app_item_list.current_page_layout[0]==4 ||
    //     app_item_list.current_page_layout[0]==5)
    //     ){

    //     c_screen_ID=3;

        
    //   }
  }

  
  
  // Dynamic Screen Switch Testing
    screen_chks.active_screen();
  
  // partial_screen_switch(app_options.current_app_id);

  // // APP Content Testing
  // if(btn_2_s==0){
  //   movement = 1;
  // }
  // else if(btn_3_s==0){
  //   movement = -1;
  // }
  // if(btn_1_s==0){
  //   app_item_list.current_item_id=0;
  // }
  // list_app_contents(0, movement); // Listing SETTINGS OPTIONS
  // movement = 0;
   
   // bat_voltage = analogRead(0);
   // // Serial.println(bat_voltage);
   // splash_screen('C', 4, (bat_voltage*100)/1024);
   // watch_face('D', 1, 0);
   // watch_face('A', 0, 0);


  // display.display();
  update_time_per_sec();

  // After fetching data according to the Logics
  // the button action should be put in InActive state
  btn_func.btn_reset();
  
}
