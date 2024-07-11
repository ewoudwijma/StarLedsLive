void main()
{
  int h=1;
  while(h>0)
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i]= hsv(i+h,255,255);
    }
    show();
    h++;
  }
}