#include <LiquidCrystal.h>
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#include <hiduniversal.h>
#include <Usb.h>
#include <usbhub.h>
#include <hidboot.h>
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
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

class KbdRptParser : public KeyboardReportParser {
    void PrintKey(uint8_t mod, uint8_t key);
  protected:
    virtual void OnKeyDown  (uint8_t mod, uint8_t key);
    virtual void OnKeyPressed(uint8_t key);
};

KbdRptParser Prs;

const int OUTPUT_PIN=A1;
int first_digit = 0;
int second_digit = 0;
int third_digit = 0;
int cursor_position_y=15;
int cursor_position_x=1;
int cursor_position_y_var=15;
int cursor_position_x_var=1;
int digit_max = 9;
const int pushBT = 12; 
const int pushBT1 = 11;
 
int lower_limit = 10;
int upper_limit = 900;

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
  lcd.begin (20,4);
  pinMode(pushBT, INPUT);
  pinMode(pushBT1, INPUT); 
  pinMode(pushBT2, INPUT);
  pinMode (OUTPUT_PIN, OUTPUT);
  pinMode (8, INPUT);
  digitalWrite(pushBT, HIGH);
  digitalWrite(pushBT1, HIGH);
  digitalWrite(pushBT2, HIGH);
   pinMode (8, INPUT);
  lcd.clear(); lcd.setCursor(6,0); lcd.print("WELCOME"); 
  lcd.setCursor(8,1); lcd.print("TO"); 
  lcd.setCursor(2,2); lcd.print("ELECTRO FUSSION");
  lcd.setCursor(0,3);   lcd.print("POLY WELDING MACHINE"); 
  delay(3500);
  resetAll();
  Serial.begin(115200);
  Usb.Init();
  delay(200);
  Hid.SetReportParser(0, (HIDReportParser*)&Prs);
  delay(200);
  
 }
 
