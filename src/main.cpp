#include <Arduino.h>
#include <TaskScheduler.h>

#include "leds.h"
#include "vesc.h"


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
        drawBatteryPercent(pc);
      }
      // fsm.run_machine();
    });


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nready!\n");
  
  vesc.init(VESC_UART_BAUDRATE);

  initialiseLeds();
  
  runner.startNow();
  runner.addTask(t_GetVescValues);
  t_GetVescValues.enable();
}

void loop() {
  
  runner.execute();

  delay(10);
}
