#include <Wire.h> 
#include <LiquidCrystal.h>
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <EEPROM.h>
 
String data = "";
String l_data = "";
String barcode = "";
String f_t;
bool buttonClicked;
bool fussionON;
bool scanON;
bool coolingON;
int fussionIndex=0;
int coolingIndex=0;
 
const int OUTPUT_PIN=A0;
const int BUZZ_PIN=A1;
int first_digit ;
int second_digit = 0;
int third_digit = 0;
int cursor_position_y=16;
int cursor_position_x=1;
int cursor_position_y_var=16;
int cursor_position_x_var=1;
int digit_max = 9;

const int pushBT1 = 8; 
const int pushBT2 = 9;
const int pushBT3 = 10;
const int pushBT4 = 11;

int lower_limit = 10;
int upper_limit = 999;

enum OPERATION_MODE {
  NONE,
  SCAN,
  MANUAL,
  RUNNING,
  SELECTED
};
OPERATION_MODE mode = NONE;
OPERATION_MODE previous_mode = NONE;

void setup() 
{  
   
Serial.begin(115200);

  lcd.begin (20,4);
  pinMode(pushBT1, INPUT_PULLUP);
  pinMode(pushBT2, INPUT_PULLUP); 
  pinMode(pushBT3, INPUT_PULLUP);
  pinMode(pushBT4, INPUT_PULLUP);
  pinMode (OUTPUT_PIN, OUTPUT);
  pinMode (BUZZ_PIN, OUTPUT);
 
  digitalWrite(pushBT1, HIGH);
  digitalWrite(pushBT2, HIGH);
  digitalWrite(pushBT3 , HIGH);
  digitalWrite(pushBT4 , HIGH);
  
  lcd.clear(); lcd.setCursor(6,0); lcd.print("WELCOME"); 
  lcd.setCursor(8,1); lcd.print("TO"); 
  lcd.setCursor(2,2); lcd.print("ELECTRO FUSION");
  lcd.setCursor(0,3);   lcd.print("POLY WELDING MACHINE"); 
  delay(3500);
   lcd.clear();
    lcd.setCursor(1,2);   lcd.print("S.I. Engineering"); 
      delay(2500);
 
 
    resetAll();
 }

 void relay()    
{ 
  digitalWrite (OUTPUT_PIN, LOW);
  lcd.clear(); lcd.setCursor(1,1); lcd.print("Fusion Stopped"); 
  delay(2000);
        resetAll();
  }
 
void resetAll()
{
    mode=NONE;
    previous_mode=NONE;
    data="";
    l_data="";
    cursor_position_y_var=cursor_position_y;
    first_digit ;
    second_digit ;
    third_digit ;
    fussionON=false;
    scanON=false;
    coolingON=false;
    fussionIndex=0;
    coolingIndex=0;
    digitalWrite (OUTPUT_PIN, LOW);
    lcd.clear();
    lcd.noBlink();

/////////////////// For Manual ///////////
mode=MANUAL;
    previous_mode=SELECTED;
 
    lcd.clear();
    lcd.setCursor (0, 1);
    lcd.print("FUSION TIME  :-  ");
     
    first_digit = EEPROM.read(1);
    second_digit = EEPROM.read(2);
    third_digit = EEPROM.read(3);
    
    lcd.setCursor (cursor_position_y, cursor_position_x);
    lcd.blink();
    lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
    lcd.print(first_digit);
    lcd.setCursor (cursor_position_y_var+1, cursor_position_x_var);
    lcd.print(second_digit);
    lcd.setCursor (cursor_position_y_var+2, cursor_position_x_var);
    lcd.print(third_digit);
    lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
    delay(1000);
   
     
}

void reStart()
{
  int checkvalue=l_data.toInt();
  if(fussionIndex<=0 || (checkvalue-fussionIndex) < (checkvalue/4))
  {
   fussionON=false;
   coolingON=false;
   fussionIndex=0;
   coolingIndex=0;
   displayfusiontime(l_data);
  }
  else
  { 
       resetAll(); 
  }
}

