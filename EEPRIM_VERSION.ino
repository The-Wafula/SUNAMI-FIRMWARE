//coder
#include <SoftwareSerial.h>
#include <LiquidCrystal.h> 
#include <EEPROM.h>
LiquidCrystal lcd(19,18,17,16,15,14); SoftwareSerial mySerial(2,3); 

///variables///
int address = 30;
int valueget;
int hourslength = 31;
int addresshours = 0;
int addresscounter = 20;
byte value; 
byte hourslengthvalue;  
byte hoursvalue; 
byte countervalue;

//////////////millis  parameters for reset //////////////////
unsigned long interval=72; //2 hours interval before count and decrement
unsigned long previousMillis=0;

//////////////millis  parameters for countdown //////////////////
//1hour =3600000ms~  3.6  as a a mil factor
//unsigned long intervalcount=3.6; // 1 hours interval before periodic reboot
unsigned long intervalcount=5; // 1 minute interval before periodic reboot
unsigned long previousMilliscount=0;


bool ledState = false;
char incomingByte;String inputString;String toGG;String airtt;String extract;String validate; String statuss;  String hours; String counter; String phone;String resp;String id;
int buzzer = 11; int housepower=9;int cloudswitch=10; 

/////ep get parameters////////
char hourbytes;String hourString; int hoursvar;

void setup()
   {
   const int STARTING_EEPROM_ADDRESS = 17;
   pinMode(cloudswitch, OUTPUT);   pinMode(housepower , INPUT);
   pinMode(buzzer, OUTPUT);   pinMode(13, OUTPUT);
   digitalWrite(4, ledState);    lcd.begin(16, 2);
   Serial.begin(9600); mySerial.begin(9600); delay(200);
 
   lcd.setCursor(0, 0); lcd.clear();lcd.print(F("Booting..."));
   delay(200);conect();   Serial.println(F("Connected.."));  
   
   mySerial.println("AT+CMGF=1");  delay(1000);  
   mySerial.println("AT+CNMI=1,2,0,0,0"); delay(1000);
   mySerial.println("AT+CMGL=\"REC UNREAD\""); lcd.setCursor(0, 0); lcd.clear();
   lcd.print(F("Sunami Solar"));
    delay(2000);
     mySerial.write("AT+CPBR=1\r\n");
 delay(2000);
  readsim1();  
  id=inputString.substring((inputString.indexOf("+CPBR: ")+10),(inputString.indexOf("ID")-7));
     Serial.println("ID: " + id);
   delay(1000);
   inputString = "";
   cleanBuffer(); checkmem(); beep(); beep(); beep();
   Serial.println(F("booting complete"));  
      Serial.println("___________________________________________");  
       }

