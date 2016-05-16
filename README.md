# Joystick-Servo-Control
A private Arduino based system to control multiple servos using one joystick.

##Basic
The folder [Basic](https://github.com/Simsso/Joystick-Servo-Control/tree/master/Basic) contains the *basic* version of the project. It uses only one Arduino which is connected to one joystick and multiple servos.

The only required library is the `Servo` library.

##Wireless
The folder [Wireless](https://github.com/Simsso/Joystick-Servo-Control/tree/master/Wireless) contains code for at least three Arduinos. It is the basic version extended with a wireless connection between the joystick and the servos.

It includes the libraries `VirtualWire`, `SoftwareSerial` and `Servo`.

 * _Transmitter-Arduino (Wireless)_:
The [Transmitter](https://github.com/Simsso/Joystick-Servo-Control/tree/master/Wireless/Transmitter) is the Arduino which is connected to the joystick and a transmitter module. It reads the joystick values and converts them into servo angles which it then broadcasts to all Receiver-Arduinos.
 * _Receiver-Arduinos (Wireless)_: The [Receiver](https://github.com/Simsso/Joystick-Servo-Control/tree/master/Wireless/Receiver)-Arduinos (you can have multiple of them) receive the servo angles which the transmitter sends. They hand them over to their particular Servo-Arduino via a `SoftwareSerial` data link. Each Receiver-Arduino is connected to exactly one Servo-Arduino. The reason for this pair-system is that both, the `Servo` and the `VirtualWire` library make use of the same interrupt vector which makes it hard to run them simultaneously on the same microcontroller.
 * _Servo Control (Wireless)_: The [Servo Control](https://github.com/Simsso/Joystick-Servo-Control/tree/master/Wireless/ServoControl)-Arduinos basically get the servo data from their individual Receiver-Arduino and drive the servos.

##Manual (German)
###Veränderung der Servowinkel Funktion
Die Funktion für den Servowinkel heißt `static unsigned transferFunction(unsigned x)`. Ihr Parameter `x` ist eine Ganzzahl im Interval von 0 bis 1023 (10 bit analoger Eingang). Ihr Rückgabewert muss im Bereich von 0 bis 180 liegen, was dem Winkel des Servos entspricht. 90 entspricht der Nullstellung eines Servos. 

Der Funktionsrumpf enthält in Version 1.0 eine Polynomfunktion dritten Grades, die im Code wie folgt definiert ist: `6.6822581E-7 * x*x*x - .001026855 * x*x + .5263598 * x`

Es kann hier jede beliebige Funktion f(x) eingesetzt werden, mit der Einschränkung im Wertebereich (maximal von 0 bis 180). Eine lineare Ansteuerung würde beispielsweise so aussehen: `180/1024 * x`

####Invertierung der Servo-Drehrichtung
Um die Drehrichtung zu invertieren, muss der Servo mit `bool inverted = true` initialisiert werden.

Beispiel:
```C++
// servo 1
unsigned servo1HighPins[] = { TASTE_1 }, servo1LowPins[] = { TASTE_2 };
CustomServo servo1(SERVO_1_PIN, DREHGEBER_1, servo1HighPins, sizeof(servo1HighPins) / sizeof(unsigned), servo1LowPins, sizeof(servo1LowPins) / sizeof(unsigned), true);
```

###Hinzufügen eines Servos
####1. Anpassung der Servoanzahl
Die Codezeile `#define NUM_SERVOS 6` auf die neue Zahl an Servos anpassen.

####2. Erstellen des Servo-Objekts
In der `void setup()` Funktion den Code 

```C++
// servo N
unsigned servoNHighPins[] = { TASTE_1, TASTE_2 }, servoNLowPins[] = {};
CustomServo servoN(SERVO_N_PIN, DREHGEBER_3, servoNHighPins, sizeof(servoNHighPins) / sizeof(unsigned), servoNLowPins, sizeof(servoNLowPins));
```

hinter den bereits erstellten Servos einfügen und `N` durch die Nummer des Servos ersetzen. 

* `servoNHighPins` beinhaltet die Pins, die `HIGH` sein müssen, damit der Servo angesteuert wird.
* `servoNLowPins` beinhaltet die Pins, die `LOW` sein müssen, damit der Servo angesteuert wird.
* `SERVO_N_PIN` ist der Pin, über den der Servo gesteuert wird (muss kein PWM Pin sein).
* `DREHGEBER_3` ist der analoge Eingang, der den Servowinkel definiert.

####3. Servo-Objekt hinzufügen
Das erzeugte Servo-Objekt muss abschließend noch zum Array aller Servos hinzgefügt werden.

`CustomServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6, servoN };`