void resetAll()
{
    mode=NONE;
    previous_mode=NONE;
    data="";
    l_data="";
    cursor_position_y_var=cursor_position_y;
    first_digit=0;
    second_digit=0;
    third_digit=0;
    fussionON=false;
    scanON=false;
    coolingON=false;
    fussionIndex=0;
    coolingIndex=0;
    digitalWrite (OUTPUT_PIN, LOW);
    lcd.clear();
    lcd.noBlink();
    lcd.setCursor (0, 1);
    lcd.print("SELECT MODE :-");
    lcd.setCursor (0, 2);
    lcd.print("PRESS 8 FOR SCAN");
    lcd.setCursor (0, 3);
    lcd.print("PRESS 10 FOR MANUAL");
 
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
  digitalWrite(OUTPUT_PIN,HIGH);
  fusiontimeloop();
}
if(fussionON==false && coolingON==true && mode== RUNNING)
{
  digitalWrite(OUTPUT_PIN,LOW);
  coolingTimeLoop();  
                       
                          if(coolingIndex<=0)
                          {
                            coolingON=false;
                            delay (1000);
                            lcd.clear();
                            fussionIndex = 0;
                            coolingIndex = 0;
                            displayfTime(l_data);
                          }
  
  }
  if (digitalRead (12) == LOW && (mode == RUNNING || previous_mode == SELECTED)){
     
      resetAll();
  }

  if (digitalRead (11) == LOW && (mode == RUNNING)){
    
    reStart();
  }
  

  if (digitalRead (8) == LOW && (mode == NONE)){
    mode=SCAN;
    previous_mode=SELECTED;
    scanON=true;
    lcd.clear();
    lcd.setCursor (0, 1);
    lcd.print("MODE IS SCAN");
    delay(1000);
    lcd.clear();
    lcd.setCursor (0, 1);
    lcd.print("PLEASE SCAN BAR CODE");
    lcd.setCursor (0, 2);
    lcd.print("PRESS 12 FOR RESET");
    
  }

  if(scanON==true)
  {
    Usb.Task();
   
  }

  if (digitalRead (10) == LOW && (mode == NONE)){
    
    mode=MANUAL;
    previous_mode=SELECTED;
    lcd.clear();
    lcd.setCursor (0, 1);
    lcd.print("MODE IS MANUAL");
    delay(1000);
    lcd.clear();
    lcd.setCursor (0, 1);
    lcd.print("FUSSION TIME :-  ");
    lcd.setCursor (0, 2);
    lcd.print("PRESS 12 FOR RESET"); 
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

  if (digitalRead (8) == LOW && (mode == MANUAL))
  {
    
     
    if(cursor_position_y_var==cursor_position_y)
    { 
      first_digit = first_digit + 1;
      if(first_digit > digit_max)
      {
        first_digit=0;    
      }
    
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
      second_digit = second_digit + 1;
      if(second_digit > digit_max)
      {
        second_digit=0;    
      }
    
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
      third_digit = third_digit + 1;
      if(third_digit > digit_max)
      {
        third_digit=0;    
      }
    
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

  if (digitalRead (10) == LOW && (mode == MANUAL))
  {
    
    if(cursor_position_y_var<(cursor_position_y+3) && cursor_position_y_var>(cursor_position_y-1))
    {
      cursor_position_y_var=cursor_position_y_var+1;
      lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
      delay(1000);
    }
  }
  if (digitalRead (11) == LOW && (mode == MANUAL))
  {
     cursor_position_y_var=cursor_position_y;
     lcd.setCursor (cursor_position_y_var, cursor_position_x_var);
     lcd.blink();
     delay(1000);
     first_digit=0;
     second_digit=0;
     third_digit=0;
  }
 
   if (digitalRead (12) == LOW  && (mode == MANUAL)){

      int finalvalue =first_digit+second_digit+third_digit;
     if(finalvalue>0)
     {  previous_mode=NONE;
        mode = RUNNING;
        data= String(first_digit)+String(second_digit)+String(third_digit);
        if(checkValidFussionValue(data))
       {
          first_digit=0;
          second_digit=0;
          third_digit=0;
         
          displayfusiontime(data);
       }
       else
       {
          resetAll();
       }
        
     }
  }

}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnKeyPressed(uint8_t key) {
  char c = (char)key;

  if (key == 19) {
    String ret = data;
    data = "";
    barcodenewCallback(ret);
  } else {
    data += c;
  }
}

int string_size_2(const char *c)
{

  size_t Size = strlen(c);
  return Size;
}
void barcodenewCallback(String data) {

  int len = string_size_2(data.c_str());

      int i=0;
      barcode="";
     for (int j = len-1; j >= 0; j--)
      {

        if(i>2 && i< 6)
        {
          barcode += data[j];
        }
         i++;
      }

               len = string_size_2(barcode.c_str());
               data="";
              for (int j = len-1; j >= 0; j--)
              {

               
                if( ((int)(barcode[j]))==32)
                {
                  data="";
                  break;
                }
                else
                {
                     data += barcode[j];
                }
          
                
          
              }

                
                if(mode == SCAN && checkValidFussionValue(data)){
                 scanON=false;
                  previous_mode=NONE;
                  mode=RUNNING;
                  displayfusiontime(data);
                }
                else
                  {
                      resetAll();
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
                                lcd.setCursor (17, 1);
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
                                lcd.setCursor (17, 2);
                                lcd.print (dvalue);               
                          
                  
                                lcd.setCursor (0,3);
                                if(fussionIndex == 0)
                                {
                                     lcd.print("PRESS 11 RESTART"); 
                                }
                                else{
                                      lcd.print("PRESS 12 RESET  ");
                                  }  
                                lcd.setCursor (0, 2);
                                lcd.print("COOLING TIME :-  "); 
                                lcd.setCursor (0, 1);
                                lcd.print("FUSSION TIME :-  "); 
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
                                lcd.setCursor (17, 1);
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
                                lcd.setCursor (17, 2);
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
