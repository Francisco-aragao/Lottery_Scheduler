#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pstat.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// getcnt syscall

extern int syscall_count[];
extern const int NELEM_SYSCALLS;
extern struct spinlock count_lock;

uint64 sys_getcnt(void) {
  int num, cnt;

  argint(0, &num);

  if (num > 0 && num < NELEM_SYSCALLS) {
    // Lock before reading
    acquire(&count_lock);
    cnt = syscall_count[num];
    release(&count_lock);

    return cnt;
  }

  return -1;
}

// settickets syscall

extern struct pstat pstat;
uint64 sys_settickets(void) {
  int num, pid;
  
  argint(0, &num);
  pid = myproc()->pid;

  //set num tickets to calling process (pid)
  if (num < 1) return -1;

  for(int i = 0; i < NPROC; i++) {
    if (pstat.pid[i] == pid && pstat.inuse[i]) {
      pstat.tickets[i] = num;
      return 0;
    }
  }

  return -1;
}

uint64 sys_getpinfo(void) {
  uint64 pstat_ptr;
  argaddr(0, &pstat_ptr);

  if ((void*) pstat_ptr == (void*) 0) return -1;

  memmove((void*) pstat_ptr, &pstat, sizeof(struct pstat));

  return 0;
}