void loop()
{  
 unsigned long currentMillis = millis(); 
 delay(500);  
 //daysvalue = EEPROM.read(addressdays);
  //Serial.print("Days stored2: " ); 
 // Serial.println(daysvalue); 
 if(mySerial.available()>0){
 while(mySerial.available()){          
 incomingByte = mySerial.read(); inputString += incomingByte;
  }
 Serial.flush(); delay(10); inputString.toUpperCase();Serial.print(inputString); Serial.println("|");
  
    
     validate=inputString.substring((inputString.indexOf("#")),(inputString.indexOf("&")+1));
  Serial.println("validate: " + validate); 
  //if (validate.endsWith("&")) {
      if (validate.startsWith("#")&& validate.endsWith("&")) {
  //  Serial.println("recognized command");
        smsretrieveal();
executestatus(); 
//////////////update hours to eeprom/////////////////////
hourseppost();
  }
  else
  {
    Serial.println("unrecognizes command");

  }


///getting current counter figure///////////
hoursepget();
////decrement figure by one
 Serial.print("Retrieved hours: ");
  Serial.println(hoursvar);
if (hoursvar>0)
{
   Serial.println("deccrementing  hours stored  by an hour");
  // hoursvar--;
     Serial.print("decremented hours value: ");
       Serial.println(hoursvar);
//////////update eeprom with hors var///////////////////////
///get length |update  eeprom//////////////
}
else
{
  Serial.println("no hours left to decrement from");
}

    Serial.println("-------------------------------------------");
 
 //Serial.print("Days stored2: " ); 
 // Serial.println(daysvalue); 
 if (inputString.indexOf("OK") == -1)
 { delay(1000);} inputString = "";
  Serial.flush();  cleanBuffer(); delay(1000);
  }

  // Serial.print("count value:");
      // Serial.println((unsigned long)(currentMillis - previousMilliscount));
       if (((unsigned long)(currentMillis - previousMilliscount))>= 3600000)//1 minute
      //  if (((unsigned long)(currentMillis - previousMilliscount)/60000)>= 2)//2 minutes
       {
         Serial.println("passed 1 min");
         Serial.println("countdown decrement initiated:");
         hoursepget();
          if (hoursvar==0){
            Serial.println("No days left .please pay");
            OFF();
         }
         else
         {
          ON();
            Serial.println("some days left, decrement hours");
             hoursvar--;
         Serial.print("decremented hours value:");
       
         Serial.println(hoursvar);
        
         String myStr;
         myStr = String(hoursvar);
             Serial.print("Mystr value:");
         Serial.println( myStr);
       decrementedeppost(myStr);
         report("0702826107",id);
         delay (1000);
         Serial.println("----------------------------------------------------");
         beep();  beep(); beep(); beep(); beep(); beep();
         }
        
           previousMilliscount = millis();
       }
       else
       {
       //  Serial.println("less than a min");
       }
       
/////////////////////timed intervalreboot code/////////////////////
  if (((unsigned long)(currentMillis - previousMillis)/100000) >= interval) {
     ledState = !ledState; 
    beep(); reseet(); previousMillis = millis();
 }
////////////////////////////

 /////////////////////1 hour decremental countdown code code/////////////////////

 }
     

void reseet()
 {
  mySerial.println(" AT+CFUN=4\r\n"); delay(1000);
 readsim(); delay(30000); Serial.print(F("rebooting...")); asm volatile ("jmp 0"); }
 
  void decrementedeppost(String hoursvar)
 {

 int hourslengthvalue=hoursvar.length();
EEPROM.update(hourslength,hourslengthvalue);

Serial.print("length of just saved hoursvar  string as read from eeprom: "); 
   Serial.println(EEPROM.read(hourslength)); 
     Serial.println("update hoursvarvalue:"); 
     
int j=0;
  for(int i=0;i<hourslengthvalue; i++,j++)
  {
    EEPROM.update(i,hoursvar.charAt(j));
  }
   Serial.println("Done posting"); 
 }
 
 void hourseppost()
 {
  int hourslengthvalue=hours.length();
EEPROM.update(hourslength,hours.length());

Serial.print("length of hours string as read from eeprom: "); 
   Serial.println(EEPROM.read(hourslength)); 
     Serial.println("Reading hours value:"); 
     
int j=0;
  for(int i=0;i<hours.length(); i++,j++)
  {
    EEPROM.update(i,hours.charAt(j));
  }
 }

  void hoursepget()
 {
  int hourslengthvalue=EEPROM.read(hourslength);
   Serial.print("hourslengthvalue:");  
    Serial.println(hourslengthvalue);  
  //    Serial.println("Read from hours slot:");  
      hourString ="";
  for(int a=0;a<hourslengthvalue;a++)
  {
    valueget = EEPROM.read(a);
    Serial.print(a);
    Serial.print("\t");
    Serial.println(char(valueget));
    hourbytes = char(valueget);
   hourString += hourbytes;
  }
      Serial.print("derived number of hours : ");
     Serial.println(hourString);
     hoursvar=hourString.toInt();
 }

