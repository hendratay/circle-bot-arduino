#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <NewPing.h>

/*Setting untuk konversi menit ke waktu digital
  Namun Karena mikrokontroler tidak bisa multitasking,
  Jadi tidak digunakan*/
#define SECS_PER_MIN  (60UL)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)

/*Motor Driver L298N*/
/*Left DC Motor*/
/*const int ENA = 9;*/
const int IN1 = 8;
const int IN2 = 7;
/*Right DC Motor*/
/*const int ENB = 6;*/
const int IN3 = 5;
const int IN4 = 4;

/*Bluetooth HC-05*/
const int btPin = 2;
boolean btConnected = false;
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
const int buttonWaktu = 3;
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

/*Servo TowerPro SG90 9g*/
Servo servo_ultrasonik;

void setup() {
    /*Motor Driver L298N*/
    /*pinMode(ENA, OUTPUT);*/
    /*pinMode(ENB, OUTPUT);*/
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    /*Kecepatan DC Motor*/
    /*analogWrite(ENA, 150);*/
    /*analogWrite(ENB, 150);*/

    /*Bluetooth HC-05: Setup Komunikasi Serial*/
    Serial.begin(9600);
    pinMode(btPin, INPUT);

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
    if(dataAndroid.toInt()) {
        waktuOtomatisasi = dataAndroid.toInt();
    }
    if(dataAndroid == "0") {
        waktuOtomatisasi = 0;
    }
}

/*void time(int val){  */
    /*int minutes = numberOfMinutes(val);*/
    /*int seconds = numberOfSeconds(val);*/

    /*lcd.setCursor(8,1);*/
    /*lcd.print(minutes);*/
    /*lcd.setCursor(10,1);*/
    /*lcd.print(":");*/
    /*lcd.setCursor(11,1);*/
    /*lcd.print(seconds);*/
/*}*/

/*void timeCountDown() {*/
    /*waktuOtomatisasi *= 60;*/
    /*for(waktuOtomatisasi > 0; waktuOtomatisasi--;) {*/
        /*lcd.clear();*/
        /*lcd.setCursor(0,0);*/
        /*lcd.print("Vacuum Robot");*/
        /*lcd.setCursor(2,1);*/
        /*lcd.print("Time:");*/
        /*time(waktuOtomatisasi);*/
        /*delay(1000);*/
    /*}*/
/*}*/

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
    delay(100);
    lastButtonState = buttonState;
    if(buttonState != HIGH) {
        lcd.setCursor(15,0);
        lcd.print(countDownTime);
        if(countDownTime > 0) {
            countDownTime -= 1;
        } else if(countDownTime == 0) {
            waktuOtomatisasi = pilihanWaktu[noPilihan];
            lcd.setCursor(15,0);
            lcd.print("O");
        }
        delay(1000);
    }
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
    delay(70);
    int cm = ultrasonik.ping_cm();
    /*Jika cm = 0 maka Jarak terlalu jauh, sehingga jadikan 250cm*/
    if(cm == 0) {
        cm=250;
    }
    return cm;
}

int bacaSensorUltrasonikKiri() {
    servo_ultrasonik.write(110);
    delay(500);
    int jki = bacaSensorUltrasonik();
    delay(100);
    servo_ultrasonik.write(60);
    return jki;
}

int bacaSensorUltrasonikKanan() {
    servo_ultrasonik.write(10);
    delay(500);
    int jka = bacaSensorUltrasonik();
    delay(100);
    servo_ultrasonik.write(60);
    return jka;
}

void cekKiriKanan() {
    int jarakKiri = 0;
    int jarakKanan = 0;
    delay(50);
    jarakKiri = bacaSensorUltrasonikKiri();    
    delay(300);
    jarakKanan = bacaSensorUltrasonikKanan();    
    delay(300);
    if(jarakKiri <= 15 && jarakKanan <= 15) {
        mundur();
        delay(200);
        berhenti();
        delay(300);
        cekKiriKanan();
    } else if(jarakKiri > jarakKanan) {
        belokKiri();
        delay(200);
        berhenti();
    } else if(jarakKanan > jarakKiri) {
        belokKanan();
        delay(200);
        berhenti();
    }
}


void otomatisasi() {
    uint32_t waktuOtomatisasiMillis = waktuOtomatisasi * 60000L;
    uint32_t tStart = millis();
    while(millis()-tStart < waktuOtomatisasiMillis) {
        if(Serial.available() > 0) {
            dataAndroid = "";
        }
        while(Serial.available() > 0) {
            dataAndroidBit = ((byte)Serial.read());
            if(dataAndroidBit == ':') {break;}
            else {dataAndroid += dataAndroidBit;}
            delay(1);
        }
        if(dataAndroid == "0") {
            break;
        }
        digitalWrite(vacuumCleaner, HIGH);
        servo_ultrasonik.write(60);
        int jarak = bacaSensorUltrasonik();
        if(jarak <= 15) {
            berhenti();
            delay(300);
            mundur();
            delay(200);
            berhenti();
            delay(300);
            cekKiriKanan();
        } else {
            maju();
        }
    }
    waktuOtomatisasi = 0;
    berhenti();
    digitalWrite(vacuumCleaner, LOW);
    /*Untuk Menghentikan Loop Otomatisasi*/
    dataAndroid = "0";
}

void loop() {
    if(!btConnected) {
        if(digitalRead(btPin)==HIGH) {
            btConnected = true;
        }
    }
    if(btConnected) {
        pengontrolan();
        lcd.setCursor(2,1);
        lcd.print("Pengontrolan");
        lcd.setCursor(15,0);
        lcd.print(" ");
    } else if(!btConnected && waktuOtomatisasi == 0) {
        settingWaktu();
    }
    if(waktuOtomatisasi != 0) {
        otomatisasi();
    }
}
