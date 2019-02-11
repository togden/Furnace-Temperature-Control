/////////////////////////////////
// Holding Temperature [deg C] //
/////////////////////////////////

// This is the temperature you want to heat the sample to, in degrees C.
unsigned long holdingTemp = 920;

///////////////////////////
// Holding Time  [hours] //
///////////////////////////

// This is how long you want the sample to stay at the holding temperature, in hours.
float holdingTime = 3;

//////////////////////////////
// Heating Rate [deg C/min] //
//////////////////////////////

// This is how quickly you want to heat your sample.
// 2 deg C/min is the standard.
// Only change this value if you know what you're doing!
float heatingRate = 2;








// Assign the pins to use for temperature control
int tempDownPin = 3;
int tempUpPin = 5;

// Starting Temperature [deg C]
int startingTemp = 25;

// Set current temperature equal to starting temperature
int currentTemp = startingTemp;

float minPerDeg = 1 / heatingRate;
unsigned long millisPerDeg = minPerDeg * (60000);

float heatingDuration = ((holdingTemp - startingTemp) / heatingRate) - 0.5;

unsigned long startTime = millis();
unsigned long currentHeatingTime = millis();
unsigned long currentHoldingTime;
unsigned long currentCoolingTime;
unsigned long printerval = 30000;
unsigned long holdingTimeMillis = holdingTime * 3600000;

bool startedHeating = 0;
bool startedCooling = 0;
bool holdingTempReached = 0;



void setup()
{
  pinMode(tempDownPin, OUTPUT);
  pinMode(tempUpPin, OUTPUT);
  Serial.begin(9600);
  while (! Serial);
  Serial.print("Current Temperature is: ");
  Serial.print(currentTemp);
  Serial.println(" [deg C]");
  Serial.print("Holding Temperature is: ");
  Serial.print(holdingTemp);
  Serial.println(" [deg C]");
  Serial.print("Specified Heating Rate: ");
  Serial.print(heatingRate);
  Serial.println(" [deg C/min]");
  Serial.print("Minutes per Degree: ");
  Serial.print(minPerDeg);
  Serial.println(" [min/Deg C]");
  // Serial.print("Seconds per Degree: ");
  // Serial.print(secPerDeg);
  // Serial.println(" [sec/Deg C]");
  Serial.print("Milliseconds per Degree: ");
  Serial.print(millisPerDeg);
  Serial.println(" [ms/Deg C]");
  Serial.print("Calculated Heating Time: ");
  Serial.print(((heatingDuration*2)/60)+holdingTime);
  Serial.println(" [hrs]");
  Serial.print("Temperature holding at ");
  Serial.print(startingTemp);
  Serial.println(" deg C for 2 minutes before heating sample.");
  Serial.print("Remaining time: ");
  float remainingTime = (120000 - (millis() - startTime)) / 60000.0;
  Serial.print(remainingTime);
  Serial.println(" minutes.");

}


void loop() {
  // Check to see if the sample has been in the oven for more than 5 minutes
  if ( (millis() - startTime) < 120000 && startedHeating == 0 ) {

    // Every 30 seconds print a message indicating that the sample is being held at 30 deg C for 5 minutes.
    if ( (millis() - currentHeatingTime) >= printerval ) {
      Serial.print("Temperature holding at ");
      Serial.print(startingTemp);
      Serial.println(" deg C for 2 minutes before heating sample.");
      Serial.print("Remaining time: ");
      float remainingTime = (120000 - (millis() - startTime)) / 60000.0;
      Serial.print(remainingTime);
      Serial.println(" minutes.");
      currentHeatingTime = millis();

    }

  } else {

    // Check to see if we have started heating the sample. If no, set the current time and begin heating.
    if ( startedHeating == 0) {

      startedHeating = 1;
      Serial.println("Beginning to heat sample.");
      raiseTemp();
      startTime = millis();
    }

    // If the current temperature is less than the desired temp, and the time since the last
    // increase is greater than our heating rate, raise the temperature.
    if (currentTemp < holdingTemp && (millis() - currentHeatingTime) >= (millisPerDeg - 1000) && holdingTempReached == 0) {

      // Raise the furnace temperature
      raiseTemp();

    } else if (holdingTempReached == 0 && currentTemp >= holdingTemp) {

      holdingTempReached = 1;
      currentHoldingTime = millis();
      Serial.println("Holding Temperature Reached!");
      Serial.print("Current Temp = ");
      Serial.println(currentTemp);
      Serial.print("Time to reach holding temp: ");
      Serial.print((millis() - startTime) / 60000.0);
      Serial.println(" minutes");
    }

    else if ( holdingTempReached == 1 && (millis() - currentHoldingTime) >= holdingTimeMillis ) {

      if ( startedCooling == 0 ) {

        Serial.println("Done holding. Beginning to lower temp.");
        Serial.print("Time held: ");
        Serial.print((millis() - currentHoldingTime) / 60000.0);
        Serial.println(" minutes");
        startedCooling = 1;
        lowerTemp();
      }
      if (currentTemp > startingTemp && (millis() - currentCoolingTime) >= (millisPerDeg - 1000) && startedCooling == 1) {

        // Lower the oven temp.
        lowerTemp();
      }

    }
  }
}

////////////////////////////////
// Raise Temperature Function //
////////////////////////////////

void raiseTemp() {

  analogWrite(tempUpPin, 255);
  delay(500);
  analogWrite(tempUpPin, 0);
  delay(500);
  analogWrite(tempUpPin, 255);
  currentHeatingTime = millis();
  delay(250);
  analogWrite(tempUpPin, 0);

  ++currentTemp;
  Serial.print("Temperature increased! Current Temp = ");
  Serial.print(currentTemp);
  Serial.println(" [deg C]");


}

////////////////////////////////
// Lower Temperature Function //
////////////////////////////////

void lowerTemp() {
  analogWrite(tempDownPin, 255);
  delay(500);
  analogWrite(tempDownPin, 0);
  delay(500);
  analogWrite(tempDownPin, 255);
  currentCoolingTime = millis();
  delay(250);
  analogWrite(tempDownPin, 0);

  --currentTemp;
  Serial.print("Temperature Decreased! Current Temp = ");
  Serial.print(currentTemp);
  Serial.println(" [deg C]");

}

