#define DEBUG_MAIN    true
#define DEBUG_RFID    true
#define DEBUG_HEATER  true
#define DEBUG_RTD     true
#define DEBUG_CHARGE  true
#define DEBUG_COLOR   true
#define DEBUG_WIFI    true

#if (DEBUG_MAIN == true)
  #define BAUDRATE  115200
#endif

const char *FIRMWARE_VERSION = "0.0.1";

const uint8_t W_LED_PIN   = 16;
const uint8_t UV_LED_PIN  = 12;

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* serverName = "https://bactosure-dev-default-rtdb.firebaseio.com";

char oldError[20];
char newError[20];
const char *ERROR_ENDPOINT = "/error.json?auth=8JZWaLRXmT9y4JHxqs3jjjpiQyzAkpji17qiJxma";
const char *SUMMARY_ENDPOINT = "/summary.json?auth=8JZWaLRXmT9y4JHxqs3jjjpiQyzAkpji17qiJxma";
const char *DETAILS_ENDPOINT = "/detail.json?auth=8JZWaLRXmT9y4JHxqs3jjjpiQyzAkpji17qiJxma";


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
  const float     thermalPower  = 25.0; // depends on the material
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
  float           hsv[3];
  uint8_t         rgb[3];
  const uint32_t  MAX_LUX = 200000; // acceptable lux to use color result if value greater than this it means lid is open
}colot_t;


typedef struct {
  const uint16_t WHITE_INTERVAL_AFTER = 60000; // time in milli sec 1 * 60 * 1000 // 1 minute // white light illumination interval
  const uint16_t UV_INTERVAL_AFTER = 300000; // time in milli sec 5 * 60 * 1000 // 5 miniute // uv light illumination interval
  unsigned long lastWhiteOnTime;
  unsigned long lastUVOnTime;
}led_illumination_t;

typedef struct {
  const uint8_t   Pins[3] = {34, 35, 26};
  bool            errorState = false;
  bool            flashing = false;
  unsigned long   lastBlink = 0;
  const uint16_t  blinkDelay = 500; // time in milli sec
}rgb_t;


typedef enum : uint8_t{
  COLITAG    = 0,
  COLILERT   = 1,
  COLILERT18 = 2,
}test_type_t;

typedef struct {
  const uint8_t COLITAG[2][3] = {
                                  {175, 55, 238}, // E.Coli
                                  {241, 235, 156} // T. Coliform
                                };
  const uint8_t COLILERT[2][3] = {
                                   {175, 55, 238}, // E.Coli
                                   {241, 235, 156} // T. Coliform
                                 };
  const uint8_t COLILERT18[2][3] = {
                                     {175, 55, 238}, // E.Coli
                                     {241, 235, 156} // T. Coliform
                                   };
}comparator_color_t;


typedef struct { // information provided by RFID tag
  test_type_t sampleType = COLITAG;
  char        sampleUUID[20];
  char        userName[15];
  char        lon[15];
  char        lat[15];
}sample_info_t;


typedef enum: uint8_t {
  NONE              = 0,
  CALCULATING_TIME  = 1,
  HEATING           = 2,  // wait for temp to read 44.5C
  COLLING           = 3,  // wait for temp to reduce to 35C
  MAINTANING_TEMP   = 4,
  COMPLETE          = 5,
}process_stage_t;


typedef struct {
  bool              started   = false;
  process_stage_t   stage     = NONE;
  unsigned long     startTime = 0;
  unsigned long     totalTime = 0;
}main_process_t;


typedef struct {
  bool isPositive = false;
  char rgb[3];
  unsigned long timeOfPositive;
  
}result_t;
