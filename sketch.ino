#include <Servo.h>

//motorok száma
const int c = 6;
Servo motor[c];

int delta_s;
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

bool command_is(const char *command);
void debug(String label, int value, bool end_line = true);

void setup()
{
    Serial.begin(19200);

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
            Serial.println("parallel");

            for (int m = 0; m < c; m++)
            {
                target[m] = Serial.parseInt();
                motor_speed[m] = Serial.parseInt();

                debug("motor", m + 1);
                debug("target", target[m]);
                debug("speed", motor_speed[m], true);
            }

            for (int m = 0; m < c; m++)
            {
                delta_s = target[m] - motor[m].read();
                if(delta_s == 0) 
                {
                    enabled[m] = 0;
                    debug("disabled motor", m, true);
                }else
                {
                    enabled[m] = 1;

                    sign[m] = delta_s / abs(delta_s); //pozitív vagy negatív irányú mozgás
                    start_pos[m] = motor[m].read();
                    travel_time[m] = 1000 * abs(delta_s) / motor_speed[m];
                    delay_time[m] = travel_time[m] / abs(delta_s);

                    if(travel_time[m] > longest_travel_time) longest_travel_time = travel_time[m];

                    debug("motor", m + 1);
                    debug("travel t.", travel_time[m]);
                    debug("delta_s", delta_s, true);
                    debug("sign", sign[m]);
                    debug("delay", delay_time[m], true);
                }
            }

            unsigned long start = millis();
            int step[6] = {1, 1, 1, 1, 1, 1};

            while(start + longest_travel_time + 1000 >= millis())
            {
                for (int m = 0; m < c; m++)
                {
                    if((millis() >= start + delay_time[m] * step[m]) && enabled[m])
                    {
                        int pos = start_pos[m] + step[m] * sign[m];
                        motor[m].write(pos);
                        if(pos != target[m]) step[m]++;
                    }
                }
            }
        }
    
        else if(command_is("single"))
        {
            Serial.println("single");
            selected = Serial.parseInt() - 1;
            
            if(selected >= 0 && selected < c)
            {
                single_target = Serial.parseInt();
                motor[selected].write(single_target);

                debug("motor", selected);
                debug("target", single_target, true);
            }
        }
    }
}

bool command_is(const char *command)
{
    char serial_input[10] = "";
    Serial.readBytesUntil('x', serial_input, 10);

    if (strcmp(serial_input, command) == 0)
        return true;
    else
        return false;
}

void debug(String label, int value, bool end_line)
{
    Serial.print(label);
    Serial.print(": ");
    Serial.print(value);
    if (end_line)
        Serial.print('\n');
    else
        Serial.print(", ");
}