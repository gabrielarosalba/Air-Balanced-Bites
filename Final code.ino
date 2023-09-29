#include <ArduinoJson.h>  // Install ArduinoJson library from Arduino Library Manager
#include <SPI.h>
#include <MFRC522.h>

/* Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

 //for the RFID
#define RST_PIN         5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

int block = 2;
byte readbackblock[18];
//

//for the logic
const int MAX_WORD_LENGTH = 16;  // Maximum length of each word

char** words;       // Pointer to store dynamically allocated array
int wordCount = 0;  // Current number of words in the array
//

// const char* json = "{\"apple\":{\"p\":2.2,\"f\":7.4,\"fa\":0.2,\"c\":1.4},\"banana\":{\"p\":12.1,\"f\":2.6,\"fa\":0.2,\"c\":2.0}}";
// StaticJsonDocument<200> doc;  // JSON document

String status;

const char* json = R"({
  "Chickenbreast": {"p": 31, "f": 3.6, "fa": 0, "c": 0},
  "Salmon": {"p": 22, "f": 13, "fa": 0, "c": 0},
  "Tofu": {"p": 8, "f": 4, "fa": 1, "c": 2},
  "Lentils": {"p": 9, "f": 0.4, "fa": 8, "c": 20},
  "Greekyogurt": {"p": 10, "f": 0, "fa": 0, "c": 4},
  "Eggs": {"p": 6, "f": 5, "fa": 0, "c": 0.6},
  "Quinoa": {"p": 4, "f": 2, "fa": 2.6, "c": 21},
  "Cottagecheese": {"p": 14, "f": 1, "fa": 0, "c": 3},
  "Turkeybreast": {"p": 29, "f": 1, "fa": 0, "c": 0},
  "Almonds": {"p": 21, "f": 49, "fa": 12, "c": 22},
  "Wheyppowder": {"p": 24, "f": 1, "fa": 0, "c": 3},
  "Blackbeans": {"p": 15, "f": 1, "fa": 16, "c": 45},
  "Chiaseeds": {"p": 5, "f": 9, "fa": 10, "c": 12},
  "Edamame": {"p": 11, "f": 6, "fa": 8, "c": 10},
  "Tempeh": {"p": 16, "f": 9, "fa": 0, "c": 9},
  "Pumpkinseeds": {"p": 30, "f": 49, "fa": 5, "c": 11},
  "Greekyogurt": {"p": 10, "f": 0, "fa": 0, "c": 3},
  "Chickpeas": {"p": 15, "f": 3, "fa": 12, "c": 45},
  "Peanutbutter": {"p": 25, "f": 50, "fa": 10, "c": 20},
  "Hempseeds": {"p": 33, "f": 49, "fa": 1, "c": 1},
  "Avocado": {"p": 2, "f": 14, "fa": 7, "c": 9},
  "Oliveoil": {"p": 0, "f": 14, "fa": 0, "c": 0},
  "Almondbutter": {"p": 6, "f": 17, "fa": 4, "c": 6},
  "Walnu": {"p": 4, "f": 18, "fa": 2, "c": 4},
  "Flaxseeds": {"p": 4, "f": 11, "fa": 27, "c": 29},
  "Coconutoil": {"p": 0, "f": 14, "fa": 0, "c": 0},
  "Cashews": {"p": 5, "f": 44, "fa": 2, "c": 32},
  "Sesameoil": {"p": 0, "f": 14, "fa": 0, "c": 0},
  "Pistachios": {"p": 6, "f": 27, "fa": 10, "c": 28},
  "Brazilnu": {"p": 4, "f": 19, "fa": 2, "c": 4},
  "Sunflowerseeds": {"p": 6, "f": 27, "fa": 9, "c": 20},
  "Peanutoil": {"p": 0, "f": 14, "fa": 0, "c": 0},
  "Macadamianu": {"p": 3, "f": 76, "fa": 9, "c": 14},
  "Hazelnu": {"p": 15, "f": 62, "fa": 9, "c": 17},
  "Pecans": {"p": 3, "f": 71, "fa": 10, "c": 14},
  "Canolaoil": {"p": 0, "f": 14, "fa": 0, "c": 0},
  "Darkchocolate": {"p": 7, "f": 43, "fa": 11, "c": 45},
  "Oa": {"p": 17, "f": 6, "fa": 10, "c": 66},
  "Wholewheatbread": {"p": 8, "f": 2, "fa": 6, "c": 16},
  "Raspberries": {"p": 1, "f": 0, "fa": 8, "c": 14},
  "Broccoli": {"p": 3, "f": 0, "fa": 2, "c": 6},
  "Brusselssprou": {"p": 3, "f": 0, "fa": 4, "c": 9},
  "Peas": {"p": 5, "f": 0, "fa": 5, "c": 17},
  "Barley": {"p": 3, "f": 0, "fa": 6, "c": 32},
  "Artichokes": {"p": 4, "f": 0, "fa": 7, "c": 11},
  "Sweetpotatoes": {"p": 2, "f": 0, "fa": 4, "c": 20},
  "Popcorn": {"p": 3, "f": 1, "fa": 3, "c": 19},
  "Apples": {"p": 0, "f": 0, "fa": 2, "c": 14},
  "Carro": {"p": 1, "f": 0, "fa": 3, "c": 12},
  "Brownrice": {"p": 7, "f": 2, "fa": 2, "c": 33},
  "Bananas": {"p": 1, "f": 0, "fa": 3, "c": 22},
  "Oranges": {"p": 1, "f": 0, "fa": 3, "c": 12},
  "Blueberries": {"p": 1, "f": 0, "fa": 3, "c": 14},
  "Strawberries": {"p": 1, "f": 0, "fa": 2, "c": 7},
  "Mangoes": {"p": 1, "f": 0, "fa": 3, "c": 15},
  "Watermelon": {"p": 1, "f": 0, "fa": 1, "c": 8},
  "Pineapple": {"p": 0, "f": 0, "fa": 2, "c": 13},
  "Kiwi": {"p": 1, "f": 0, "fa": 3, "c": 14},
  "Corn": {"p": 3, "f": 1, "fa": 4, "c": 21},
  "Wholewheatpasta": {"p": 7, "f": 2, "fa": 5, "c": 37}
})";


StaticJsonDocument<3000> doc;  // JSON document

float globalProtein = 0.0, globalFiber = 0.0, globalFats = 0.0, globalCarbs = 0.0;
int proteinPercentage, fiberPercentage, fatsPercentage, carbsPercentage;

// Pins for controlling the bags
//Protein bag
const int bag1InPin = 56;
const int bag1OutPin = 57;
//Carbs bag
const int bag2InPin = 58;
const int bag2OutPin = 59;
//Fats bag
const int bag3InPin = 60;
const int bag3OutPin = 61;
//Fiber bag
const int bag4InPin = 62;
const int bag4OutPin = 63;


// Variables to store the inflation percentages
float bag1Inflation = 0.0;
float bag2Inflation = 0.0;
float bag3Inflation = 0.0;
float bag4Inflation = 0.0;


// Variables to store the time when each bag started inflating
unsigned long bag1StartTime = 0;
unsigned long bag2StartTime = 0;
unsigned long bag3StartTime = 0;
unsigned long bag4StartTime = 0;


// Variables to store the inflation durations for each bag
unsigned long bag1InflationDuration = 0;
unsigned long bag2InflationDuration = 0;
unsigned long bag3InflationDuration = 0;
unsigned long bag4InflationDuration = 0;


// Duration to inflate/deflate 10% (in milliseconds)
const unsigned long INFLATE_DURATION = 150;
const unsigned long DEFLATE_DURATION = 900;
const unsigned long INFLATE_DURATION2 = 80;
const unsigned long DEFLATE_DURATION2 = 900;
const unsigned long INFLATE_DURATION3 = 150;
const unsigned long DEFLATE_DURATION3 = 900;
const unsigned long INFLATE_DURATION4 = 150;
const unsigned long DEFLATE_DURATION4 = 900;

bool isFirstLoop = true;

const int addLedPin = 47;
const int deleteLedPin = 48;
const int ingredientLedPin = 49;

void setup() {
    Serial.begin(9600);
    deserializeJson(doc, json);  // Deserialize the JSON document


    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }


    // Set the output modes for the control pins
    pinMode(bag1InPin, OUTPUT);
    pinMode(bag1OutPin, OUTPUT);
    pinMode(bag2InPin, OUTPUT);
    pinMode(bag2OutPin, OUTPUT);
    pinMode(bag3InPin, OUTPUT);
    pinMode(bag3OutPin, OUTPUT);
    pinMode(bag4InPin, OUTPUT);
    pinMode(bag4OutPin, OUTPUT);

    pinMode(addLedPin, OUTPUT);
    pinMode(deleteLedPin, OUTPUT);
    pinMode(ingredientLedPin, OUTPUT);

    digitalWrite(bag1InPin, LOW);
    digitalWrite(bag1OutPin, LOW);
    digitalWrite(bag2InPin, LOW);
    digitalWrite(bag2OutPin, LOW);
    digitalWrite(bag3InPin, LOW);
    digitalWrite(bag3OutPin, LOW);
    digitalWrite(bag4InPin, LOW);
    digitalWrite(bag4OutPin, LOW);

    digitalWrite(addLedPin, LOW);
    digitalWrite(deleteLedPin, LOW);
    digitalWrite(ingredientLedPin, LOW);

}

void(*resetFunc) (void) = 0;

void loop() {
    if (isFirstLoop) {
        bag1InflationDuration = (100) * DEFLATE_DURATION / 10;
        bag2InflationDuration = (100) * DEFLATE_DURATION2 / 10;
        bag3InflationDuration = (100) * DEFLATE_DURATION3 / 10;
        bag4InflationDuration = (100) * DEFLATE_DURATION4 / 10;
        activateDeflationAll(bag1InflationDuration, bag2InflationDuration, bag3InflationDuration, bag4InflationDuration);
         bag1InflationDuration = 0;// (30)*INFLATE_DURATION / 10;
        bag2InflationDuration = 0;
        bag3InflationDuration = 0;
        bag4InflationDuration = (30)*INFLATE_DURATION4 / 10;;
        //activateInflationAll(bag1InflationDuration, bag2InflationDuration, bag3InflationDuration, bag4InflationDuration);
        isFirstLoop = false;  // Set isFirstLoop to false after the first loop
    }
    //rfid sensor part
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }
    float protein, fiber, fats, carbs;
    String word = "";
    readBlock(block, readbackblock);//read the block back
    Serial.print("read block: ");
    for (int j = 0; j < 16; j++)//print the block conten
    {
        //Serial.write (readbackblock[j]);//Serial.write() transmi the ASCII numbers as human readable characters to serial monitor
        char c = readbackblock[j];

        if (c != ' ') {
            word += c;
        }
    }
    Serial.println(word);

    //end of rfid sensor part


    //if word is equals to __ then change the state
    if (!word.compareTo("Add")) {
        status = "Add";
        Serial.println("ADD");
        digitalWrite(addLedPin, HIGH);
        digitalWrite(deleteLedPin, LOW);
        digitalWrite(ingredientLedPin, LOW);
    }
    else if (!word.compareTo("Delete")) {
        status = "Delete";
        Serial.println("DELETE");
        digitalWrite(addLedPin, LOW);
        digitalWrite(deleteLedPin, HIGH);
        digitalWrite(ingredientLedPin, LOW);
    }
    else if (!word.compareTo("Balance")) {
        status = "Balance";
        Serial.println("BALANCE THE TABLE");
        digitalWrite(ingredientLedPin, HIGH);
        digitalWrite(addLedPin, HIGH);
        digitalWrite(deleteLedPin, HIGH);
        delay(1000);  // Give some time for the message to be sent
        digitalWrite(addLedPin, LOW);
        digitalWrite(deleteLedPin, LOW);
        digitalWrite(ingredientLedPin, LOW);
        inflating(globalProtein, globalFiber, globalFats, globalCarbs);
    }
    else if (!word.compareTo("Restart")) {
        status = "Restart";
        Serial.println("RESET");
        digitalWrite(ingredientLedPin, HIGH);
        digitalWrite(addLedPin, HIGH);
        digitalWrite(deleteLedPin, HIGH);
        delay(1000);  // Give some time for the message to be sent
        digitalWrite(ingredientLedPin, LOW);
        digitalWrite(addLedPin, LOW);
        digitalWrite(deleteLedPin, LOW);
        resetFunc();
    }
    else {
        // Search for the word in the JSON document
        JsonObject nutrient = doc[word];
        if (!nutrient.isNull()) {
            Serial.println("setting values");
            protein = nutrient["p"];
            fats = nutrient["f"];
            fiber = nutrient["fa"];
            carbs = nutrient["c"];
        }
        if (status == "Add") {
            updateGlobalIndexes(protein, fiber, fats, carbs);
            delay(500);
            digitalWrite(ingredientLedPin, LOW);
            // Print the nutrient values to the serial monitor
            Serial.print("Nutrient Values - Protein: ");
            Serial.print(protein);
            Serial.print(", Fiber: ");
            Serial.print(fiber);
            Serial.print(", Fats: ");
            Serial.print(fats);
            Serial.print(", Carbs: ");
            Serial.println(carbs);

            //delay(1000);
            Serial.print("Nutrient Values GLOBAL- Protein: ");
            Serial.print(globalProtein);
            Serial.print(", Fiber: ");
            Serial.print(globalFiber);
            Serial.print(", Fats: ");
            Serial.print(globalFats);
            Serial.print(", Carbs: ");
            Serial.println(globalCarbs);
        }if (status == "Delete") {
            substractGlobalIndexes(protein, fiber, fats, carbs);
            delay(500);
            digitalWrite(ingredientLedPin, LOW);
            // Print the nutrient values to the serial monitor
            Serial.print("Nutrient Values - Protein: ");
            Serial.print(protein);
            Serial.print(", Fiber: ");
            Serial.print(fiber);
            Serial.print(", Fats: ");
            Serial.print(fats);
            Serial.print(", Carbs: ");
            Serial.println(carbs);

            //delay(1000);
            Serial.print("Nutrient Values GLOBAL- Protein: ");
            Serial.print(globalProtein);
            Serial.print(", Fiber: ");
            Serial.print(globalFiber);
            Serial.print(", Fats: ");
            Serial.print(globalFats);
            Serial.print(", Carbs: ");
            Serial.println(globalCarbs);
        }
        else {
            return;
        }
    }



    Serial.println("");

    delay(4000);

}


void updateGlobalIndexes(float p, float f, float fa, float c) {
    globalProtein += p;
    globalFiber += f;
    globalFats += fa;
    globalCarbs += c;
    digitalWrite(addLedPin, LOW);
    digitalWrite(deleteLedPin, LOW);
    digitalWrite(ingredientLedPin, HIGH);
}

void substractGlobalIndexes(float p, float f, float fa, float c) {
    globalProtein -= p;
    globalFiber -= f;
    globalFats -= fa;
    globalCarbs -= c;
    digitalWrite(addLedPin, LOW);
    digitalWrite(deleteLedPin, LOW);
    digitalWrite(ingredientLedPin, HIGH);
}


int readBlock(int blockNumber, byte arrayAddress[])
{
    int largestModulo4Number = blockNumber / 4 * 4;
    int trailerBlock = largestModulo4Number + 3;//determine trailer block for the sector

    /*****************************************authentication of the desired block for access***********************************************************/
    byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed (read): ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 3;//return "3" as error message
    }



    /*****************************************reading a block***********************************************************/

    byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
    status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
    if (status != MFRC522::STATUS_OK) {
        Serial.print("MIFARE_read() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 4;//return "4" as error message
    }
    Serial.println("block was read");
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

void inflating(float protein, float fiber, float fats, float carbs) {
    Serial.print("bag %");
    Serial.println(bag1Inflation);
    float sum = protein + fiber + fats + carbs;
    proteinPercentage = (int)((protein * 100) / sum);
    fiberPercentage = (int)((fiber * 100) / sum);
    fatsPercentage = (int)((fats * 100) / sum);
    carbsPercentage = (int)((carbs * 100) / sum);
    Serial.println(proteinPercentage);
    Serial.println(fiberPercentage);
    Serial.println(fatsPercentage);
    Serial.println(carbsPercentage);

    int percentage = proteinPercentage;
    int percentage2 = fiberPercentage;
    int percentage3 = fatsPercentage;
    int percentage4 = carbsPercentage;

    // Check if the percentage is within the valid range
    if (percentage >= 0 && percentage <= 100) {
        // Bag 1
        int percForBag = 0;
        if (percentage >= 25) {
            percForBag = map(percentage, 25, 100, 50, 100);
        }
        else {
            percForBag = map(percentage, 0, 25, 0, 50);
        }
        //int percForBag = map(percentage, 0, 50, 0, 100);
        Serial.print("map %");
        Serial.println(percForBag);
        if (percentage >= 20 && percentage <= 30) {

            //fully deflate if is balanced
            bag1InflationDuration = (100) * DEFLATE_DURATION / 10;
            activateDeflation(bag1InflationDuration);
            bag1Inflation = 0;
            Serial.print("bag %");
            Serial.println(bag1Inflation);
        }
        else{
            if (percForBag >= bag1Inflation) {
                Serial.println("1");
                bag1InflationDuration = (percForBag - bag1Inflation) * INFLATE_DURATION / 10;
                activateInflation(bag1InflationDuration);
            }
            else {
                Serial.println("2");
                bag1InflationDuration = (bag1Inflation - percForBag) * DEFLATE_DURATION / 10;
                activateDeflation(bag1InflationDuration);

            }
            bag1Inflation = percForBag;
            Serial.print("bag %");
            Serial.println(bag1Inflation);
            }   
            
            
        
        // Store the entered percentage as the new inflation percentage
    }


    // Check if the percentage is within the valid range
    if (percentage2 >= 0 && percentage2 <= 100) {
        // Bag 1
        int percForBag = 0;
        if (percentage2 >= 13) {
            percForBag = map(percentage2, 13, 100, 50, 100);
        }
        else {
            percForBag = map(percentage2, 0, 13, 0, 50);
        }
        Serial.print("map %");
        Serial.println(percForBag);
        if (percentage2 >= 10 && percentage2 <= 15) {

            bag2InflationDuration = (100) * DEFLATE_DURATION2 / 10;
            activateDeflation2(bag2InflationDuration);
            bag2Inflation = 0;
            Serial.print("bag %");
            Serial.println(bag2Inflation);
        }else {
            if (percForBag >= bag2Inflation) {
                Serial.println("1");
                bag2InflationDuration = (percForBag - bag2Inflation) * INFLATE_DURATION2 / 10;
                activateInflation2(bag2InflationDuration);
            }
            else {
                Serial.println("2");
                bag2InflationDuration = (bag2Inflation - percForBag) * DEFLATE_DURATION2 / 10;
                activateDeflation2(bag2InflationDuration);

            }
            bag2Inflation = percForBag;
            Serial.print("bag %");
            Serial.println(bag2Inflation);
        }
        
        // Store the entered percentage as the new inflation percentage


    }

    // Check if the percentage is within the valid range
    if (percentage3 >= 0 && percentage3 <= 100) {
        // Bag 1
        int percForBag = 0;
        if (percentage3 >= 30) {
            percForBag = map(percentage3, 30, 100, 50, 100);
        }
        else {
            percForBag = map(percentage3, 0, 30, 0, 50);
        }
        Serial.print("map %");
        Serial.println(percForBag);
        if (percentage3 >= 25 && percentage3 <= 35) {


            bag3InflationDuration = (100) * DEFLATE_DURATION3 / 10;
            activateDeflation3(bag3InflationDuration);
            bag3Inflation = 0;
            Serial.print("bag %");
            Serial.println(bag3Inflation);
        }
        else {
            if (percForBag >= bag3Inflation) {
                Serial.println("1");
                bag3InflationDuration = (percForBag - bag3Inflation) * INFLATE_DURATION3 / 10;
                activateInflation3(bag3InflationDuration);
            }
            else {
                Serial.println("2");
                bag3InflationDuration = (bag3Inflation - percForBag) * DEFLATE_DURATION3 / 10;
                activateDeflation3(bag3InflationDuration);

            }
            bag3Inflation = percForBag;
            Serial.print("bag %");
            Serial.println(bag3Inflation);
        }
        // Store the entered percentage as the new inflation percentage


    }

    // Check if the percentage is within the valid range
    if (percentage4 >= 0 && percentage4 <= 100) {
        // Bag 1
        int percForBag = 0;
        if (percentage4 >= 55) {
            percForBag = map(percentage4, 55, 100, 50, 100);
        }
        else {
            percForBag = map(percentage4, 0, 55, 0, 50);
        }
        Serial.print("map %");
        Serial.println(percForBag);
        if (percentage4 >= 45 && percentage4 <= 65) {

            bag4InflationDuration = (100) * DEFLATE_DURATION4 / 10;
            activateDeflation4(bag4InflationDuration);
            bag4Inflation = 0;
            Serial.print("bag %");
            Serial.println(bag4Inflation);
        }
        else {
            if (percForBag >= bag4Inflation) {
                Serial.println("1");
                bag4InflationDuration = (percForBag - bag4Inflation) * INFLATE_DURATION4 / 10;
                activateInflation4(bag4InflationDuration);
            }
            else {
                Serial.println("2");
                bag4InflationDuration = (bag4Inflation - percForBag) * DEFLATE_DURATION4 / 10;
                activateDeflation4(bag4InflationDuration);

            }
            bag4Inflation = percForBag;
            Serial.print("bag %");
            Serial.println(bag4Inflation);
        }
        // Store the entered percentage as the new inflation percentage


    }


}

// Function to calculate the inflation percentage based on the inflation duration
float calculateInflationPercentage(unsigned long inflationDuration) {
    // Calculate the percentage based on the inflation duration and the known inflation/deflation durations
    float percentage = (inflationDuration / float(INFLATE_DURATION)) * 10.0;
    if (percentage > 100.0) {
        percentage = 100.0;
    }
    return percentage;
}

void activateInflation(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag1StartTime = millis();
    // Serial.println(bag1StartTime);
    digitalWrite(bag1InPin, HIGH);
    while (millis() - bag1StartTime < abs(timeToInflate)) {
        //analogWrite(bag1InPin, 255);
        Serial.println("Inflating bag 1");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag1InPin));
        // Serial.println(millis() - bag1StartTime);
    };
    digitalWrite(bag1InPin, LOW);
    Serial.println("Stop inflation bag 1");
}