void loop() {

if(fussionON==true && mode== RUNNING && fussionIndex>=0)
{
   Serial.println("fusiontimeloop");
  digitalWrite(OUTPUT_PIN,HIGH);
  fusiontimeloop();
}
if(fussionON==false && coolingON==true && mode== RUNNING)
{
  Serial.println("coolingTimeLoop");
  digitalWrite(OUTPUT_PIN,LOW);
  coolingTimeLoop();  
                       
                          if(coolingIndex<=0)
                          {
                            coolingON=false;
                            delay (1000);
                            lcd.clear();
                            fussionIndex = 0;
                            coolingIndex = 0;
                            BUZZ();
                          }
  }
  if (digitalRead (pushBT2) == LOW && (mode == RUNNING)){
     Serial.println("resetAll");
      relay();   
  }
 
  if (digitalRead (pushBT1) == LOW && (mode == RUNNING)){
     Serial.println("reStart");
    reStart();
  }
 
  if (digitalRead (pushBT4) == LOW && (mode == MANUAL))
  {
    
    if(cursor_position_y_var==cursor_position_y)
    { 
      Serial.println("IF 1");
      Serial.println("cursor_position_y_var");
    Serial.println(cursor_position_y_var); 
    Serial.println("cursor_position_y");
    Serial.println(cursor_position_y);
    
 first_digit = EEPROM.read(1);
      first_digit = first_digit + 1;
      if(first_digit > digit_max)
      {
         first_digit=0;    
      }
      
      EEPROM.update(1,first_digit );   
        Serial.println(first_digit );

        int val1 = EEPROM.read(1); 
        Serial.print("EEPROM"); 
         Serial.println(val1);
   
      

      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      lcd.print(first_digit);
      lcd.setCursor (cursor_position_y_var+1, cursor_position_x_var);
      lcd.print(second_digit);
      lcd.setCursor (cursor_position_y_var+2, cursor_position_x_var);
      lcd.print(third_digit);
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      delay(1000);

    }
    else if(cursor_position_y_var==(cursor_position_y+1))
    { 
      Serial.println("IF 2");
      Serial.println("cursor_position_y_var");
    Serial.println(cursor_position_y_var); 
    Serial.println("cursor_position_y");
    Serial.println(cursor_position_y);
    Serial.println("cursor_position_x_var");
    Serial.println(cursor_position_x_var);


     second_digit = EEPROM.read(2);
      second_digit = second_digit + 1;
      if(second_digit > digit_max)
       
      {
        second_digit=0; 
      }
       EEPROM.update(2,second_digit);   
        Serial.println(second_digit);

        int val2 = EEPROM.read(2); 
        Serial.print("EEPROM"); 
         Serial.println(val2);


    
      lcd.setCursor (cursor_position_y_var-1, cursor_position_x_var);
      lcd.print(first_digit);
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      lcd.print(second_digit);
      lcd.setCursor (cursor_position_y_var+1, cursor_position_x_var);
      lcd.print(third_digit);
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      
      delay(1000);
   
    }
    else if(cursor_position_y_var==(cursor_position_y+2))
    { 
      Serial.println("IF 3");
      Serial.println("cursor_position_y_var");
    Serial.println(cursor_position_y_var); 
    Serial.println("cursor_position_y");
    Serial.println(cursor_position_y);
     
      
     third_digit = EEPROM.read(3);
      third_digit = third_digit + 1;
      if(third_digit > digit_max)
      
      {
        third_digit=0;
      } 
      
      EEPROM.update(3,third_digit );   
        Serial.println(third_digit );

        int val3 = EEPROM.read(3); 
        Serial.print("EEPROM"); 
         Serial.println(val3);  

    
      lcd.setCursor (cursor_position_y_var-2, cursor_position_x_var);
      lcd.print(first_digit);
      lcd.setCursor (cursor_position_y_var-1, cursor_position_x_var);
      lcd.print(second_digit);
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      lcd.print(third_digit);
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      delay(1000);
      
    }

  }

  if (digitalRead (pushBT3) == LOW && (mode == MANUAL))
  {
    Serial.println("IF 4");
    if(cursor_position_y_var<(cursor_position_y+3) && cursor_position_y_var>(cursor_position_y-1))
    {
      cursor_position_y_var=cursor_position_y_var+1;
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      delay(1000);
    }
  }
  if (digitalRead (pushBT2
  ) == LOW && (mode == MANUAL))
  {  EEPROM.update(1, first_digit);
     EEPROM.update(2,second_digit);
     EEPROM.update(3,third_digit );
     Serial.println("IF 5");
     cursor_position_y_var=cursor_position_y;
     lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
     lcd.blink();
     delay(1000);
     
     first_digit   ;
     second_digit  ;
     third_digit   ;
  }
 
   if (digitalRead (pushBT1) == LOW  && (mode == MANUAL)){
      Serial.println("IF 6");
      int finalvalue =first_digit+second_digit+third_digit;
     if(finalvalue>0)
     {  previous_mode=NONE;
        mode = RUNNING;
        data= String(first_digit)+String(second_digit)+String(third_digit);
        if(checkValidFussionValue(data))
       {
          first_digit ;
          second_digit ;
          third_digit ;
    
          displayfusiontime(data);
       }
   
        
       }  
     }
  }
 
 
