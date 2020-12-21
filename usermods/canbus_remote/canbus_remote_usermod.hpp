#pragma once

#include "wled.h"
#include "canbus_message.hpp"

#include <CAN.h>

static const uint8_t LED_MODES[] = 
{
    FX_MODE_STATIC,
    FX_MODE_RAINBOW,
    FX_MODE_BREATH,
};

static uint8_t currModeIdx = 0;

class CanbusRemoteUsermod : public Usermod
{
    public:

        void setup()
        {
            if (!CAN.begin(500E3)) 
            {
                Serial.println("Starting CAN failed!");
            }

            can_message_t m;
            m.id = MESSAGE_ID_ON_OFF;

            if(bri > 0)
            {
                m.value0 = 1;
                turnedOn = true;
            }
            else
            {
                m.value0 = 0;
                turnedOn = false;
            }

            sendCanMessage(&m);
        }

        // void connected()
        // {
        //     Serial.println("Connected to WiFi!");
        // }

        void loop()
        {
            can_message_t m;

            if(turnedOn && bri == 0)
            {
                m.id = MESSAGE_ID_ON_OFF;
                m.value0 = 0;
                sendCanMessage(&m);

                turnedOn = false;
            }
            else if(!turnedOn && bri > 0)
            {
                m.id = MESSAGE_ID_ON_OFF;
                m.value0 = 1;
                sendCanMessage(&m);

                turnedOn = true;
            }

            if(processCanMessage(&m))
            {
                switch(m.id)
                {
                    case MESSAGE_ID_ON_OFF:
                    {
                        
                        if(m.value0 != 0 && bri == 0) // ON
                        {
                            // Serial.println("MESSAGE_ID_ON_OFF ON");
                            bri = briLast;
                        }
                        else if(m.value0 == 0 && bri > 0) // OFF
                        {
                            // Serial.println("MESSAGE_ID_ON_OFF OFF");
                            briLast = bri;
                            bri = 0;
                        }
                    } break;

                    case MESSAGE_ID_SPEED:
                    {
                        // Serial.println("MESSAGE_ID_SPEED");
                        effectSpeed = m.value0;
                    } break;

                    case MESSAGE_ID_MODE:
                    {
                        // Serial.println("MESSAGE_ID_MODE");
                        currModeIdx = (currModeIdx + 1) % sizeof(LED_MODES);
                        effectCurrent = LED_MODES[currModeIdx];
                    } break;

                    case MESSAGE_ID_BRIGHTNESS:
                    {
                        // Serial.println("MESSAGE_ID_BRIGHTNESS");
                        bri = (127 - m.value0) * 2;
                    } break;

                    case MESSAGE_ID_INTENSITY:
                    {
                        // Serial.println("MESSAGE_ID_INTENSITY");
                        effectIntensity = (127 - m.value0) * 2;
                    } break;

                    case MESSAGE_ID_COLOR:
                    {
                        // Serial.println("MESSAGE_ID_COLOR");
                        col[0] = m.value0;
                        col[1] = m.value1;
                        col[2] = m.value2;

                    } break;

                    case MESSAGE_ID_SYNC_STATE:
                    {
                        // Serial.println("MESSAGE_ID_SYNC_STATE");
                        can_message_t m = {0};

                        m.id = MESSAGE_ID_ON_OFF;
                        m.value0 = bri > 0 ? 0x1: 0x0;
                        sendCanMessage(&m);


                    } break;

                    default:
                        break;
                };

                colorUpdated(NOTIFIER_CALL_MODE_DIRECT_CHANGE);
            }
        }

    private:
        bool processCanMessage(can_message_t* m)
        {
            bool retval = false;
            int count = CAN.parsePacket();
            uint8_t* buf = (uint8_t*)m;

            if(count > 0 && CAN.packetRtr() == false)
            {
                uint8_t cnt = 0;

                if(CAN.packetId() == CAN_MESSAGE_ID)
                {
                    // Serial.println("ID match!");

                    while(CAN.available() && cnt < sizeof(can_message_t))
                    {
                        buf[cnt] = CAN.read();
                        cnt++;
                    }

                    retval = true;
                }

                /* Always consume the whole message (even if it was wrongly sized!) */
                while(CAN.available()) CAN.read();
            }

            return retval;
        }

        void sendCanMessage(can_message_t* m)
        {
            // Serial.printf("Sending message: id=%d, len=%d\r\n", m->id, sizeof(can_message_t));
            CAN.beginPacket(CAN_MESSAGE_ID);
            CAN.write((uint8_t*)m, sizeof(can_message_t));
            CAN.endPacket();
        }

    private:
        bool turnedOn;
};