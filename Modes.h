#include "Input.h"

// Function prototypes
bool modeKeyCheck (String key); 
void eqnMode();
void compMode ();
void menuMode();
void aboutMode();
void integralMode();

int currentDisplayingChars = 0;
int scrollIndex = 0;

void menuMode() {
    bool printMenu = true; // to fix flickering
    
    String key;
    while(true) {
        menuflag = false;
        errflag = false; //reset
        
        key = getKeyStr();
        if (printMenu) {
            lcd.clear(); lcd.noBlink();
            lcd.print("1.COMP     2.EQN");
            lcd.setCursor(0, 1);
            lcd.print("3.INTGRL 4.ABOUT");
            printMenu = false;
        }
        
        if (key == "1") {
            compMode();
            printMenu = true;
        }
        else if (key == "2") {
            eqnMode();  
            printMenu = true;
        }
        else if (key == "3") {
            integralMode();
            printMenu = true;
        }
        else if (key == "4") {
            aboutMode();
            printMenu = true;
        }
        
    }
}

void aboutMode() {
    Serial.println("\n[STAT] About mode");
    while (true) {
        lcd.clear();
        lcd.print(F("NES  HCMUS  2020"));
        lcd.setCursor(0, 1);
        lcd.print(F("Ngoc Phat  K19IT"));
        delay(4000);
        lcd.clear();
        lcd.print(F("Github link:..."));
        delay(2000);
        lcd.clear();
        lcd.print(F("hungngocphat01/arduino-calculator"));
        delay(2000);
        for (int i = 0; i < 20; i++) {
            lcd.scrollDisplayLeft();
            delay(500);
        }
        lcd.clear();
        lcd.print(F("RESET to quit"));
        delay(2000);
    }
}

void printSolution(String msg, float value) {
    lcd.clear();
    lcd.print(msg);
    lcd.setCursor(0, 1);
    lcd.print(value, 6);

    pressAnyKey();
    lcd.clear();
}

void integralMode() {
    Serial.println(F("\n[STAT] Integral mode"));

    allowxkey = true;
    prntline = 1;

    lcd.clear();
    lcd.print("Intgrl expr:");
    lcd.setCursor(0, prntline);
    
    String tokens[max_size];
    int ti;
    scanTokens(tokens, ti);

    sprintMemoryUsage(F("Free mem after scanning: "));

    if (menuflag) return;
    if (errflag) {
        pressAnyKey();
        return;
    }

    float a = scanCoefficient("a=");
    float b = scanCoefficient("b=");

    sprintMemoryUsage(F("Free mem after scanning a, b: "));
    
    const int ranges = 10;

    // Middle Riemann sum
    float rsum = 0;
    float dx = abs(b - a)/ranges;
    int negative = 1;

    if (b < a) {
        float t = b;
        b = a;
        a = b;
        negative = -1;
    }

    for (int i = 0; i < ranges; i++) {
        X = (2 * a + dx * (2 * i + 1)) / 2;
        rsum += evalExpression(tokens, ti);

        if (errflag) {
            errflag = false;
            pressAnyKey();
            return;
        }
    }

    rsum *= dx * negative;
    printSolution("Result:", rsum);
}

void eqnMode() {
    Serial.println(F("\n[STAT] EQN mode"));
    
    prntline = 1;
    allowxkey = false;
    
    lcd.noBlink();
    String key;
        
    lcd.clear();
    lcd.print(F("1.ax^2+bx+c=0"));
    lcd.setCursor(0, 1);
    lcd.print(F("2.ax+by=c"));

    do {
        key = getKeyStr();
        delay(50);        
    } while (key != "1" && key != "2" && key != "MODE");
    
    if (modeKeyCheck(key)) return;
    
    // ax^2 + bx + c = 0
    if (key == "1") {
        Serial.println(F("[EQN] ax^2+bx+c=0"));
        lcd.clear();
        lcd.blink();
        float a = scanCoefficient("a=");
        float b = scanCoefficient("b=");
        float c = scanCoefficient("c=");
        lcd.noBlink();

        float delta = b*b-4*a*c;
        sprintVariable<float>(F("[OUTP] delta="), delta);

        if (delta == 0) {
            float x = -b/(2*a);
            
            sprintVariable<float>(F("[OUTP] x="), x);
            printSolution("x=", x);
        }
        else if (delta > 0) {
            float x1 = (-b-sqrt(delta))/(2*a);
            float x2 = (-b+sqrt(delta))/(2*a); 

            sprintVariable<float>(F("[OUTP] x1="), x1);
            sprintVariable<float>(F("[OUTP] x2="), x2);
            
            printSolution("x1=", x1);
            printSolution("x2=", x2);
        }
        else if (delta < 0) {
            Serial.println(F("[OUTP] No solution"));
            
            lcd.clear();
            lcd.print("No solution");
            pressAnyKey();
        }
    }
    else if (key == "2") {
        Serial.println(F("[EQN] ax+by=c"));
        lcd.clear();
        lcd.blink();

        float a1 = scanCoefficient(F("[a1x+b1y=c1] a1="));
        float b1 = scanCoefficient(F("[a1x+b1y=c1] b1="));
        float c1 = scanCoefficient(F("[a1x+b1y=c1] c1="));
        float a2 = scanCoefficient(F("[a2x+b2y=c2] a2="));
        float b2 = scanCoefficient(F("[a2x+b2y=c2] b2="));
        float c2 = scanCoefficient(F("[a2x+b2y=c2] c2="));

        float d = a1*b2 - a2*b1;
        float dx = c1*b2 - c2*b1;
        float dy = a1*c2 - a2*c1;

        sprintVariable<float>(F("[OUTP] d="), d);
        sprintVariable<float>(F("[OUTP] dx="), dx);
        sprintVariable<float>(F("[OUTP] dy="), dy);

        if (d == 0 && (dx != 0 || dy != 0)) {
            Serial.println(F("[OUTP] No solution"));
            
            lcd.clear();
            lcd.print(F("No solution"));          
            pressAnyKey();
        }
        else if (d == 0 && dx == 0 && dy == 0) {
            Serial.println("[OUTP] Infnt solutions");
            
            lcd.clear();
            lcd.print(F("Infnt solutions"));
            pressAnyKey();
        }
        else {
            float x = dx/d;
            float y = dy/d;
            
            sprintVariable<float>(F("[OUTP] x="), x);
            sprintVariable<float>(F("[OUTP] y="), y);
            
            printSolution("x=", x);
            printSolution("y=", y);            
        }
    }
}

void compMode () {
    Serial.println(F("\n[STAT] COMP mode"));
    lcd.clear();
    float result = 0;

    prntline = 0;
    allowxkey = false;
        
    do {
        sprintMemoryUsage(F("Free mem before input: "));
        lcd.blink();
        result = scanAndEvalExpression();
        if (!menuflag) {
            lcd.noBlink();
            printSolution("Result:", result);
        }
    } while (!menuflag);
}
