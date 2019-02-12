-----------------------------------------------------------------------------------------
EE 590 Ubiquitous Computing(Spring 2018)
Assignment 2: Gesture Recognition using an Android Phone
Anusha Kamat(anusha7@uw.edu)
-----------------------------------------------------------------------------------------

Assignment Submission : It is uploaded as a zip file on Canvas

The assignment is to train the phone to recognise 3 gestures and when tested , 
predict which one of these gestures is made.
I have done changes only to the data section of the code provided by the Professor

The three gestures I have considered are :
Flip, Infinity, W

While choosing the data to separate these 3 gestures , I observed that during 
Flip gesture, there is a lot of variance in x axis of gyrometer
Infinity Gesture, there is a lot of variance in x axis of accelerometer
W Gesture , there is a variance in y axis of accelerometer.
Hence I picked these signals to differentiate between these gestures.
-----------------------------------------------------------------------------------------
