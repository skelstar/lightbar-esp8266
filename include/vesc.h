#include <vesc_comms.h>

#define MOTOR_POLE_PAIRS 7
#define WHEEL_DIAMETER_MM 97
#define MOTOR_PULLEY_TEETH 15
#define WHEEL_PULLEY_TEETH 36 // https://hobbyking.com/en_us/gear-set-with-belt.html
#define NUM_BATT_CELLS 11

uint8_t vesc_packet[PACKET_MAX_LENGTH];

float batteryVoltage = 0.0;
bool moving = false;

#define VESC_UART_BAUDRATE 115200

#define BATT_100PC                      NUM_BATT_CELLS * 4.2
#define BATT_0PC                        NUM_BATT_CELLS * 3.1
#define POWERING_DOWN_BATT_VOLTS_START  NUM_BATT_CELLS * 3.0

vesc_comms vesc;

//-----------------------------------------------------------------------------------
bool getVescValues()
{
  bool success = vesc.fetch_packet(vesc_packet) > 0;

  if (success)
  {
    batteryVoltage = vesc.get_voltage(vesc_packet);
    moving = vesc.get_rpm(vesc_packet) > 50;
  }
  else {
    batteryVoltage = 0.0;
    moving = false;
  }
  return success;
}

uint8_t getBatteryPercentage(float voltage) {
  float voltsLeft = voltage - BATT_0PC;
  float voltsAvail = BATT_100PC - BATT_0PC;

  uint8_t percent = 0;
  if ( voltage > BATT_0PC ) { 
    percent = (voltsLeft /  voltsAvail) * 100;
  }
  if (percent > 100) {
    percent = 100;
	}
  return percent;
}