void activateDeflation(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag1StartTime = millis();
    // Serial.println(bag1StartTime);
    digitalWrite(bag1OutPin, HIGH);
    while (millis() - bag1StartTime < abs(timeToInflate)) {
        Serial.println("Deflating bag 1");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag1OutPin));
        // Serial.println(millis() - bag1StartTime);
    };
    digitalWrite(bag1OutPin, LOW);
    Serial.println("stop deflation bag 1");
}

void activateInflation2(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag2StartTime = millis();
    //Serial.println(bag2StartTime);
    digitalWrite(bag2InPin, HIGH);
    while (millis() - bag2StartTime < abs(timeToInflate)) {
        Serial.println("Inflating bag 2");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag2InPin));
        // Serial.println(millis() - bag2StartTime);
    };
    digitalWrite(bag2InPin, LOW);
    Serial.println("Stop inflation bag 2");
}

void activateDeflation2(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag2StartTime = millis();
    // Serial.println(bag2StartTime);
    digitalWrite(bag2OutPin, HIGH);
    while (millis() - bag2StartTime < abs(timeToInflate)) {
        Serial.println("Deflating bag 2");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag2OutPin));
        // Serial.println(millis() - bag2StartTime);
    };
    digitalWrite(bag2OutPin, LOW);
    Serial.println("stop deflation bag 2");
}

