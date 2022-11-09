// time.cpp
#ifndef TIME_H
#define TIME_H

#include <math.h>

class TIME{
  public:

    int convert_12_2_24hr(int hh, int meridiem){
      // return : 24_HR_hh

      
      if(meridiem==0 && hh==12){
        return 0;
      }
      else if(meridiem==1 && hh>12){
        return 12+hh;
      }
      
      return hh;
    }

    int convert_24_2_12hr(int hh){
      // return (hh*10)+meridiem
      // value of meridiem => 0: AM
      //                      1: PM

      // Retrive hh & meridiem
      // hh_meridiem=(hh*10)+meridiem
      // hh = ((int)(hh_meridiem/10))*10
      // if hh_meridiem > hh
      // then meridiem == 1

      if(hh==0){
        return 120; // (12*10)+0
      }
      else if(hh>12){
        return ((hh-12)*10)+1;
      }
      
      return (hh*10);

    }


    int chk_for_prime_days(int mm){
      // mm == 1,3,5,7,8,10,12 | return 1 | 31 days
      // mm == 4,6,9,11 | return 0 | 30 days
      // Specifically for months having 30 && 31 days

      if(mm==4 || mm==6 || mm==9 || mm==11){
        return 0;
      }

      return 1;
    }

    int get_total_days_of_month(int mm, int yyyy){
      // yyyy => year | used to identify leap year

      if(mm==2){
        return 28+chk_leap_year(yyyy); // For February | return 28/29
      }
      
      return 30+chk_for_prime_days(mm); // return 30/31
    }

    int get_week_day(int dd, int mm, int yyyy){

      // Calender Type : Georgerian Calender
      // Reference : https://cs.uwaterloo.ca/~alopez-o/math-faq/node73.html
      ///
      // m: month
      // c: centuary
      // y: year endian
      // k: day
      // w: day of the Week
      ////
      /// Month Number conversion
      // 1: March : 31
      // 2: April : 30
      // 3: May : 31
      // 4: June : 30
      // 5: July : 31
      // 6: August : 31
      // 7: September : 30
      // 8: October : 31
      // 9: November : 30
      // 10: December : 31
      // 11: January : 31 // Preceding Year
      // 12: February : 28/29(LeapYear) // Preceding year

      if(mm==1 || mm==2){
        mm+=10;
        yyyy--;
      }
      else{
        mm-=2;
      }

      int c=yyyy/100; // 2022 -> 20
      int y=yyyy-(c*100); // 2022 -> 22
      
      int w = (int)(dd+floor((2.6*mm)-0.2)-2*c+y+floor(y/4)+floor(c/4))%7;


      if(w<0){
        w+=7;
      }

      // Starting Day ==> Sunday
      // 0: Sunday
      // 1: Monday
      // 2: Tuesday
      // 3: Wednesday
      // 4: Thursday
      // 5: Friday
      // 6: Saturday
      
       // Serial.println(c);
       // Serial.println(c*100);
       // Serial.println(yyyy);
       // Serial.println(y);
       // Serial.println(mm);
       // Serial.println(dd);
       // Serial.print("\nWeek day Num ");
       // Serial.println(w);
      

      return w;


    }



    int chk_leap_year(int year){
      // return : 1: Leap Year
      //          0: Not a Leap Year

      if((year%400==0 && year%100==0) || (year%4==0 && year%100!=0)){
        return 1; // year == leap_year
      }

      return 0; // year != leap_year
    }

    int verify_clock_format(int hh, int mm, int ss, int is_12hr){
      // return : 0: Incorrect clock format
      //          1: Correct clock format


      if(is_12hr==0 && (hh>23 || hh<0 || mm>59 || mm<0 || ss>59 || ss<0)){
        return 0;
      }
      else if(is_12hr==1 && (hh>12 || hh<1 || mm>59 || mm<0 || ss>59 || ss<0)){
        return 0;
      }
      return 1;
    }


    int verify_date_format(int dd, int mm, int yyyy){
      // return : 0: Incorrect date format
      //          1: Correct date Format

      // First check for leap year

      
      if(
          (yyyy<0) ||
          (mm>12 || mm<1) ||
          (dd>get_total_days_of_month(mm, yyyy) || dd<1)
        ){
        return 0;
      }

      return 1;
      
    }

};

#endif