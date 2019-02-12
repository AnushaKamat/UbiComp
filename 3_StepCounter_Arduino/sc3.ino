
#include <Adafruit_CircuitPlayground.h>


#define N 15 //number of samples in sliding window
#define numofLEDs 5 //number of LEDs to be blinked
#define arr_len(a) (sizeof(a)/sizeof(*a))

float slidingWindow[N]={0} ; 
float thresholdlimit =0.3; //to check level crossing
float timeDebounce =300; //around 0.3s for one step
float Xdiff,Xdelta,Xnew, oldAccelVal = 0;
float maxaccel =0;

unsigned long oldTs,ts = 0;
int stepCount = 0;

void setup() {
  
  Serial.begin(9600); // Start com port 
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels(); //clear all pixels
  
}

void loop() {
 
    Xnew = CircuitPlayground.motionX();
    //X = Xnew-oldAccelVal;
    ts = millis();
   // oldAccelVal = Xnew;
    
    Serial.print(" X: ");
    Serial.print(Xnew);
    Serial.print(" Timestamp: ");
    Serial.println(ts);
    
    signalprocessing(Xnew,ts);
    
    Serial.print("StepCount :");
    Serial.println(stepCount);
    
    GlowLEDs(stepCount);
}

//Signal Processing block - Peak Detection/Sliding Window/Threshold crossing/time Debounce 

void signalprocessing(float Xtemp, unsigned long T){
  Xdelta = Xtemp - oldAccelVal; //change in Accel
  oldAccelVal = Xtemp;
 
  float absaccel = Xdelta - thresholdlimit; //threshold crossing
  
  addToWindow(absaccel);
  maxaccel = getMax();
  
  if(maxaccel >0.2)  // array of maximas crossing threshold
  {
    if ((T -oldTs)>timeDebounce) //Time Debounce
    {
      stepCount+=1;
    }
    oldTs = T;
  }
  
}

//Function to add new entry inthe Sliding Window

void addToWindow(float accelMag){
  for(int i = 1;i<arr_len(slidingWindow);i++){
    slidingWindow[i-1] = slidingWindow[i];
  }
  slidingWindow[arr_len(slidingWindow)-1] = accelMag;
}

//Function to get maximum out of the current values in the sliding Window

float getMax (){
  float max_temp = slidingWindow[0];
  for(int i=1;i<N;i++){
    if(slidingWindow[i]>max_temp){
     max_temp =slidingWindow[i];
    }
  }
  return max_temp;
}

//Function to convert steps into binary and display on array of 5 LEDs

void GlowLEDs(int numsteps){
 
    for (int i=0; i<numofLEDs; i++) {
    if (numsteps%2)
      CircuitPlayground.setPixelColor(i, 255,   0,   0); //ON or'1'
    else
      CircuitPlayground.setPixelColor(i, 0,   0,   0); //OFF or '0'
    numsteps/=2;
  }  
}
