#ifndef _ALARM_H_
#define _ALARM_H_

class Alarm{
public:
   Alarm()
   {
      interval = 1.0f;
      timeLeft = interval;
      ringing = false;
   }
   void  update()
   {
      timeLeft -= TickSec;
      if( timeLeft<=0 ){
         timeLeft += interval;
         ringing = true;
      }else{
         ringing = false;
      }
   }
   void  restart()
   {
      timeLeft = interval;
      ringing = false;
   }
   bool  isRinging(){return ringing;}
   void  setInterval(F32 _interval, bool randomize=false)
   {
      AssertFatal(_interval>=TickSec,"Alarm period to small. Must be greater than time of tick");
      interval = _interval;
      if( randomize ||  timeLeft>interval ){
         timeLeft = gRandGen.randF(0.0f, interval);
      }
   }
   F32   getInterval()
   {
      return interval;
   }
private:
   F32   interval;
   F32   timeLeft;
   bool  ringing;
};
#endif