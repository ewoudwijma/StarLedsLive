/*
   @title     StarBase
   @file      UserModLive.h
   @date      20240411
   @repo      https://github.com/ewowi/StarBase, submit changes to this file as PRs to ewowi/StarBase
   @Authors   https://github.com/ewowi/StarBase/commits/main
   @Copyright © 2024 Github StarBase Commit Authors, asmParser © https://github.com/hpwit/ASMParser
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
*/


// #define __RUN_CORE 0

#define __HARDWARE_MAP
#define __NON_HEAP

#include "FastLED.h"
#include "I2SClocklessLedDriver.h"
#include "parser.h"

#define NUM_LEDS_PER_STRIP 256
#define NUM_STRIPS 1
#define NUM_LEDS_MAX 3072

CRGB leds[NUM_LEDS_MAX];
uint16_t _maping[NUM_LEDS_MAX];

int pins[NUM_STRIPS] = {2};

I2SClocklessLedDriver driver;

static void clearleds()
{
    memset(leds, 0, NUM_LEDS_MAX * 3);
}

long time1;
long time4;
static float _min = 9999;
static float _max = 0;
static uint32_t _nb_stat = 0;
static float _totfps;
static float fps = 0; //integer?
static unsigned long frameCounter = 0;
static void show()
{
  frameCounter++;
    
  // SKIPPED: check nargs (must be 3 because arg[0] is self)
  long time2 = ESP.getCycleCount();

  driver.showPixels(WAIT);
  long time3 = ESP.getCycleCount();
  float k = (float)(time2 - time1) / 240000000;
  fps = 1 / k;
  float k2 = (float)(time3 - time2) / 240000000;
  float fps2 = 1 / k2;
  float k3 = (float)(time2 - time4) / 240000000;
  float fps3 = 1 / k3;
  _nb_stat++;
  if (_min > fps && fps > 10 && _nb_stat > 10)
    _min = fps;
  if (_max < fps && fps < 5000 && _nb_stat > 10)
    _max = fps;
  if (_nb_stat > 10)
    _totfps += fps;
  if (_nb_stat%1000 == 0)
    //Serial.printf("current show fps:%.2f\tglobal fps:%.2f\tfps animation:%.2f\taverage:%.2f\tmin:%.2f\tmax:%.2f\r\n", fps2, fps3, fps, _totfps / (_nb_stat - 10), _min, _max);
    ppf("current show fps:%.2f\tglobal fps:%.2f\tfps animation:%.2f  average:%.2f min:%.2f max:%.2f\r\n",fps2, fps3,  fps, _totfps / (_nb_stat - 10), _min, _max);
  time1 = ESP.getCycleCount();
  time4 = time2;

  // SKIPPED: check that both v1 and v2 are int numbers
  // RETURN_VALUE(VALUE_FROM_INT(0), rindex);
}

static void resetShowStats()
{
    float min = 999;
    float max = 0;
    _nb_stat = 0;
    _totfps = 0;
}

uint16_t map2;

uint16_t mapfunction(uint16_t pos)
{
  map2=pos;
  SCExecutable.execute("mapfunction");
  return map2;
}

static void __initleds(int *pins,int numstrip,int num_leds_per_strip)
{
  driver.num_led_per_strip = num_leds_per_strip;  
  driver.num_strips=numstrip;  
  for(int i=0;i<numstrip;i++)
  {
    driver.stripSize[i]=num_leds_per_strip;
  }
  driver.setPins(pins);
}

//external implementations
static CRGB POSV(uint8_t h, uint8_t s, uint8_t v) {return CHSV(h, s, v);}
void __map() {driver.createhardwareMap();}
static uint8_t _sin8(uint8_t a) {return sin8(a);}
static float _hypot(float x,float y) {return hypot(x,y);}
static float _atan2(float x,float y) { return atan2(x,y);}
static void dispshit(int g) { ppf("coming from assembly int %x %d", g, g);}
static float _sin(float j) {return sin(j);}
static void __print(char *s) {ppf("from assembly :%s\r\n", s);}
static void showError(int line, uint32_t size, uint32_t got) { ppf("Overflow error line %d max size: %d got %d", line, size, got);}
static void displayfloat(float j) {ppf("display float %f", j);}

