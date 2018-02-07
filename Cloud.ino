#include <Adafruit_NeoPixel.h>
#include <math.h>

enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, MUSIC };

enum  direction { FORWARD, REVERSE };


class NeoPatterns : public Adafruit_NeoPixel
{

    #define N_PIXELS  150 
    #define MIC_PIN   A1 
    #define LED_PIN    6  
    #define SAMPLE_WINDOW   10  
    #define PEAK_HANG 0 
    #define PEAK_FALL 1 
    #define INPUT_FLOOR 30 
    #define INPUT_CEILING 150 
   
    public:

    // Member Variables:  
    pattern  ActivePattern;  
    direction Direction;     
     
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate;
    
    uint32_t Color1, Color2;  
    uint16_t TotalSteps;  
    uint16_t Index; 
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;
                case MUSIC:
                    MusicUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else 
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }

     void RainbowIncrement()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= 255)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }
    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }
      
     void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }
    
 
    void RainbowCycleUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        RainbowIncrement();
    }

   
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
   }
    

    void TheaterChaseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }

 
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color;
        Index = 0;
        Direction = dir;
    }
    

    void ColorWipeUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }
    

    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
    }


    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }
    
  
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }
    
    void FadeUpdate()
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
        
        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }
   

    uint32_t DimColor(uint32_t color)
    {
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }


    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }

    //Red component
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }

    //Green component
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }

    // Blue component
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }

void Music(){
        ActivePattern = MUSIC;
    
        Index = 0;
}

void MusicUpdate(){

  byte peak = 16;     
  unsigned int sample;
 
  byte dotCount = 0;  
  byte dotHangCount = 0; 

  unsigned long startMillis= millis();  
  float peakToPeak = 0;   
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;

  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    if (sample < 1024) 
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  
  c = fscale(INPUT_FLOOR, INPUT_CEILING, 0, 250, peakToPeak, 2);
  
  for (int i=0; i<=numPixels()-1; i++){
    setPixelColor(i,Wheel(map(c,INPUT_FLOOR,INPUT_CEILING,120,250)));
  }
  
  show();
  Increment();
 
}

void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    setPixelColor(i, c);
  }
}
 
 

float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){
 
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
 
 
  // condition curve parameter
  // limit range
 
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;
 
  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
 
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
 
  // Zero Refference the values
  OriginalRange = originalMax - originalMin;
 
  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }
 
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
 
  if (originalMin > originalMax ) {
    return 0;
  }
 
  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;
 
  }
  else   
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }
 
  return rangedValue;
}


    
    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }
};

void StickComplete();

NeoPatterns Stick(150, 6, NEO_GRB + NEO_KHZ800, &StickComplete);

// Initialize everything and prepare to start
void setup()
{
  //analogReference(EXTERNAL);
  Serial.begin(9600);
  Stick.begin();
    
    //Default pattern after power loss
    Stick.Scanner(Stick.Color(255,0,0), 55);
}

// Main loop
void loop()
{
  
    if(Serial.available()){
      int mode = (Serial.read()-'0');
      switch(mode){
        case 1:
              Stick.Interval=20;
              Stick.ActivePattern= RAINBOW_CYCLE;
              break;
        case 2:
              Stick.Interval=55;
              Stick.ActivePattern= THEATER_CHASE;
              break;
        case 3:
              Stick.Color1 = Stick.Wheel(180);
              Stick.ActivePattern= COLOR_WIPE;
              break; // Probably going to have to use these cases to also update color variable
        case 4:
              Stick.Interval=55;
              Stick.ActivePattern= SCANNER;
              break;
        case 5:
              Stick.Interval=30;
              Stick.ActivePattern= FADE;
              break;
        case 6: 
              Stick.ActivePattern= NONE;
              break;
        case 7:
              Stick.ActivePattern=MUSIC;
              break;
              
      }
    }
    
  Stick.Update();
}

// Stick Completion Callback
void StickComplete()
{

}
