#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <NewPing.h>

/*Motor Driver L298N*/
/*Left DC Motor*/
const int ENA = 9;
const int IN1 = 8;
const int IN2 = 7;
/*Right DC Motor*/
const int ENB = 6;
const int IN3 = 5;
const int IN4 = 4;

/*Bluetooth HC-05*/
char dataAndroidBit;
String dataAndroid;

/*Relay 5V (For Vacuum Cleaner)
  Can Using Transistor, but I don't have any*/
const int vacuumCleaner = 13;

/*LCD
  Parameter 1 : Address 
  Sisanya Lupa
  Parameter 9,10 : Setting Backlight*/
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/*Button Untuk Setting Waktu*/
const int buttonWaktu = 12;
int buttonState = 0;
int lastButtonState = 0;

/*Setting Waktu Otomatisasi*/
int pilihanWaktu[] = {0, 5, 10, 30, 60};
int noPilihan = 0;
int countDownTime = 5;

/*Waktu Otomatisasi untuk Robot*/
int waktuOtomatisasi;

/*Sensor Ultrasoink HC-SR04*/
#define trig_pin A1
#define echo_pin A2
#define jarak_maximum 200
NewPing ultrasonik(trig_pin, echo_pin, jarak_maximum);
int jarak;

/*Servo TowerPro SG90 9g*/
Servo servo_ultrasonik;

void setup() {
    /*Motor Driver L298N*/
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    /*Kecepatan DC Motor*/
    analogWrite(ENA, 100);
    analogWrite(ENB, 100);

    /*Bluetooth HC-05: Setup Komunikasi Serial*/
    Serial.begin(9600);

    /*Relay 5V (Vacuum Cleaner)*/
    pinMode(vacuumCleaner, OUTPUT);

    /*LCD Setup*/
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("Vacuum Robot");

    /*Servo di bawah ultrasonik*/
    servo_ultrasonik.attach(10); // Perlu PWM
}

void pengontrolan() {
    if(Serial.available() > 0) {
        /*Send Data Waktu Otomatisasi ke Android Ketika Bluetooth Terkoneksi*/
        Serial.write(waktuOtomatisasi);

        dataAndroid = "";
    }
    while(Serial.available() > 0) {
        dataAndroidBit = ((byte)Serial.read());
        if(dataAndroidBit == ':') {break;}
        else {dataAndroid += dataAndroidBit;}
        delay(1);
    }
    if(dataAndroid == "u") {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, LOW);
    }
    if(dataAndroid == "d") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN4, HIGH);
    }
    if(dataAndroid == "l") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, LOW);
    }
    if(dataAndroid == "r") {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, LOW);
    }
    if(dataAndroid == "s") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, LOW);
    }
    if(dataAndroid == "v") {
        digitalWrite(vacuumCleaner, HIGH);
    }
    if(dataAndroid == "nv") {
        digitalWrite(vacuumCleaner, LOW);
    }
    if(isDigit(dataAndroid.toInt())) {
        waktuOtomatisasi = dataAndroid.toInt();
    }
    if(dataAndroid == "0") {
        waktuOtomatisasi = 0;
    }
}

void settingWaktu() {
    buttonState = digitalRead(buttonWaktu);
    if(buttonState != lastButtonState) {
        if(buttonState == HIGH && noPilihan < 4) {
            noPilihan++;
            lcd.setCursor(5,1);
            lcd.print(pilihanWaktu[noPilihan]);
            lcd.setCursor(8,1);
            lcd.print("Menit");
            countDownTime = 5;
        } else if (buttonState == HIGH && noPilihan == 4) {
            noPilihan = 1;
            lcd.setCursor(5,1);
            lcd.print(pilihanWaktu[noPilihan]);
            lcd.setCursor(6,1);
            lcd.print(" ");
            lcd.setCursor(8,1);
            lcd.print("Menit");
            countDownTime = 5;
        }
    } 
    delay(150);
    lastButtonState = buttonState;
    lcd.setCursor(15,0);
    lcd.print(countDownTime);
    if(countDownTime > 0) {
        countDownTime -= 1;
    } else if(countDownTime == 0) {
        waktuOtomatisasi = pilihanWaktu[noPilihan];
        lcd.setCursor(5,1);
        lcd.print(hour:minute:second);
    }
    delay(1000);
}

void maju() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
}

void mundur() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
}

void belokKiri() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
}

void belokKanan() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
}

void berhenti() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
}

int bacaSensorUltrasonik() {
    delay(100);
    int cm = ultrasonik.ping_cm();
    /*Jika cm = 0 maka Jarak terlalu jauh, sehingga jadikan 250cm*/
    if (cm == 0){
        cm=200;
    }
    return cm;
}

int bacaSensorUltrasonikKiri() {
    servo_ultrasonik.write(170);
    int jarakKiri = bacaSensorUltrasonik();
    servo_ultrasonik.write(115);
    return jarakKiri;
}

int bacaSensorUltrasonikKanan() {
    servo_ultrasonik.write(50);
    int jarakKanan = bacaSensorUltrasonik();
    servo_ultrasonik.write(115);
    return jarakKanan;
}

void cekKiriKanan() {
    int jarakKiri = 0;
    int jarakKanan = 0;
    jarakKiri = bacaSensorUltrasonikKiri();    
    jarakKanan = bacaSensorUltrasonikKanan();    
    if(jarakKiri >= jarakKanan) {
        belokKiri();
        maju();
    } else if(jarakKanan >= jarakKiri) {
        belokKanan();
        maju();
    } else if(jarakKiri <= 3 && jarakKanan <= 3) {
        mundur();
        cekKiriKanan();
    }
}

void otomatisasi() {
    uint32_t waktuOtomatisasiMillis = waktuOtomatisasi * 60000L;
    for(uint32_t tStart = millis();  (millis()-tStart) < waktuOtomatisasiMillis;) {
        digitalWrite(vacuumCleaner, HIGH);
        servo_ultrasonik.write(115);
        jarak = bacaSensorUltrasonik();
        if(jarak <= 3) {
            cekKiriKanan();
        } else {
            maju();
        }
    }
    digitalWrite(vacuumCleaner, LOW);
    berhenti();
}

void loop() {
    pengontrolan();
    if(waktuOtomatisasi == 0) {
        settingWaktu();
    }
}
