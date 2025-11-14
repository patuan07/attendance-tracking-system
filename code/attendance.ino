#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

uint8_t id;
String username;

SoftwareSerial fingerPrint(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define  start_door  14 // nut start
#define  exit_door   15 // nut exit
#define  servoPin    5 // dong co dong mo cua
#define  buzzer      4 // coi canh bao
void setup()
{
  finger.begin(57600);
  Serial.begin(9600);
  //khai bao động cơ servo
  myServo.attach(servoPin);
  myServo.write(0);
  //khai báo  còi
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  // khai báo nút nhấn
  pinMode(start_door, INPUT_PULLUP);
  pinMode(exit_door, INPUT_PULLUP);
  //khởi tạo LCD
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("He thong quan ly");
  lcd.setCursor(0, 1);
  lcd.print("THPT An Khanh");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Tim cam bien");
  delay(1000);
  while (1)
  {
    if (finger.verifyPassword()) // check mật khẩu kết nối đến cảm biến vân tay
    {
      lcd.setCursor(2, 0);
      lcd.print("  Da ket noi  ");
      delay(1000);
      lcd.clear();
      break;
    }
    else
    {
      
      lcd.setCursor(2, 0);
      lcd.print(" Mat ket noi ");
      lcd.setCursor(4, 1);
      lcd.print("Nhan RESET");
      lcd.clear();
    }
    delay(200);
  }
}
void loop()
{
  lcd.setCursor(3, 0);
  lcd.print("Nhan START ");
  lcd.setCursor(3, 1);
  lcd.print("de mo cua");
  char c = Serial.read(); // nhận tín hiệu từ máy tính xuống
  if (c == 's' || digitalRead(start_door) == 0) // nhấn nút start trên máy tính hoặc nút nhấn start
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Dat van tay");
    delay(2500);
    int result = getFingerprintIDez(); // kiểm tra vân tay
    if (result != -1)
    {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("ID: ");
      lcd.print(result);
      // truyền ID  vân taylên máy tính
      Serial.print("i");
      Serial.print(result);
      Serial.print("d");
      delay(500);
      while (Serial.available() > 0)
      {
        char s = Serial.read();
        username += s;
        delay(5);
      }
      lcd.setCursor(0, 1);
      lcd.print("Hi: ");
      lcd.print(username);
      delay(1500);
      Door();
      username = "";
      lcd.clear();
    }
    else
    {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Van tay loi");
      lcd.setCursor(2, 1);
      lcd.print("Moi thu lai!!");
      for (int i = 0; i < 5; i++)
      {
        digitalWrite(buzzer, LOW);
        delay(100);
        digitalWrite(buzzer, HIGH);
        delay(100);
      }
      lcd.clear();
    }
    
  }
  else if (c == 'e') // nhấn nút enroll trên máy tính
  {
    while (Serial.available() == 0);
    id = Serial.parseInt();
    getFingerprintEnroll(id);
    lcd.clear();
  }
  else if (c == 'x') // nhấn nút delete trên máy tính
  {
    while (Serial.available() == 0);
    id = Serial.parseInt();
    deleteFingerprint(id);
    lcd.clear();
  }
  else if (digitalRead(exit_door) == 0) // nhấn nút nhấn exit
  {
    Door();
  }
  else if ( c == 'd') // nhấn nút delete all trên máy tính
  {
    finger.emptyDatabase();
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Xoa tat ca");
    lcd.setCursor(4, 1);
    lcd.print("van tay");
    delay(1000);
    lcd.clear();
  }
  delay(100);
}
void Door()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Xin moi vao");
  myServo.write(60); // mở cửa
  delay(5000);      // thời gian mở cửa là 5 giây
  myServo.write(0); // đóng cửa
  delay(500);
  
}
uint8_t getFingerprintEnroll(uint8_t id)
{
  uint8_t p = -1;
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Dat van tay");
  delay(1500);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
        //Serial.println("Image Taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  // OK success
  p = finger.image2Tz(1);
  switch (p)
  {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Bo van tay");
  //Serial.println("Remove finger");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dat lai van tay");
  delay(1500);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        return;
    }
  }
  // OK success
  p = finger.image2Tz(2);
  switch (p)
  {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  //OK converted
  //Serial.print("Creating model for #: ");
  //Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Van tay phu hop");
    delay(500);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Commuication error");
    return p;
  }
  else if ( p == FINGERPRINT_ENROLLMISMATCH)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Khong phu hop");
    delay(1000);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }
  p = finger.storeModel(id);
  if ( p == FINGERPRINT_OK)
  {
    lcd.setCursor(4, 1);
    lcd.print("Da luu!");
    delay(1000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if ( p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Could not store in that location");
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Error writing to flash");
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }
}
int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if ( p != FINGERPRINT_OK)
    return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }
  return finger.fingerID;
}
uint8_t deleteFingerprint(uint8_t id)
{
  uint8_t p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("ID : ");
    lcd.print(id, DEC);
    lcd.setCursor(1, 1);
    lcd.print("Xoa thanh cong");
    delay(1000);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Loi! Thu lai!");
    delay(1000);
    return p;
  }
}
