#include <Arduino.h>
#include <TaskScheduler.h>
#include <Fsm.h>

#include "leds.h"
#include "vesc.h"


enum EventsEnum
{
  POWERED_UP,
  EV_MOVING,
  EV_STOPPED_MOVING,
  EV_VESC_OFFLINE
} event;

//-------------------------------
State state_poweredup(
  [] { Serial.printf("state_poweredup \n"); },
  [] { 
    int8_t pc = getBatteryPercentage(batteryVoltage);
    drawBatteryPercent(batteryVoltage);
  }, 
  NULL
);

State state_moving(
  [] { 
    Serial.printf("state_moving \n");
    allLedsOn(COLOUR_WHITE);
  }, 
  NULL, 
  NULL
);

State state_stopped(
  [] { Serial.printf("state_stopped \n"); },
  [] { 
    int8_t pc = getBatteryPercentage(batteryVoltage);
    drawBatteryPercent(pc);
  }, 
  NULL
);

State state_vesc_offline(
  [] { 
    Serial.printf("state_vesc_offline \n"); 
    allLedsOn(COLOUR_RED);
  },
  NULL,
  NULL
);

Fsm fsm(&state_poweredup);

void addFsmTransitions() {

  uint8_t event = POWERED_UP;

  event = EV_MOVING;
  fsm.add_transition(&state_poweredup, &state_moving, event, NULL);
  fsm.add_transition(&state_stopped, &state_moving, event, NULL);
  fsm.add_transition(&state_vesc_offline, &state_moving, event, NULL);

  event = EV_STOPPED_MOVING;
  // when state connected is entered it will transition to new state after 3 seconds
  fsm.add_timed_transition(&state_moving, &state_stopped, 3000, NULL);
  fsm.add_transition(&state_poweredup, &state_stopped, event, NULL);

  event = EV_VESC_OFFLINE;
  fsm.add_transition(&state_moving, &state_vesc_offline, event, NULL);
  fsm.add_transition(&state_poweredup, &state_vesc_offline, event, NULL);
  fsm.add_transition(&state_stopped, &state_vesc_offline, event, NULL);  
}

Scheduler runner;

#define GET_FROM_VESC_INTERVAL 1000

Task t_GetVescValues(
    /*period*/ 500,
    TASK_FOREVER,
    [] {
      // btn1 can put vesc into offline
      bool vescOnline = getVescValues() == true; 

      if (vescOnline == false)
      {
        Serial.printf("VESC offline\n");
        fsm.trigger(EV_VESC_OFFLINE);
      }
      else
      {
        // Serial.printf("Batt: %.1f\n", batteryVoltage);
        if (moving) {
          fsm.trigger(EV_MOVING);
        }
        else {
          fsm.trigger(EV_STOPPED_MOVING);
        }
      }
      fsm.run_machine();
    });


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nready!\n");

  addFsmTransitions();
  fsm.run_machine();
  
  vesc.init(VESC_UART_BAUDRATE);

  initialiseLeds(COLOUR_OFF);
  
  runner.startNow();
  runner.addTask(t_GetVescValues);
  t_GetVescValues.enable();
}

void loop() {
  
  runner.execute();

  // fsm.run_machine();

  delay(10);
}
