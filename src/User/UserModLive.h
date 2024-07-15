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
#pragma once
#include "parser.h"

//LEDS specific
#define __HARDWARE_MAP
#define __NON_HEAP

#include "FastLED.h"
#include "I2SClocklessLedDriver.h"

#define NUM_LEDS_PER_STRIP 256
#define NUM_STRIPS 1 //nr of panels?
#define NUM_LEDS_MAX 3072 //NUM_LEDS_PER_STRIP * NUM_LEDS_MAX ?

#define width 16
#define height 16
#define NB_PANEL_WIDTH 1 //num strips / nb panel width = nb_panel height ???

CRGB leds[NUM_LEDS_MAX];
uint16_t _maping[NUM_LEDS_MAX];

int pins[NUM_STRIPS] = {2};

I2SClocklessLedDriver driver;

static void clearleds()
{
    memset(leds, 0, NUM_LEDS_MAX * 3);
}
//END LEDS specific

long time1;
long time4;
static float _min = 9999;
static float _max = 0;
static uint32_t _nb_stat = 0;
static float _totfps;
static float fps = 0; //integer?
static unsigned long frameCounter = 0;

//external function implementation (tbd: move into class)

static void show()
{
  frameCounter++;
    
  // SKIPPED: check nargs (must be 3 because arg[0] is self)
  long time2 = ESP.getCycleCount();

  driver.showPixels(WAIT); // LEDS specific

  long time3 = ESP.getCycleCount();
  float k = (float)(time2 - time1) / 240000000;
  fps = 1 / k; //StarBase: class variable so it can be shown in UI!!!
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

static void dispshit(int g) { ppf("coming from assembly int %x %d", g, g);}
static void __print(char *s) {ppf("from assembly :%s\r\n", s);}
static void showError(int line, uint32_t size, uint32_t got) { ppf("Overflow error line %d max size: %d got %d", line, size, got);}
static void displayfloat(float j) {ppf("display float %f", j);}

static float _hypot(float x,float y) {return hypot(x,y);}
static float _atan2(float x,float y) { return atan2(x,y);}
static float _sin(float j) {return sin(j);}

//LEDS specific
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

//END LEDS specific


class UserModLive:public SysModule {

public:

  Parser p = Parser();
  char fileName[32] = ""; //running sc file
  string scPreScript = ""; //externals etc generated (would prefer String for esp32...)

  UserModLive() :SysModule("Live") {
    isEnabled = false; //need to enable after fresh setup
  };

  void setup() override {
    SysModule::setup();

    parentVar = ui->initUserMod(parentVar, name, 6310);

    ui->initSelect(parentVar, "script", UINT16_MAX, false , [this](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onUI: {
        // ui->setComment(var, "Fixture to display effect on");
        JsonArray options = ui->setOptions(var);
        options.add("None");
        files->dirToJson(options, true, ".sc"); //only files containing F(ixture), alphabetically

        return true; }
      case onChange: {
        //set script
        uint8_t fileNr = var["value"];

        ppf("%s script f:%d f:%d\n", name, funType, fileNr);

        char fileName[32] = "";

        if (fileNr > 0) { //not None
          fileNr--;  //-1 as none is no file
          files->seqNrToName(fileName, fileNr, ".sc");
          // ppf("%s script f:%d f:%d\n", name, funType, fileNr);
        }

        if (strcmp(fileName, "") != 0)
          run(fileName, true); //force a new file to run
        else {
          kill();
        }

        return true; }
      default: return false; 
    }}); //script

    ui->initText(parentVar, "fps1", nullptr, 10, true);
    ui->initText(parentVar, "fps2", nullptr, 10, true);

    //Live scripts defaults
    addExternalFun("void", "show", "()", (void *)&show);
    // addExternalFun("void", "showM", "()", (void *)&UserModLive::showM); // warning: converting from 'void (UserModLive::*)()' to 'void*' [-Wpmf-conversions]
    addExternalFun("void", "resetStat", "()", (void *)&resetShowStats);

    addExternalFun("void", "display", "(int a1)", (void *)&dispshit);
    addExternalFun("void", "dp", "(float a1)", (void *)displayfloat);
    addExternalFun("void", "error", "(int a1, int a2, int a3)", (void *)&showError);
    addExternalFun("void", "print", "(char * a1)", (void *)__print);

    addExternalFun("float", "atan2","(float a1, float a2)",(void*)_atan2);
    addExternalFun("float", "hypot","(float a1, float a2)",(void*)_hypot);
    addExternalFun("float", "sin", "(float a1)", (void *)_sin);

    // added by StarBase
    addExternalFun("void", "pinMode", "(int a1, int a2)", (void *)&pinMode);
    addExternalFun("void", "digitalWrite", "(int a1, int a2)", (void *)&digitalWrite);
    addExternalFun("void", "delay", "(int a1)", (void *)&delay);

    // addExternalFun("delay", [](int ms) {delay(ms);});
    // addExternalFun("digitalWrite", [](int pin, int val) {digitalWrite(pin, val);});

    //LEDS specific
    
    driver.setHmap(_maping);
    driver.initled((uint8_t*)leds,pins,NUM_STRIPS,NUM_LEDS_PER_STRIP,ORDER_GRB);
    driver.setBrightness(10);
    driver.setMapLed(mapfunction);
    // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    // FastLED.setBrightness(30);

    addExternalVal("CRGB *", "leds", (void *)leds);
    addExternalFun("CRGB", "hsv", "(int a1, int a2, int a3)", (void *)POSV);
    addExternalFun("void", "clear", "()", (void *)clearleds);
    addExternalFun("void", "initleds", "(int *a1, int a2, int a3)", (void *)__initleds);
    addExternalFun("uint8_t", "sin8","(uint8_t a1)",(void*)_sin8); //using int here causes value must be between 0 and 16 error!!!

    //mapping stuff
    addExternalVal("uint16_t *", "pos", (void *)&map2); //used in map function
    addExternalFun("void", "map", "()", (void *)__map);

    scPreScript += "define width " + to_string(width) + "\n";
    scPreScript += "define height " + to_string(height) + "\n";
    scPreScript += "define NUM_LEDS " + to_string(width * height) + "\n";
    scPreScript += "define panel_width " + to_string(width) + "\n";  //isn't panel_width always the same as width?
    scPreScript += "define NB_PANEL_WIDTH " + to_string(NB_PANEL_WIDTH) + "\n";
    scPreScript += "define NUM_STRIPS " + to_string(NUM_STRIPS) + "\n";
    scPreScript += "define NUM_LEDS_PER_STRIP " + to_string(NUM_LEDS_PER_STRIP) + "\n";
    scPreScript += "uint32_t pins[NUM_STRIPS]={";
    char sep[2] = "";
    for (int i= 0; i<NUM_STRIPS; i++) {
      scPreScript += sep + to_string(pins[i]);
      strcpy(sep, ",");
    }
    scPreScript += "};\n";

    //END LEDS specific

  } //setup

  void addExternalVal(string result, string name, void * ptr) {
    addExternal(name, externalType::value, ptr);
    scPreScript += "external " + result + " " + name + ";\n";
  }

  void addExternalFun(string result, string name, string parameters, void * ptr) {
    addExternal(name, externalType::function, ptr);
    scPreScript += "external " + result + " " + name + parameters + ";\n";
  }

  // void addExternalFun(string name, std::function<void(int)> fun) {
  //   addExternal(name, externalType::function, (void *)&fun)); //unfortionately InstructionFetchError, why does it work in initText etc?
  //   ppf("external %s(int arg1);\n", name.c_str()); //add to string
  // }
  // void addExternalFun(string name, std::function<void(int, int)> fun) {
  //   addExternal(name, externalType::function, (void *)&fun); //unfortionately InstructionFetchError
  //   ppf("external %s(int arg1, int arg2);\n", name.c_str()); //add to string
  // }

  //testing class functions instead of static
  void showM() {
    long time2 = ESP.getCycleCount();
    // driver.showPixels(WAIT);
    frameCounter++;

    float k = (float)(time2 - time1) / 240000000; //always 240MHz?
    fps = 1 / k;
    time1 = ESP.getCycleCount();
  }

  void loop20ms() {
    //workaround
    if (strstr(web->lastFileUpdated, ".sc") != nullptr) {
      run(web->lastFileUpdated);
      strcpy(web->lastFileUpdated, "");
    }
  }

  void loop1s() {
    mdl->setUIValueV("fps1", "%.0f /s", fps);
    mdl->setUIValueV("fps2", "%d /s", frameCounter);
    frameCounter = 0;
  }

  void run(const char *fileName, bool force = false) {
    ppf("live run n:%s o:%s (f:%d)\n", fileName, this->fileName, force);

    if (!force && strcmp(fileName, this->fileName) != 0) // if another fileName then force should be true;
      return;

    kill();

    if (strcmp(fileName, "") != 0) {

      File f = files->open(fileName, "r");
      if (!f)
        ppf("UserModLive setup script open %s for %s failed\n", fileName, "r");
      else {

        string scScript = scPreScript + string(f.readString().c_str());

        Serial.println(scPreScript.c_str());

        if (p.parse_c(&scScript))
        {
          ppf("parsing done\n");
          SCExecutable.executeAsTask("main");
          strcpy(this->fileName, fileName);
        }
        f.close();
      }
    }
    else
      ppf("UserModLive setup file for %s not found\n", fileName);
  }

  void kill() {
    SCExecutable._kill(); //kill any old tasks
    fps = 0;
    strcpy(fileName, "");

    driver.showPixels(WAIT); //LEDS specific
    clearleds(); //LEDS specific
  }

};

extern UserModLive *liveM;


//asm_parser.h:325:1: warning: control reaches end of non-void function 