void displayfTime(String data)
{
   String dvalue="";
                        if(data!="")
                        {
                                int i= data.toInt();
                                if(i<10)
                                {
                                  dvalue="00"+String(i);
                                }
                                else if(i>=10 && i<100)
                                {
                                  dvalue="0"+String(i);
                                }
                                else{
                                    dvalue=String(i);
                                  }
                                lcd.setCursor (16, 0);
                                lcd.print (dvalue);
                                int j= data.toInt()*2;
                                String dvalue="";
                                
                                if(j<10)
                                {
                                  dvalue="00"+String(j);
                                }
                                else if(j>=10 && j<100)
                                {
                                  dvalue="0"+String(j);
                                }
                                else{
                                    dvalue=String(j);
                                  }
                                lcd.setCursor (16, 1);
                                lcd.print (dvalue);               
                        
                                lcd.setCursor (0,3);
                                if(fussionIndex == 0)
                                {
                                     lcd.print("PRESS OK to RESTART"); 
                                }
                                else{
                                      lcd.print("PRESS R to RESET   ");
                                  }  
                                lcd.setCursor (0, 1);
                                lcd.print("COOLING TIME :- "); 
                                lcd.setCursor (0, 0);
                                lcd.print("FUSION TIME  :- "); 
                                delay (1000); 
                      }             
}
bool checkValidFussionValue(String data)
{
       int checkValue=data.toInt(); 
       if(checkValue>=lower_limit && checkValue<=upper_limit)
       {
          return true;
       }
  return false;     
}

 void BUZZ() {   //........................................................................

   if (coolingIndex==0)
{ 
  digitalWrite (BUZZ_PIN, HIGH);
  lcd.clear(); lcd.setCursor(1,1); lcd.print("Fusion Complete"); 
  delay(2000);
  displayfTime(l_data);
        digitalWrite (BUZZ_PIN, LOW);
  }
}

void fusiontimeloop()
{                               fussionIndex--;
                                fussionON=true;
                                String dvalue="";
                                
                                if(fussionIndex<10)
                                {
                                  dvalue="00"+String(fussionIndex);
                                }
                                else if(fussionIndex>=10 && fussionIndex<100)
                                {
                                  dvalue="0"+String(fussionIndex);
                                }
                                else{
                                    dvalue=String(fussionIndex);
                                  }
                                lcd.setCursor (16, 0);
                                lcd.print (dvalue);
                                delay (1000);
                                coolingTime(); 
 }
void displayfusiontime(String data)
{
  lcd.noBlink();
 
                  if(data!="")
                  {
                          l_data=data;
                          buttonClicked=false;
                          fussionIndex=data.toInt();
                          fussionON=true;
                          displayfTime(data);
                  }

}
void coolingTimeLoop()
{                               coolingIndex--;
                                String dvalue="";
                                if(coolingIndex<10)
                                {
                                  dvalue="00"+String(coolingIndex);
                                }
                                else if(coolingIndex>=10 && coolingIndex<100)
                                {
                                  dvalue="0"+String(coolingIndex);
                                }
                                else{
                                    dvalue=String(coolingIndex);
                                  }
                                lcd.setCursor (16, 1);
                                lcd.print (dvalue);
                                delay (1000);
}
void coolingTime()
{
                          if(fussionIndex<=0)
                          {
                            fussionON=false;
                            coolingON=true;
                            coolingIndex= l_data.toInt()*2;
                           } 
}     