void activateInflation3(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag3StartTime = millis();
    //Serial.println(bag3StartTime);
    digitalWrite(bag3InPin, HIGH);
    while (millis() - bag3StartTime < abs(timeToInflate)) {
        Serial.println("Inflating bag 3");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag3InPin));
        // Serial.println(millis() - bag3StartTime);
    };
    digitalWrite(bag3InPin, LOW);
    Serial.println("Stop inflation bag 3");
}

void activateDeflation3(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag3StartTime = millis();
    //Serial.println(bag3StartTime);
    digitalWrite(bag3OutPin, HIGH);
    while (millis() - bag3StartTime < abs(timeToInflate)) {
        Serial.println("Deflating bag 3");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag3OutPin));
        // Serial.println(millis() - bag3StartTime);
    };
    digitalWrite(bag3OutPin, LOW);
    Serial.println("stop deflation bag 3");
}

void activateInflation4(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag4StartTime = millis();
    //Serial.println(bag4StartTime);
    digitalWrite(bag4InPin, HIGH);
    while (millis() - bag4StartTime < abs(timeToInflate)) {
        Serial.println("Inflating bag 4");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag4InPin));
        // Serial.println(millis() - bag4StartTime);
    };
    digitalWrite(bag4InPin, LOW);
    Serial.println("Stop inflation bag 4");
}

