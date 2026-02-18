#include <Arduino.h>
#include <Wire.h>

#include "Arduino_BMI270_BMM150.h"
#include <Adafruit_MLX90393.h>

// XA1110 GPS (SparkFun I2C GPS) + TinyGPS++
#include <SparkFun_I2C_GPS_Arduino_Library.h>
#include <TinyGPS++.h>

static const int SDA_PIN = 21;
static const int SCL_PIN = 22;

static const uint8_t MLX_ADDR = 0x18;
// XA1110 / MT333x default I2C address is 0x10 (library default)
static const uint8_t GPS_ADDR = 0x10; // (not strictly needed; begin() uses default) :contentReference[oaicite:3]{index=3}

// UART2 pins for Logomatic
static const int LOG_TX = 17;  // ESP32 TX2 → Logomatic RX
static const int LOG_RX = 16;  // Not required, but defined

Adafruit_MLX90393 mlx;

// GPS objects
I2CGPS myI2CGPS;
TinyGPSPlus gps;

static float headingDegFromXY(float mx_uT, float my_uT) {
  float heading = atan2f(my_uT, mx_uT) * 180.0f / PI;
  if (heading < 0) heading += 360.0f;
  return heading;
}

void setup() {
  Serial.begin(9600);   // USB monitor
  Serial2.begin(9600, SERIAL_8N1, LOG_RX, LOG_TX);  // Logomatic
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  Serial.println("\n=== BMI270 + MLX90393 + XA1110 (I2C GPS) ===");
  Serial2.println("=== Logging Started ===");

  if (!IMU.begin()) {
    Serial.println("ERROR: BMI270 failed");
    while (1);
  }

  if (!mlx.begin_I2C(MLX_ADDR, &Wire)) {
    Serial.println("ERROR: MLX90393 failed");
    while (1);
  }

  // Start XA1110 GPS over I2C (uses default address 0x10 in the library) :contentReference[oaicite:4]{index=4}
  if (!myI2CGPS.begin(Wire, 100000)) {
    Serial.println("ERROR: XA1110 GPS failed (no I2C response). Check wiring/pullups.");
    while (1);
  }

  // CSV header
  String header =
    "ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps,"
    "mx_uT,my_uT,mz_uT,heading_deg,"
    "lat,lon,alt_m,sats,hdop";
  Serial.println(header);
  Serial2.println(header);
}

void loop() {
  // ---- Read IMU + Mag ----
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

  bool aok = IMU.readAcceleration(ax, ay, az);
  bool gok = IMU.readGyroscope(gx, gy, gz);
  bool mok = mlx.readData(&mx, &my, &mz);

  if (!aok || !gok || !mok) {
    Serial.println("Read fail (IMU/Mag)");
    return;
  }

  float head = headingDegFromXY(mx, my);

  // ---- Read GPS bytes over I2C and feed TinyGPS++ ----
  while (myI2CGPS.available()) {          // available() returns count of new bytes :contentReference[oaicite:5]{index=5}
    gps.encode(myI2CGPS.read());          // read() returns next byte :contentReference[oaicite:6]{index=6}
  }

  // Pull GPS fields (NaN / -1 when not valid yet)
  double lat = NAN, lon = NAN, alt_m = NAN;
  int sats = -1;
  double hdop = NAN;

  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }
  if (gps.altitude.isValid()) {
    alt_m = gps.altitude.meters();
  }
  if (gps.satellites.isValid()) {
    sats = gps.satellites.value();
  }
  if (gps.hdop.isValid()) {
    hdop = gps.hdop.hdop();
  }

  // ---- Build one log line ----
  String line =
    String(ax,4) + "," + String(ay,4) + "," + String(az,4) + "," +
    String(gx,3) + "," + String(gy,3) + "," + String(gz,3) + "," +
    String(mx,2) + "," + String(my,2) + "," + String(mz,2) + "," +
    String(head,1) + "," +
    (isnan(lat) ? "" : String(lat, 6)) + "," +
    (isnan(lon) ? "" : String(lon, 6)) + "," +
    (isnan(alt_m) ? "" : String(alt_m, 1)) + "," +
    String(sats) + "," +
    (isnan(hdop) ? "" : String(hdop, 2));

  Serial.println(line);
  Serial2.println(line);

  delay(1000);
}
