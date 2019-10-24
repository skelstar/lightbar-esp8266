#include <Arduino.h>
#include <TaskScheduler.h>
#include <Fsm.h>

#include "leds.h"
#include "vesc.h"


enum EventsEnum
{
  POWERED_UP,
  MOVING,
  STOPPED_MOVING
} event;

//-------------------------------
State state_poweredup(
  [] { Serial.printf("state_poweredup \n"); },
  [] { 
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
    drawBatteryPercent(batteryVoltage);
  }, 
  NULL
);

Fsm fsm(&state_poweredup);

void addFsmTransitions() {

  uint8_t event = POWERED_UP;
  // fsm.add_transition(&state_battery_voltage_screen, &state_server_disconnected, event, NULL);

  event = MOVING;
  fsm.add_transition(&state_poweredup, &state_moving, event, NULL);
  fsm.add_transition(&state_stopped, &state_moving, event, NULL);
  

  event = STOPPED_MOVING;
  // when state connected is entered it will transition to new state after 3 seconds
  fsm.add_timed_transition(&state_moving, &state_stopped, 3000, NULL);
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
      }
      else
      {
        Serial.printf("Batt: %.1f\n", batteryVoltage);
        int8_t pc = getBatteryPercentage(batteryVoltage);
        if (moving) {
          fsm.trigger(MOVING);
        }
        else {
          fsm.trigger(STOPPED_MOVING);
        }
        // drawBatteryPercent(pc);
        fsm.run_machine();
      }
      // fsm.run_machine();
    });


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nready!\n");

  addFsmTransitions();
  fsm.run_machine();
  
  vesc.init(VESC_UART_BAUDRATE);

  initialiseLeds();
  
  runner.startNow();
  runner.addTask(t_GetVescValues);
  t_GetVescValues.enable();
}

void loop() {
  
  runner.execute();

  // fsm.run_machine();

  delay(10);
}
