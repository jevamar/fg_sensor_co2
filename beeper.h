
void playMelody()
{
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIN_BEEPER, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN_BEEPER);
  }
}

void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds)
{ 
    digitalWrite(LED_BUILTIN, HIGH);   
    //use led to visualize the notes being played
    
    int x;   
    long delayAmount = (long)(1000000/frequencyInHertz);
    long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
    for (x=0;x<loopTime;x++)   
    {    
        digitalWrite(speakerPin,HIGH);
        delayMicroseconds(delayAmount);
        digitalWrite(speakerPin,LOW);
        delayMicroseconds(delayAmount);
    }    
    
    digitalWrite(LED_BUILTIN, LOW);
    //set led back to low
    
    delay(20);
    //a little delay to make all notes sound separate
} 

void march()
{    
    //for the sheet music see:
    //http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0016254
    //this is just a translation of said sheet music to frequencies / time in ms
    //used 500 ms for a quart note
    
    beep(PIN_BEEPER, a, 500); 
    beep(PIN_BEEPER, a, 500);     
    beep(PIN_BEEPER, a, 500); 
    beep(PIN_BEEPER, f, 350); 
    beep(PIN_BEEPER, cH, 150);
    
    beep(PIN_BEEPER, a, 500);
    beep(PIN_BEEPER, f, 350);
    beep(PIN_BEEPER, cH, 150);
    beep(PIN_BEEPER, a, 1000);
    //first bit
    
    beep(PIN_BEEPER, eH, 500);
    beep(PIN_BEEPER, eH, 500);
    beep(PIN_BEEPER, eH, 500);    
    beep(PIN_BEEPER, fH, 350); 
    beep(PIN_BEEPER, cH, 150);
    
    beep(PIN_BEEPER, gS, 500);
    beep(PIN_BEEPER, f, 350);
    beep(PIN_BEEPER, cH, 150);
    beep(PIN_BEEPER, a, 1000);
    //second bit...
    
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, a, 350); 
    beep(PIN_BEEPER, a, 150);
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, gSH, 250); 
    beep(PIN_BEEPER, gH, 250);
    
    beep(PIN_BEEPER, fSH, 125);
    beep(PIN_BEEPER, fH, 125);    
    beep(PIN_BEEPER, fSH, 250);
    delay(250);
    beep(PIN_BEEPER, aS, 250);    
    beep(PIN_BEEPER, dSH, 500);  
    beep(PIN_BEEPER, dH, 250);  
    beep(PIN_BEEPER, cSH, 250);  
    //start of the interesting bit
    
    beep(PIN_BEEPER, cH, 125);  
    beep(PIN_BEEPER, b, 125);  
    beep(PIN_BEEPER, cH, 250);      
    delay(250);
    beep(PIN_BEEPER, f, 125);  
    beep(PIN_BEEPER, gS, 500);  
    beep(PIN_BEEPER, f, 375);  
    beep(PIN_BEEPER, a, 125); 
    
    beep(PIN_BEEPER, cH, 500); 
    beep(PIN_BEEPER, a, 375);  
    beep(PIN_BEEPER, cH, 125); 
    beep(PIN_BEEPER, eH, 1000); 
    //more interesting stuff (this doesn't quite get it right somehow)
    
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, a, 350); 
    beep(PIN_BEEPER, a, 150);
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, gSH, 250); 
    beep(PIN_BEEPER, gH, 250);
    
    beep(PIN_BEEPER, fSH, 125);
    beep(PIN_BEEPER, fH, 125);    
    beep(PIN_BEEPER, fSH, 250);
    delay(250);
    beep(PIN_BEEPER, aS, 250);    
    beep(PIN_BEEPER, dSH, 500);  
    beep(PIN_BEEPER, dH, 250);  
    beep(PIN_BEEPER, cSH, 250);  
    //repeat... repeat
    
    beep(PIN_BEEPER, cH, 125);  
    beep(PIN_BEEPER, b, 125);  
    beep(PIN_BEEPER, cH, 250);      
    delay(250);
    beep(PIN_BEEPER, f, 250);  
    beep(PIN_BEEPER, gS, 500);  
    beep(PIN_BEEPER, f, 375);  
    beep(PIN_BEEPER, cH, 125); 
           
    beep(PIN_BEEPER, a, 500);            
    beep(PIN_BEEPER, f, 375);            
    beep(PIN_BEEPER, c, 125);            
    beep(PIN_BEEPER, a, 1000);       
    //and we're done \ó/    
}
