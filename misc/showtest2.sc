external void show();
external CRGB *leds;
external CRGB hsv(int h,int s,int v); 

void main()
{
  int h=1;
  while(0==0)
  {
    for(int i=0;i<256;i++)
    {
      leds[i]= hsv(i+h,255,255);
    }
    show();
    h++;
  }
}