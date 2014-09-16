#include <TinyWireS.h>
#define I2C_ADDRESS 0X04
#define SHUTDOWN_PIN 1  // Digital 1 -> PushButton
#define SLEEP_PIN 4     // Digital 4 -> Regulator 
#define CURRENT_SENSOR_PIN 3   // Analog 3 -> Current sensor
#define SLEEP_STATE 1  // The device is sleeping
#define SHUTDOWN_STATE 2 // The device is about to shut down
#define STATE 0           // Index of current state in i2c_regs
#define WAIT_TIMER 1      // Index of wait timer byte
#define SLEEP_TIME_L 2     // Low byte of sleep timer
#define SLEEP_TIME_H 3    // High byte of sleep timer
#define CURRENT_SENSOR_L 4 // Low byte of current sensor value
#define CURRENT_SENSOR_H 5 // High byte of current sensor value
#ifndef TWI_RX_BUFFER_SIZE  
#define TWI_RX_BUFFER_SIZE ( 16 ) // Max bytes to receive
#endif

volatile uint8_t i2c_regs[] =
{
0x0,  // Current mode
0x0,  // Sleep/shutdown wait timer
0x0,  // Sleep time low byte
0x0,  // Sleep time high byte
0x0,  // Current sensor low byte
0x0,  // Current sensor high byte
};

volatile byte reg_position;
const byte reg_size = sizeof(i2c_regs);

// I2C Request handler,from TinyWireS examples
// Send byte from i2c_regs array to i2c bus and increment current position.
void onRequest(){
  TinyWireS.send(i2c_regs[reg_position]);
    // Increment the reg position on each read, and loop back to zero
    reg_position++;
    if (reg_position >= reg_size)
    {
    reg_position = 0;
    }
}

// I2C receive handler, from TinyWireS examples
// Write bytes from i2c bus to i2c_regs array.
void onReceive(uint8_t howMany){
  if (howMany < 1) return;
  if (howMany > TWI_RX_BUFFER_SIZE) return;

  reg_position = TinyWireS.receive();
  howMany--;
  if (!howMany) return;
  
  while(howMany--){
    i2c_regs[reg_position] = TinyWireS.receive();
    reg_position++;
    if (reg_position >= reg_size){
      reg_position = 0;
    }
 }
}

// Setup i2c slave and output pins.
void setup(){
  TinyWireS.begin(I2C_ADDRESS);
  TinyWireS.onRequest(onRequest);
  TinyWireS.onReceive(onReceive);
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, LOW);
  pinMode(SLEEP_PIN, OUTPUT);
  digitalWrite(SLEEP_PIN, HIGH);
}  

unsigned long previousTime=0; // To measure 1s intervals
void loop(void){
  TinyWireS_stop_check();
  // Read & store current sensor value
  int value = analogRead(CURRENT_SENSOR_PIN);
  i2c_regs[CURRENT_SENSOR_H] = highByte(value);
  i2c_regs[CURRENT_SENSOR_L] = lowByte(value);
  
  unsigned long time = millis();
  if(time-previousTime > 1000){
    // 1s has elapsed - decrease timers
    previousTime = time;
    
    if(i2c_regs[WAIT_TIMER]>0) {
      // Decrease wait timer until zero.
      i2c_regs[WAIT_TIMER]-=1;
      if(i2c_regs[WAIT_TIMER]==0){
          // Trigger either sleep or shutdown pin
          // depending on current state.
          switch(i2c_regs[STATE]){
          case SLEEP_STATE:
            digitalWrite(SLEEP_PIN, LOW);
            break;
          case SHUTDOWN_STATE:
            digitalWrite(SHUTDOWN_PIN, HIGH);
            break;
        }
      }
    } else {
      // Wait time has elapsed      
      // Read sleep timer from i2c_regs array
      word sleepTimer = word(i2c_regs[SLEEP_TIME_H], i2c_regs[SLEEP_TIME_L]);    
      // Decrease sleep timer until zero
      if(sleepTimer>0){
        sleepTimer-=1;
      } else {
        // Sleep time has elapsed - turn power back on
        i2c_regs[STATE] = 0;
        digitalWrite(SHUTDOWN_PIN, LOW);
        digitalWrite(SLEEP_PIN, HIGH);
      }
      // Store timer value back to i2c_regs array
      i2c_regs[SLEEP_TIME_H] = highByte(sleepTimer);
      i2c_regs[SLEEP_TIME_L] = lowByte(sleepTimer);
    }
  }
}

