# Joystick-Servo-Control
A private Arduino based system to control multiple servos using one joystick.

##Manual (German)
###Veränderung der Servowinkel Funktion
Die [Funktion für den Servowinkel](https://github.com/Simsso/Joystick-Servo-Control/blob/master/Sketch/Sketch.ino#L67) heißt `static unsigned transferFunction(unsigned x)`. Ihr Parameter `x` ist eine Ganzzahl im Interval von 0 bis 1023 (10 bit analoger Eingang). Ihr Rückgabewert muss im Bereich von 0 bis 180 liegen, was dem Winkel des Servos entspricht. 90 entspricht der Nullstellung eines Servos. 

Der [Funktionsrumpf](https://github.com/Simsso/Joystick-Servo-Control/blob/master/Sketch/Sketch.ino#L68) enthält in Version 1.0 eine Polynomfunktion dritten Grades, die im Code wie folgt definiert ist: `6.6822581E-7 * x*x*x - .001026855 * x*x + .5263598 * x`

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
Die [Codezeile](https://github.com/Simsso/Joystick-Servo-Control/blob/master/Sketch/Sketch.ino#L20) `#define NUM_SERVOS 6` auf die neue Zahl an Servos anpassen.

####2. Erstellen des Servo-Objekts
In der `void setup()` Funktion den Code 

```C++
// servo N
unsigned servoNHighPins[] = { TASTE_1, TASTE_2 }, servoNLowPins[] = {};
CustomServo servoN(SERVO_N_PIN, DREHGEBER_3, servoNHighPins, sizeof(servoNHighPins) / sizeof(unsigned), servoNLowPins, sizeof(servoNLowPins));
```

[hinter den bereits erstellten Servos](https://github.com/Simsso/Joystick-Servo-Control/blob/master/Sketch/Sketch.ino#L141) einfügen und `N` durch die Nummer des Servos ersetzen. 

* `servoNHighPins` beinhaltet die Pins, die `HIGH` sein müssen, damit der Servo angesteuert wird.
* `servoNLowPins` beinhaltet die Pins, die `LOW` sein müssen, damit der Servo angesteuert wird.
* `SERVO_N_PIN` ist der Pin, über den der Servo gesteuert wird (muss kein PWM Pin sein).
* `DREHGEBER_3` ist der analoge Eingang, der den Servowinkel definiert.

####3. Servo-Objekt hinzufügen
Das erzeugte Servo-Objekt muss abschließend noch zum [Array aller Servos](https://github.com/Simsso/Joystick-Servo-Control/blob/master/Sketch/Sketch.ino#L142) hinzgefügt werden.

`CustomServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6, servoN };`
