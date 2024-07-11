//import memset
//save_reg
define PI 3.1415926535
define speed 1
define nb_branches 5
uint8_t C_X ;
uint8_t C_Y;
uint8_t mapp;
uint8_t rMapRadius[NUM_LEDS];
uint8_t rMapAngle[NUM_LEDS];

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

void init()
{
  C_X = width / 2;
  C_Y = height / 2;
  mapp = 255 / width;
  for (int x = -C_X; x < C_X + (width % 2); x++) {
    for (int y = -C_Y; y < C_Y + (height % 2); y++) {

      float h=128*(atan2(y, x)/PI);
      rMapAngle[(x + C_X) *height+y + C_Y]= (int)(h);
      h=hypot(x,y)*mapp;
      rMapRadius[(x + C_X)*height +y + C_Y] = (int)(h); //thanks Sutaburosu
    }
  }
}


void main() {
  initleds(pins,NUM_STRIPS,NUM_LEDS_PER_STRIP);
  map();
  resetStat();
  init();

  uint32_t t;
  //t = speed;
  while(2>1)
  {
    // memset(leds,0,4096*3);
    for (uint8_t x = 0; x < width; x++) {
      for (uint8_t y = 0; y < height; y++) {
        uint8_t angle = rMapAngle[x*height+y];
        uint8_t radius = rMapRadius[x*height+y];

        leds[y*panel_width+x] = hsv(t + radius, 255, sin8(t*4+sin8(t * 4 - radius)+angle*nb_branches));
        //int h=sin8(t*4+sin8(t * 4 - radius)+angle*nb_branches);
        // leds[y*panel_width+x] = hsv(t + radius, 255, h);
      }
    }
    show();
    t=t+speed;
}
  //delay(16);
}