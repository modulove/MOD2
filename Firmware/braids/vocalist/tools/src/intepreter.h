#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

class Interpreter {

public:
  Interpreter();
  ~Interpreter();

  int Parser1();
  void Parser2();
  void CopyStress();
  void SetPhonemeLength();
  void AdjustLengths();
  void Code41240();

  void Insert(unsigned char position, unsigned char mem60, unsigned char mem59, unsigned char mem58);
  void InsertBreath(unsigned char mem59);
  bool LoadNextWord();

  void SetInput(char *_input);
  void ChangeRule(unsigned char position, unsigned char rule,unsigned char mem60, unsigned char mem59, unsigned char stress, const char * descr);

private:

  void drule(const char * str);
  void drule_pre(const char *descr, unsigned char X);
  void drule_post(unsigned char X);
  void rule_alveolar_uw(unsigned char X);
  void rule_ch(unsigned char X, unsigned char mem59);
  void rule_j(unsigned char X, unsigned char mem59);
  void rule_g(unsigned char pos);
  void change(unsigned char pos, unsigned char val, const char * rule);
  void rule_dipthong(unsigned char p, unsigned char pf, unsigned char pos, unsigned char mem59);

  char input[256]; //tab39445

}
#endif
