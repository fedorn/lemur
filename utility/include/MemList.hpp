#ifndef _MEMLISTH_
#define _MEMLISTH_

class MemList {
public:
  MemList(int incr, int align=8);
  void Reset();
  int NumberOfSegments() const { return numSegments; }
  int Used() const { return (((int)curp) - ((int)p)) + Total(); }
  int Total() const
    {
      int total = ((int)endp) - ((int)p);
      for (MemList *curml = next; curml; curml = curml->next)
	total += ((int)curml->endp) - ((int)curml->p);
      return total;
    }
  ~MemList();
  void *GetMem(int size);

private:
  int numSegments;
  int increment;
  int alignment;
  char *p;
  char *curp;
  char *endp;
  MemList *next;
};

#endif

