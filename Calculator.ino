#include "Modes.h"   
// Include level: Calculator.ino -> Modes.h -> Input.h -> ExprCalc.h -> Screen.h

void setup(){
    Serial.begin(9600);
    pinMode(shiftPin, INPUT_PULLUP);
    
    lcd.init();
    lcd.backlight();
    lcd.print(F("  CASINO CALC"));
    lcd.setCursor(0, 1);
    lcd.print(F(" (c) NES HCMUS"));
    delay(1500);
    Serial.println(F("[STAT] Calculator ready"));
}

// main func
void loop(){
    // Default: menu mode
    menuMode();

    // The program shouldn't exit to here. If so, probably there was a bug
    lcd.clear();
    lcd.print(F("An error occured"));
    lcd.setCursor(0, 1);
    lcd.print(F("Please RESET"));
}
