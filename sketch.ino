#include <Servo.h>

const int c = 6; //motorok száma
Servo motor[c];  //Servo osztály tagjainak deklarálása

int delta_s;                 //ekkora utat kell megtennie a motornak a cél eléréshez
int sign[c];                 //mozgás előjele
int delay_time[c];           //késleltési idő 2 állapot között
int start_pos[c];            //kezdő pozíció mozgásnál
int travel_time[c];          //mozgás megtételéhez szükséges idő
int longest_travel_time = 0; //leghosszabb ideig tartó mozgás párhuzamos vezérlésnél
int enabled[c];              //szükséges-e mozognia a motornak
int target[c];               //a mozgás végpontja párhuzamos vezérlésnél
int single_target;           //a mozgás végpontja egyszerű vezérlésnél
int motor_speed[c];          //motor mozgási sebessége
int selected;                //kiválaszott motor egyszerű vezérlésnél

int command_is(const char *command);
void debug(String label, int value, int end_line = 0);

void setup()
{
    Serial.begin(19200);

    //motorok pinhez rendelése
    motor[0].attach(11);
    motor[1].attach(10);
    motor[2].attach(9);
    motor[3].attach(6);
    motor[4].attach(5);
    motor[5].attach(3);
}

void loop()
{
    if (Serial.available())
    {
        if (command_is("parallel"))
        {
            Serial.println("PARALLEL CONTROL");

            for (int m = 0; m < c; m++)
            {
                target[m] = Serial.parseInt();
                motor_speed[m] = Serial.parseInt();

                debug("motor", m + 1);
                debug("target", target[m]);
                debug("speed", motor_speed[m], 1);
            }

            for (int m = 0; m < 6; m++)
            {
                delta_s = target[m] - motor[m].read();
                if (delta_s == 0)
                {
                    enabled[m] = 0;
                    debug("disabled motor", m, 1);
                }
                else
                {
                    enabled[m] = 1;

                    sign[m] = delta_s / abs(delta_s); //pozitív vagy negatív irányú mozgás
                    start_pos[m] = motor[m].read();
                    travel_time[m] = 1000 * abs(delta_s) / motor_speed[m];
                    delay_time[m] = travel_time[m] / abs(delta_s);

                    if (travel_time[m] > longest_travel_time)
                        longest_travel_time = travel_time[m];

                    debug("motor", m + 1);
                    debug("travel t.", travel_time[m]);
                    debug("delta_s", delta_s, 1);
                    debug("sign", sign[m]);
                    debug("delay", delay_time[m], 1);
                }
            }

            unsigned long start = millis();
            int step[6] = {0, 0, 0, 0, 0, 0};

            while (start + longest_travel_time + 500 >= millis())
            {
                for (int k = 0; k < 6; k++)
                {
                    if ((millis() >= start + delay_time[k] * step[k]) && enabled[k])
                    {
                        int pos = start_pos[k] + step[k] * sign[k];
                        motor[k].write(pos);
                        if (pos != target[k])
                            step[k]++;
                        else
                            enabled[k] = 0;
                    }
                }
            }
        }

        else if (command_is("single"))
        {
            Serial.println("SINGLE CONTROL");

            selected = Serial.parseInt() - 1;
            if (selected >= 0 && selected < 6)
            {
                single_target = Serial.parseInt();
                motor[selected].write(single_target);

                debug("motor", selected);
                debug("target", single_target, 1);
            }
        }
    }
}

int command_is(const char *command)
{
    char serial_input[10] = "";
    Serial.readBytesUntil('x', serial_input, 10);

    if (strcmp(serial_input, command) == 0)
        return 1;
    else
        return 0;
}

void debug(String label, int value, int end_line = 0)
{
    Serial.print(label);
    Serial.print(": ");
    Serial.print(value);
    if (end_line)
        Serial.print('\n');
    else
        Serial.print(", ");
}