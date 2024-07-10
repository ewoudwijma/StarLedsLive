external void show();
external CRGB hsv(int h,int s,int v); 
external CRGB *leds; 
external void clear(); 
define LED_WIDTH 32
define LED_HEIGHT 32
define panel_width 16

define NB_PANEL_WIDTH 1
define NUM_STRIPS 1
define NUM_LEDS_PER_STRIP 256
external uint16_t *pos;
external void map();
external void initleds(uint32_t *pins,int num,int num_leds_per_strip);
 uint32_t pins[NUM_STRIPS]={21,19,22,23};

void mapfunction()
{
  
  int panelnumber = pos[0] / 256;
  int datainpanel = pos[0] % 256;
  int Y = panelnumber / NB_PANEL_WIDTH;
  int X = panelnumber % NB_PANEL_WIDTH;
  //int Y = yp;
  //int X = Xp;

  int x = datainpanel % 16;
  int y = datainpanel / 16;

  if (y % 2 == 0)
  {
    Y = Y * 16 + y;
    X = X * 16 + x;
  }
  else
  {
    Y = Y * 16 + y;
    X = X * 16 + 16 - x - 1;
  }

 pos[0]= Y * 16 * NB_PANEL_WIDTH  + X;

}

void main()
{

 initleds(pins,NUM_STRIPS,NUM_LEDS_PER_STRIP);
    map();
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
    leds[h%(LED_HEIGHT*LED_WIDTH)]=CRGB(0255,255,255);
    show();
    h++;
  }
}