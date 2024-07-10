import rand
external CRGB *leds;
external void show();
external void clear();
external CRGB hsv(uint8_t h, uint8_t s, uint8_t v);
external void resetStat();
define width 16
define height 16
define panel_width 16 

char copy[1024];  //be careful to change this
uint8_t line1[width];
uint8_t linec[width];

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

void create_leds(int o)
{

   for (int j = 1; j < height - 1; j++)
   {
      for (int h = 0; h < width; h++)
      {

       
         line1[h] = linec[h];

         //tmp = copy[(j)*width + h];
        
         linec[h] = copy[(j)*width + h];

      }
      for (int i = 1; i < width - 1; i++)
      {
       
   int res = 0;

   res = linec[i - 1] + linec[i + 1];
   res = res + line1[i - 1] + line1[i + 1] + line1[i];
   int _pos = (j + 1) * width + i;

   res = res + copy[_pos - 1] + copy[_pos + 1] + copy[_pos];

         int po = 0;
         int gh = width * j + i;
         int l = copy[gh];
         int gh2 = j * panel_width + i;
if(res==3)
{
int hqs=0;
po=1;
}
else
{
   
         if (l == 1)
         {
            
            if (res == 2)
            {
               po = 1;            
         }
          }
}
         if (po == 1)
         {
            leds[gh2] = hsv(o,255,255);
            copy[gh] = 1;
         }
         else
         {
            leds[gh2] = CRGB(0, 0, 0);
            copy[gh] = 0;
         }
      }
   }

}


int main()
{
resetStat();

  initleds(pins,NUM_STRIPS,NUM_LEDS_PER_STRIP);
  map();
   /*
  for (int h = 0; h < height; h++)
  {
     int _pos = h * width + width / 2;
    copy[_pos] = 1;

  }
  */
   for (int h = 0; h < width * height; h++)
   {

      copy[h]=rand(2);
   }
   clear();

   show();
   for (int i = 0; i < 501; i++)
   {

     create_leds(i);
     show();

   }
}