string ScScript;

class UserModLive:public SysModule {

public:

  Parser p = Parser();

  UserModLive() :SysModule("Live") {
    isEnabled = false; //need to enable after fresh setup
  };

  void setup() override {
    SysModule::setup();

    parentVar = ui->initUserMod(parentVar, name, 6310);

    ui->initSelect(parentVar, "script", UINT16_MAX, false ,[this](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onUI: {
        // ui->setComment(var, "Fixture to display effect on");
        JsonArray options = ui->setOptions(var);
        options.add("None");
        files->dirToJson(options, true, ".sc"); //only files containing F(ixture), alphabetically

        return true; }
      case onChange: {
        //set script
        uint8_t fileNr = var["value"];

        SCExecutable._kill(); //kill any old 
        clearleds();
        driver.showPixels(WAIT);
        fps = 0;

        ppf("%s script f:%d f:%d\n", name, funType, fileNr);

        if (fileNr > 0) { //not None

          fileNr--;  //-1 as none is no file

          char fileName[32] = "";

          files->seqNrToName(fileName, fileNr, ".sc");

          // ppf("%s script f:%d f:%d\n", name, funType, fileNr);

          if (strcmp(fileName, "") != 0) {

            File f = files->open(fileName, "r");
            if (!f)
              ppf("UserModLive setup script  open %s for %s failed", fileName, "r");
            else {

              ScScript = string(f.readString().c_str());
              f.close();
              // ppf("%s\n", script.c_str());

              if (p.parse_c(&ScScript))
              {
                  SCExecutable.executeAsTask("main");
              }
            }
          }
          else
            ppf("UserModLive setup file for %d not found", fileNr);
        }
        return true; }
      default: return false; 
    }}); //script

    ui->initText(parentVar, "fps1", nullptr, 10, true);
    ui->initText(parentVar, "fps2", nullptr, 10, true);

    // ui->initButton
    driver.setHmap(_maping);
    driver.initled((uint8_t*)leds,pins,NUM_STRIPS,NUM_LEDS_PER_STRIP,ORDER_GRB);
    driver.setBrightness(10);
    driver.setMapLed(mapfunction);
    // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    // FastLED.setBrightness(30);

    addExternal("leds", externalType::value, (void *)leds);
    addExternal("show", externalType::function, (void *)&show);
    addExternal("hsv", externalType::function, (void *)POSV);
    addExternal("clear", externalType::function, (void *)clearleds);
    addExternal("resetStat", externalType::function, (void *)&resetShowStats);
    addExternal("map", externalType::function, (void *)__map);
    addExternal("initleds", externalType::function, (void *)__initleds);
    addExternal("pos", externalType::value, (void *)&map2);
    addExternal("atan2",externalType::function,(void*)_atan2);
    addExternal("hypot",externalType::function,(void*)_hypot);
    addExternal("sin8",externalType::function,(void*)_sin8);

    // addExternal("showM", externalType::function, (void *)&UserModLive::showM); // warning: converting from 'void (UserModLive::*)()' to 'void*' [-Wpmf-conversions]
    // addExternal("display", externalType::function, (void *)&dispshit);
    // addExternal("dp", externalType::function, (void *)displayfloat);
    // addExternal("sin", externalType::function, (void *)_sin);
    // addExternal("error", externalType::function, (void *)&showError);
    // addExternal("print", externalType::function, (void *)__print);

  }

  //testing class functions instead of static
  void showM() {
    long time2 = ESP.getCycleCount();
    // driver.showPixels(WAIT);
    frameCounter++;

    float k = (float)(time2 - time1) / 240000000; //always 240MHz?
    fps = 1 / k;
    time1 = ESP.getCycleCount();
  }

  void loop1s() {
    mdl->setUIValueV("fps1", "%.0f /s", fps);
    mdl->setUIValueV("fps2", "%d /s", frameCounter);
    frameCounter = 0;
  }

};

extern UserModLive *live;


//asm_parser.h:325:1: warning: control reaches end of non-void function 