external void show();
external CRGB hsv(int h,int s,int v); 
external CRGB *leds; 
external void clear(); 
define LED_WIDTH 16
define LED_HEIGHT 16
define panel_width 16

void main()
{
  int h=1;
  clear();
  while(h>0)
  {
    for(int i=0;i<LED_WIDTH;i++)
    {
      for(int j=0;j<LED_HEIGHT;j++)
      {
        leds[i+j*panel_width]= hsv(i+j+h,255,255);
      }
    }
    show();
    h++;
  }
}