#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*Motor Driver L298N*/
/*Left DC Motor*/
const int ENA = 6;
const int IN1 = 10;
const int IN2 = 9;
/*Right DC Motor*/
const int ENB = 5;
const int IN3 = 8;
const int IN4 = 7;

/*Bluetooth HC-05*/
/*pin state*/
const int btPin = 3;
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
const int buttonWaktu = 2;
int buttonState = 0;
int lastButtonState = 0;

/*Setting Waktu Otomatisasi*/
int pilihanWaktu[] = {0, 1, 2, 30, 60};
int noPilihan = 0;
int countDownTime = 5;

/*Waktu Otomatisasi untuk Robot*/
int waktuOtomatisasi;

/*Limit Switch Sensor*/
int middle = 4;
int left = 11;
int right = 12;

void setup() {
    /*Motor Driver L298N*/
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    /*Kecepatan DC Motor*/
    analogWrite(ENA, 150);
    analogWrite(ENB, 130);

    /*Bluetooth HC-05: Setup Komunikasi Serial*/
    Serial.begin(9600);
    pinMode(btPin, INPUT);

    /*Relay 5V (Vacuum Cleaner)*/
    pinMode(vacuumCleaner, OUTPUT);

    /*LCD Setup*/
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("Vacuum Robot");

    /*Limit Switch Sensor*/
    pinMode(middle, INPUT);
    pinMode(left, INPUT);
    pinMode(right, INPUT);
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
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN4, LOW);
    }
    if(dataAndroid == "r") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN3, HIGH);
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
            dataAndroid = ""; // Untuk atur dataAndroid tidak 0
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
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
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
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
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
        int rightBumper = digitalRead(right);
        int middleBumper = digitalRead(middle);
        int leftBumper = digitalRead(left);
        int randNumber;
        if(middleBumper == 1) {
            berhenti();
            delay(100);
            mundur();
            delay(800);
            randNumber = random(1,3);
            if(randNumber == 1) {
                belokKanan();
                delay(1200);
            } else {
                belokKiri();
                delay(1200);
            }
        } else if(leftBumper == 1) {
            berhenti();
            delay(100);
            mundur();
            delay(800);
            belokKanan();
            delay(1200);
        } else if(rightBumper == 1) {
            berhenti();
            delay(100);
            mundur();
            delay(800);
            belokKiri();
            delay(1200);
        } else {
            maju();
        }
    }
    waktuOtomatisasi = 0;
    berhenti();
    digitalWrite(vacuumCleaner, LOW);
    if(!btConnected) {
        lcd.setCursor(5,1);
        lcd.print("        ");
    }
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
    }
    if(!btConnected && waktuOtomatisasi == 0) {
        settingWaktu();
    }
    if(waktuOtomatisasi != 0) {
        otomatisasi();
    }
}

