
package edu.uw.ubicomplab.accelapp;

        import android.util.Log;

        import org.apache.commons.math3.stat.descriptive.DescriptiveStatistics;


public class StepCounter {
    private int windowSize = 15;
    private DescriptiveStatistics accelWindow = new DescriptiveStatistics(windowSize);
    public int stepCount = 0;

    public float oldTimeStamp = 0;
    public double thresholdLimit = 0.5; //value over which the signal could be a step
    public float timeDebounce = 8; // time between 2 signals for it to be different steps 5

    public void signalProcessing(float time, float deltaAccel) {

        // Check threshold of the signal
        double absThreshAccel = deltaAccel - thresholdLimit;
        accelWindow.addValue(absThreshAccel);
        //Maximum value in the window
        double maxAbsThrAccel = accelWindow.getMax();
        if (maxAbsThrAccel > 0){
            if ((time - oldTimeStamp)>timeDebounce){
                stepCount += 1;
                Log.i("[step]",  "getting a step");
            }
            oldTimeStamp = time;
        }

    }


    public void reset() {
        stepCount = 0;
    }
}