void activateDeflation4(long timeToInflate) {
    // Serial.println("TIME");
    // Serial.println(timeToInflate);
    bag4StartTime = millis();
    //Serial.println(bag4StartTime);
    digitalWrite(bag4OutPin, HIGH);
    while (millis() - bag4StartTime < abs(timeToInflate)) {
        Serial.println("Deflating bag 4");
        // Serial.print("PWM Value: ");
        // Serial.println(analogRead(bag4OutPin));
        // Serial.println(millis() - bag4StartTime);
    };
    digitalWrite(bag4OutPin, LOW);
    Serial.println("stop deflation bag 4");
}


void activateInflationAll(long timeToInflate, long timeToInflate2, long timeToInflate3, long timeToInflate4) {
    Serial.println("TIME");
    Serial.println(timeToInflate);
    Serial.println(timeToInflate2);
    Serial.println(timeToInflate3);
    Serial.println(timeToInflate4);

    bag1StartTime = millis();
    bag2StartTime = millis();
    bag3StartTime = millis();
    bag4StartTime = millis();

    digitalWrite(bag1InPin, HIGH);
    digitalWrite(bag2InPin, HIGH);
    digitalWrite(bag3InPin, HIGH);
    digitalWrite(bag4InPin, HIGH);

    while ((millis() - bag1StartTime < abs(timeToInflate)) ||
        (millis() - bag2StartTime < abs(timeToInflate2)) ||
        (millis() - bag3StartTime < abs(timeToInflate3)) ||
        (millis() - bag4StartTime < abs(timeToInflate4))) {
        if (millis() - bag1StartTime < abs(timeToInflate)) {
            Serial.println("Inflating bag 1");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag1InPin));
            Serial.println(millis() - bag1StartTime);
        }
        else {
            digitalWrite(bag1InPin, LOW);
        }

        if (millis() - bag2StartTime < abs(timeToInflate2)) {
            Serial.println("Inflating bag 2");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag2InPin));
            Serial.println(millis() - bag2StartTime);
        }
        else {
            digitalWrite(bag2InPin, LOW);
        }

        if (millis() - bag3StartTime < abs(timeToInflate3)) {
            Serial.println("Inflating bag 3");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag3InPin));
            Serial.println(millis() - bag3StartTime);
        }
        else {
            digitalWrite(bag3InPin, LOW);
        }

        if (millis() - bag4StartTime < abs(timeToInflate4)) {
            Serial.println("Inflating bag 4");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag4InPin));
            Serial.println(millis() - bag4StartTime);
        }
        else {
            digitalWrite(bag4InPin, LOW);
        }
    }

    digitalWrite(bag1InPin, LOW);
    digitalWrite(bag2InPin, LOW);
    digitalWrite(bag3InPin, LOW);
    digitalWrite(bag4InPin, LOW);

    Serial.println("Stop inflation bags");
}

