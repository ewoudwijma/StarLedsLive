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
    for(int i=0;i<width;i++)
    {
      for(int j=0;j<height;j++)
      {
        leds[i+j*panel_width]= hsv(i+j+h,255,255);
      }
    }
    leds[h%(height*width)]=CRGB(0255,255,255);
    show();
    h++;
  }
}