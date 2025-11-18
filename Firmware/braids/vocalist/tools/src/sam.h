#ifndef SAM_H
#define SAM_H

class SAM {

public:
  SAM() {
    // sam.cc
    mem59 = 0;

    //standard sam sound
    speed = 72;
    pitch = 64;
    mouth = 128;
    throat = 128;
    singmode = 0;

    speedcounter = 72;
    phase1 = 0;
    phase2 = 0;
    phase3 = 0;

    mem66 = 0;
    Init();
  }

  ~SAM() { }

  void Init();
  int Parser1();
  void Parser2();
  void CopyStress();
  void SetPhonemeLength();
  void AdjustLengths();
  void Code41240();

  void Insert(unsigned char position, unsigned char mem60, unsigned char mem59, unsigned char mem58);
  void InsertBreath(unsigned char mem59);
  bool LoadNextWord(const unsigned char *phonemeindex, const unsigned char *phonemeLength, const unsigned char *stress, int len);

  void SetInput(char *_input);
  void SetSpeed(unsigned char _speed);
  void SetPitch(unsigned char _pitch);
  void SetMouth(unsigned char _mouth);
  void SetThroat(unsigned char _throat);
  void EnableSingmode();
  void EnableDebug();

  int PreparePhonemes();

  char* GetBuffer();
  int GetBufferLength();
  void ChangeRule(unsigned char position, unsigned char rule,unsigned char mem60, unsigned char mem59, unsigned char stress, const char * descr);

  // ---- render.cc

  void Output(int index, unsigned char A, int *bufferpos, char *buffer);
  void PrepareFrames();
  void SetMouthThroat(unsigned char mouth, unsigned char throat);

  void RenderSample(unsigned char *mem66, unsigned char consonantFlag, unsigned char mem49, int *bufferpos, char *buffer);
  unsigned char RenderVoicedSample(unsigned short hi, unsigned char off, unsigned char phase1, int *bufferpos, char *buffer);
  void RenderUnvoicedSample(unsigned short hi, unsigned char off, unsigned char mem53, int *bufferpos, char *buffer);
  void CreateFrames();
  void RescaleAmplitude();
  void AssignPitchContour();
  void AddInflection(unsigned char inflection, unsigned char phase1, unsigned char pos);

  // processframes.cc
  void InitFrameProcessor();

  //void ProcessFrames(unsigned char mem48, int *bufferpos, char *buffer);
  int Drain(int threshold, int count, char *buffer);
  int FillBufferFromFrame(int count, char *buffer);

  unsigned char ProcessFrame(unsigned char Y, unsigned char mem48, int *bufferpos, char *buffer);
  void CombineGlottalAndFormants(unsigned char phase1, unsigned char phase2, unsigned char phase3, unsigned char Y, int *bufferpos, char *buffer);

  // createtransitions.cc
  unsigned char CreateTransitions();
  unsigned char Read(unsigned char p, unsigned char Y);
  void Write(unsigned char p, unsigned char Y, unsigned char value);
  void interpolate(unsigned char width, unsigned char table, unsigned char frame, unsigned char mem53);
  void interpolate_pitch(unsigned char width, unsigned char pos, unsigned char mem49, unsigned char phase3);

  // ---- sam.cc
  char input[256]; //tab39445
  unsigned char speed;
  unsigned char pitch;
  unsigned char mouth;
  unsigned char throat;
  int singmode;

  unsigned char mem39;
  unsigned char mem44;
  unsigned char mem47;
  unsigned char mem49;
  unsigned char mem50;
  unsigned char mem51;
  unsigned char mem53;
  unsigned char mem59;

  unsigned char X;

  // this is the cursor into the input phoneme list between word renders
  unsigned char srcpos;

  unsigned char stress[256]; //numbers from 0 to 8
  unsigned char phonemeLength[256]; //tab40160
  unsigned char phonemeindex[256];

  unsigned char phonemeIndexOutput[60]; //tab47296
  unsigned char stressOutput[60]; //tab47365
  unsigned char phonemeLengthOutput[60]; //tab47416

  void drule(const char * str);
  void drule_pre(const char *descr, unsigned char X);
  void drule_post(unsigned char X);
  void rule_alveolar_uw(unsigned char X);
  void rule_ch(unsigned char X, unsigned char mem59);
  void rule_j(unsigned char X, unsigned char mem59);
  void rule_g(unsigned char pos);
  void change(unsigned char pos, unsigned char val, const char * rule);
  void rule_dipthong(unsigned char p, unsigned char pf, unsigned char pos, unsigned char mem59);

  // ---- render.cc
  unsigned char pitches[256]; // tab43008

  unsigned char frequency1[256];
  unsigned char frequency2[256];
  unsigned char frequency3[256];

  unsigned char amplitude1[256];
  unsigned char amplitude2[256];
  unsigned char amplitude3[256];

  unsigned char sampledConsonantFlag[256]; // tab44800

  //processframes.cc
  unsigned char framesRemaining;
  unsigned char frameProcessorPosition;

  unsigned char speedcounter;
  unsigned char phase1;
  unsigned char phase2;

  unsigned char phase3;
  unsigned char mem66;
  unsigned char glottal_pulse;
  unsigned char mem38;
};

//char input[]={"/HAALAOAO MAYN NAAMAEAE IHSTT SAEBAASTTIHAAN \x9b\x9b\0"};
//unsigned char input[]={"/HAALAOAO \x9b\0"};
//unsigned char input[]={"AA \x9b\0"};
//unsigned char input[] = {"GUH5DEHN TAEG\x9b\0"};

//unsigned char input[]={"AY5 AEM EY TAO4LXKIHNX KAX4MPYUX4TAH. GOW4 AH/HEH3D PAHNK.MEYK MAY8 DEY.\x9b\0"};
//unsigned char input[]={"/HEH3LOW2, /HAW AH YUX2 TUXDEY. AY /HOH3P YUX AH FIYLIHNX OW4 KEY.\x9b\0"};
//unsigned char input[]={"/HEY2, DHIHS IH3Z GREY2T. /HAH /HAH /HAH.AYL BIY5 BAEK.\x9b\0"};
//unsigned char input[]={"/HAH /HAH /HAH \x9b\0"};
//unsigned char input[]={"/HAH /HAH /HAH.\x9b\0"};
//unsigned char input[]={".TUW BIY5Y3,, OHR NAA3T - TUW BIY5IYIY., DHAE4T IHZ DHAH KWEH4SCHAHN.\x9b\0"};
//unsigned char input[]={"/HEY2, DHIHS \x9b\0"};

//unsigned char input[]={" IYIHEHAEAAAHAOOHUHUXERAXIX  \x9b\0"};
//unsigned char input[]={" RLWWYMNNXBDGJZZHVDH \x9b\0"};
//unsigned char input[]={" SSHFTHPTKCH/H \x9b\0"};

//unsigned char input[]={" EYAYOYAWOWUW ULUMUNQ YXWXRXLX/XDX\x9b\0"};


#endif
