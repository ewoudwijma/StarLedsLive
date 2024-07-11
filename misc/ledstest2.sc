
void main()
{
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
    show();
    h++;
  }
}