void smsretrieveal()
{

 statuss=inputString.substring((inputString.indexOf("#")+1),(inputString.indexOf("$")));
  Serial.println("Command: " + statuss);  
 // int command=statuss.toInt;
  hours=inputString.substring((inputString.indexOf("$")+1),(inputString.indexOf("%")));
  Serial.println("Days: " + hours); 
    counter=inputString.substring((inputString.indexOf("%")+1),(inputString.indexOf("&")));
  Serial.println("count : " + counter); 
}
void executestatus(){
  if (statuss.toInt()>3||statuss.length() > 1)
  {
    
  }
  else{
     EEPROM.update(address,statuss.toInt());
  }

 //  statuss== statuss.toInt();
 
   if (statuss=="0")
   {
      Serial.println("Off command received"); 
      OFF();
   }
     if (statuss=="1")
   {
      Serial.println("On command received"); 
      ON();
   }
        if (statuss=="2")
   {
      Serial.print("feedback command received"); 
   }
        if (statuss=="3")
   {
      Serial.println("reset command received"); 
      reseet();
   }
   int value=(EEPROM.read(address));
   Serial.print("eeprom value: "); 
    Serial.println(value); 
  }
  
void toggle()
{digitalWrite(cloudswitch, HIGH);  delay(500);  digitalWrite(cloudswitch, LOW);}

void ON()
{
 int housestatus = digitalRead( housepower );
 
 if(housestatus== HIGH )
 { lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("SUNAMI SOLAR"));
  lcd.setCursor(2,1); lcd.print(F("KENYA LTD."));}
  
 if(housestatus== LOW )
 {toggle(); EEPROM.update(address,1);lcd.clear(); lcd.setCursor(0, 0);
  lcd.print(F("SUNAMI SOLAR"));lcd.setCursor(2,1); lcd.print(F("KENYA LTD.")); }
}

void OFF()
{
 int housestatus = digitalRead(housepower);

 if(housestatus== HIGH )
 {toggle(); delay(1000); EEPROM.write(address,0);lcd.clear(); 
  lcd.setCursor(0,0); lcd.print("TAFADHALI LIPIA "); lcd.setCursor(8,1); lcd.print("SOLAR...");Serial.println("Okay");}
  
   if(housestatus== LOW )
 {lcd.clear(); lcd.setCursor(0, 0); lcd.print("TAFADHALI LIPIA "); lcd.setCursor(8,1); lcd.print("SOLAR...");

 }
}
  
void report(String phone,String id)
{
    mySerial.println("AT+CMGF=1");  delay(1000);  
int housestatus = digitalRead( housepower ); int val=(EEPROM.read(address));
 delay(1000); 
 if(housestatus== HIGH )
  {
  mySerial.print( "AT+CMGS=\"" + phone + "\"\r\n");
  delay(1000);
 mySerial.print(id); mySerial.write("|");
  mySerial.write("sys activ,stat:");
  mySerial.print(val);
  delay(1000);
  mySerial.write((char)26);
  delay(1000);
  }
 if(housestatus== LOW )
 {mySerial.print( "AT+CMGS=\"" + phone + "\"\r\n");
  delay(1000);
  mySerial.print(id); mySerial.write("|");
  mySerial.write("sys inactiv,stat:");
  mySerial.print(val);
  delay(1000);
  mySerial.write((char)26);
  delay(1000);
 }
 }
 void checkmem()
 { 
 value = EEPROM.read(address); delay(1000);  
 if (value==1){ ON(); }
 else if (value==0){OFF();}   
 }
  
void conect()
{
mySerial.write("AT\r\n");delay(1000);readsim();  
Serial.println(F("Full Functions Initialised  "));
mySerial.println(" AT+CFUN=1\r\n");delay(1000);readsim(); delay(3000);               
mySerial.write("ATS0=1\r\n");delay(1000);readsim();
}

void cleanBuffer()
{Serial.flush();}

void readsim()
{ delay(10);
 if(mySerial.available()) // check if the esp is sending a message 
 {
 while(mySerial.available())
  {char c = mySerial.read(); //Serial.write(c); 
  resp=resp+c;
  } 
  
 }}

 void readsim1() {
  delay(10);  //arbitrary value
inputString="";
 if(mySerial.available()) // check if the esp is sending a message 
 {
 while(mySerial.available())
  {
incomingByte = mySerial.read();
   inputString += incomingByte;
  } 
 }
  delay(500); 
    Serial.print("Input string:");
   Serial.println(inputString); 
        // Serial.flush();
          delay(100);
 }


void beep() {
 digitalWrite(buzzer, HIGH); delay(100); 
 digitalWrite(buzzer, LOW); delay(100); 
}
