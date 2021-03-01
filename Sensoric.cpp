#include <CapacitiveSensor.h>

typedef struct
{
    int identifier;
    CapacitiveSensor *cs;
    int controlChange;
    long threshold;
    int led;
    boolean active;
    boolean send;
}

Sensor;

// Register as many capacitive sensors as you want or as many as your Arduino can handle.

CapacitiveSensor s1 = CapacitiveSensor(23, 22); // 10M resistor between pins 4 and 2, 2 is sensor pin - add a wire and/or foil if desired
CapacitiveSensor s2 = CapacitiveSensor(25, 24);
CapacitiveSensor s3 = CapacitiveSensor(27, 26);

// Define a treshold or enter different values for each sensor
long threshold = 900;

// Treshold notes
// small    ~3000 (150)
// normal   ~2500 (150)
// big      ~5000 (150)

// A higher value means less trigger action

Sensor sensors[] = {
    {1, &s1, 40, threshold, 22, false, false},
    {2, &s2, 45, threshold, 22, false, false},
    {3, &s3, 50, threshold, 22, false, false}};

size_t listLength = sizeof(sensors) / sizeof(Sensor);

int midiKanal = 176;
int noteOn = 144;
int noteOff = 128;
int on = 127;
int off = 0;

void setup()
{
    s1.set_CS_AutocaL_Millis(0xFFFFFFFF); // Turn off auto calibration on sensor 1 - just as an example

    Serial.begin(115200);
    pinMode(22, OUTPUT); // LED
}

void loop()
{
    Sensor copy[listLength];
    for (int i = 0; i < listLength; i++)
    {
        // Serial.println(sensors[i].cs->capacitiveSensor(30)); // Get a log of the sensor
        copy[i] = sensors[i];
        updateSensors(&sensors[i]);
        stateHandler(&sensors[i], &copy[i]);
        sendMessage(&sensors[i]);
    }
}

void updateSensors(Sensor *s)
{
    long test = s->cs->capacitiveSensor(150);
    if (test > s->threshold)
    {
        s->active = true;
    }
    else
    {
        s->active = false;
    }
}

void stateHandler(Sensor *s1, Sensor *s2)
{
    if (s1->active && !s2->active && !s1->send)
    {
        s1->send = true;
    }
    else if (!s1->active && s2->active && !s1->send)
    {
        s1->send = true;
    }
    else
    {
        s1->send = false;
    }
}

void sendMessage(Sensor *s)
{
    if (s->send)
    {
        int i = s->identifier;
        if (s->active)
        {

            Serial.write(noteOn);
            Serial.write(s->controlChange);
            Serial.write(on);
            // Serial.println("on");
            delay(1);
        }
        else
        {
            Serial.write(noteOff);
            Serial.write(s->controlChange);
            Serial.write(on);
            // Serial.println("off");
            delay(1);
        }
        delay(1);
        s->send = false;
    }
}