void activateDeflationAll(long timeToDeflate, long timeToDeflate2, long timeToDeflate3, long timeToDeflate4) {
    Serial.println("TIME");
    Serial.println(timeToDeflate);
    Serial.println(timeToDeflate2);
    Serial.println(timeToDeflate3);
    Serial.println(timeToDeflate4);

    bag1StartTime = millis();
    bag2StartTime = millis();
    bag3StartTime = millis();
    bag4StartTime = millis();

    digitalWrite(bag1OutPin, HIGH);
    digitalWrite(bag2OutPin, HIGH);
    digitalWrite(bag3OutPin, HIGH);
    digitalWrite(bag4OutPin, HIGH);

    while ((millis() - bag1StartTime < abs(timeToDeflate)) ||
        (millis() - bag2StartTime < abs(timeToDeflate2)) ||
        (millis() - bag3StartTime < abs(timeToDeflate3)) ||
        (millis() - bag4StartTime < abs(timeToDeflate4))) {
        if (millis() - bag1StartTime < abs(timeToDeflate)) {
            Serial.println("Deflating bag 1");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag1OutPin));
            Serial.println(millis() - bag1StartTime);
        }

        if (millis() - bag2StartTime < abs(timeToDeflate2)) {
            Serial.println("Deflating bag 2");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag2OutPin));
            Serial.println(millis() - bag2StartTime);
        }

        if (millis() - bag3StartTime < abs(timeToDeflate3)) {
            Serial.println("Deflating bag 3");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag3OutPin));
            Serial.println(millis() - bag3StartTime);
        }

        if (millis() - bag4StartTime < abs(timeToDeflate4)) {
            Serial.println("Deflating bag 4");
            Serial.print("PWM Value: ");
            Serial.println(analogRead(bag4OutPin));
            Serial.println(millis() - bag4StartTime);
        }
    }

    digitalWrite(bag1OutPin, LOW);
    digitalWrite(bag2OutPin, LOW);
    digitalWrite(bag3OutPin, LOW);
    digitalWrite(bag4OutPin, LOW);

    Serial.println("Stop deflation bags");
}

