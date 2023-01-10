#define DEBUG_MAIN    true
#define DEBUG_RFID    true
#define DEBUG_HEATER  true
#define DEBUG_RTD     true
#define DEBUG_CHARGE  true
#define DEBUG_COLOR   true

#if (DEBUG_MAIN == true)
  #define BAUDRATE  115200
#endif

const char *FIRMWARE_VERSION = "0.0.1";

const uint8_t W_LED_PIN   = 16;
const uint8_t UV_LED_PIN  = 12;

typedef struct {
  const uint8_t csPin     = 17;
  const uint8_t misoPin   = 19;
  const uint8_t mosiPin   = 23;
  const uint8_t clkPin    = 18;
  const uint8_t drdPin    = 5;
  const float   RREF      = 430.0;
  const float   RNOMINAL  = 100.0;
  const float   TOLERANCE = 0.5;    // depends on sensor material
  float temp;
}temperature_t;


typedef struct {
  const uint8_t   pin           = 27;
  uint8_t         duty          = 0;
  bool            state         = false;
  uint8_t         channel       = 0;
  const uint16_t  frequency     = 1000;
  const uint8_t   resolution    = 8;
  const float     thermalPower  = 25.0;
}heater_t;


typedef enum : bool {
  PRESSED   = 1,
  RELEASED  = 0,
}button_state_t;


typedef struct {
  const uint8_t csPin     = 15;
  const uint8_t misoPin   = 19;
  const uint8_t mosiPin   = 23;
  const uint8_t clkPin    = 18;
  const uint8_t rstPin    = 13;
  const uint8_t irqPin    = 14;
}rfid_reader_t;


typedef struct {
  bool            working = false;
  uint16_t        value[4]; // red, green, blue, white
  const uint32_t  MAX_LUX = 200000; // acceptable lux to use color result
}colot_t;


typedef struct {
  const uint16_t WHITE_INTERVAL_AFTER = 60000; // time in milli sec 1 * 60 * 1000 // 1 minute
  const uint16_t UV_INTERVAL_AFTER = 300000; // time in milli sec 5 * 60 * 1000 // 5 miniute
  unsigned long lastWhiteOnTime;
  unsigned long lastUVOnTime;
}led_illumination_t;


typedef struct {
  const uint8_t    pin         = 32;
  bool             pullUp      = true;
  unsigned long    lastPressed = 0;
  button_state_t   state       = RELEASED;
}button_t;


typedef enum: uint8_t {
  NONE              = 0,
  CALCULATING_TIME  = 1,
  HEATING           = 2,  // wait for temp to read 44.5C
  COLLING           = 3,  // wait for temp to reduce to 35C
  MAINTANING_TEMP   = 4,
}process_stage_t;


typedef struct {
  bool              started   = false;
  process_stage_t   stage     = NONE;

  unsigned long     startTime = 0;
  unsigned long     totalTime = 0;
}main_process_t;


typedef struct {
  bool isPositive = false;
}result_t;
