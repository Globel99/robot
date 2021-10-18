#include <Servo.h>

//motorok száma
const int c = 6;
Servo motor[c];

int delta;
int sign[c];
int delay_time[c];
int start_pos[c];
int travel_time[c];
int longest_travel_time = 0;
int enabled[c];
int target[c];
int single_target;
int motor_speed[c];
int selected;

int commandIs(const char *command);
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
        if (commandIs("parallel"))
        {
            Serial.println("PARALLEL");

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
                delta = target[m] - motor[m].read();
                if(delta == 0) 
                {
                    enabled[m] = 0;
                    debug("disabled motor", m, 1);
                }else
                {
                    enabled[m] = 1;

                    sign[m] = delta / abs(delta); //pozitív vagy negatív irányú mozgás
                    start_pos[m] = motor[m].read();
                    travel_time[m] = 1000 * abs(delta) / motor_speed[m];
                    delay_time[m] = travel_time[m] / abs(delta);

                    if(travel_time[m] > longest_travel_time) longest_travel_time = travel_time[m];

                    debug("motor", m + 1);
                    debug("travel t.", travel_time[m]);
                    debug("delta", delta, 1);
                    debug("sign", sign[m]);
                    debug("delay", delay_time[m], 1);
                }
            }

            unsigned long start = millis();
            int step[6] = {0, 0, 0, 0, 0, 0};

            while(start + longest_travel_time + 500 >= millis())
            {
                for (int k = 0; k < 6; k++)
                {
                    if((millis() >= start + delay_time[k] * step[k]) && enabled[k])
                    {
                        int pos = start_pos[k] + step[k] * sign[k];
                        motor[k].write(pos);
                        if(pos != target[k]) step[k]++;
                    }
                }
            }
        }
    
        else if(commandIs("single"))
        {
            Serial.println("PARALLEL");

            selected = Serial.parseInt() - 1;
            if(selected >= 0 && selected < 6)
            {
                single_target = Serial.parseInt();
                motor[selected].write(single_target);

                debug("motor", selected);
                debug("target", single_target, 1);
            }
        }
    }
}

int commandIs(const char *command)
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
    if(end_line) Serial.print('\n'); else Serial.print(", ");
}