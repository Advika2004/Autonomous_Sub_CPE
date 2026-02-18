//choose analog pin 0
const int pressurePin = A0;
const float R_SHUNT = 250.0; //the 250 Ohm resistor that was provided
const float P_MAX = 100.0;  //setting some range for the pressure

void setup() {
  Serial.begin(9600);
}

void loop() {

  //read the analog value from A0
  int adcValue = analogRead(pressurePin);

  //convert that voltage from 0-5V range
  float voltage = adcValue * (5.0 / 1023.0);

  //V = IR convert it to current
  float current_mA = (voltage / R_SHUNT) * 1000.0;

  //go from current to pressure
  // 4 mA = 0 PSI
  // 20 mA = P_MAX
  float pressure = ((current_mA - 4.0) / 16.0) * P_MAX;

  //debug prints
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | Current: ");
  Serial.print(current_mA, 2);
  Serial.print(" mA | Pressure: ");
  Serial.print(pressure, 2);
  Serial.println(" PSI");

  //delay 1 second printing
  delay(1000);
}

// EXTRA COMMENTS FOR CLARITY //
// __________________________ //

// - Sensor output: 4–20 mA linear current
// - Supply voltage: 8–30 VDC
// - Our unit is a gauge pressure sensor (PSIG), not absolute
// - Gauge pressure is measured relative to atmospheric pressure
// - In open air = 0 PSIG

// - Relationship:
//   PSIG = PSIA - Patm

// - Atmospheric pressure ≈ 14.7 PSIA at sea level
// - Absolute sensors would read 14.7 PSI in open air; gauge sensors read 0

// - 4 mA represents minimum pressure
// - 20 mA represents maximum pressure
// - Current span = 16 mA
// - 4 mA is used instead of 0 mA so that 0 mA indicates a fault

// - Arduino cannot measure current directly — it measures voltage
// - A 250 Ω shunt resistor converts current to voltage using:
//   V = I × R

// - With 250 Ω:
//   4 mA → 1.0 V
//   20 mA → 5.0 V

// - Therefore:
//   4–20 mA signal becomes a 1–5 V signal

// - Correct wiring:
//   +12V or +24V → RED wire
//   BLACK wire → 250 Ω resistor
//   Other end of resistor → GND
//   Arduino A0 → junction between BLACK wire and resistor
//   Arduino GND → power supply GND
//   Yellow/Green wire → not connected

// - Pressure conversion formula:
//   Pressure = ((I - 4) / 16) × Pmax

// - In open air you should see:
//   ~4.00 mA
//   ~1.00 V
//   ~0 PSIG

// - Slightly negative readings near zero are normal due to:
//   ADC resolution
//   Resistor tolerance
//   Sensor zero tolerance

// - 12 V supply works for testing
// - 24 V is standard in industry
// - Higher supply voltage gives more compliance headroom

// - Key concept:
//   Arduino measures voltage,
//   resistor converts current to voltage,
//   4 mA = minimum pressure

