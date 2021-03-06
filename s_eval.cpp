//#define LINUX

// #define FREE_CELL, if enable timer, need comment the macro

//#include <sstream>

#ifdef UEFI
  #define ENTER '\n'
#endif

#ifdef OS_CPP
  #define ENTER '\n'
  #include <iostream>
  #include <string>
  #include <list>
  #include <map>
  #include <vector>

  #include <cstdio>
  #include <cstdlib>
  #define myprint printf
  #define s_strncpy strncpy
  using namespace std;

#else // non os
  #define first k_ // DS::map use (k_,v_), but std::map use (first,second)
  #define second v_ // DS::map use (k_,v_), but std::map use (first,second)
  #include "myiostream.h"
  #include "k_string.h"
  #include "k_stdio.h"
  #include "cstring.h"
  #include "gdeque.h"
  #define strcmp s_strcmp
  #define sprintf s32_sprintf
  using namespace DS;
#endif

//#define RL
// readline
#ifdef RL
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "s_eval.h"

#ifdef LINUX
vector<Timer*> timer_list;
#endif

#ifdef OS_CPP
char* s32_itoa_s(int n, char* str, int radix)
{
  char digit[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char* p=str;
  char* head=str;
  //int radix = 10;

//  if(!p || radix < 2 || radix > 36)
//    return p;
  if (n==0)
  {
    *p++='0';
    *p=0;
    return str;
  }
  if (radix == 10 && n < 0)
  {
    *p++='-';
    n= -n;
  }
  while(n)
  {
    *p++=digit[n%radix];
    //s32_put_char(*(p-1), (u8*)(0xb8000+80*2));
    n/=radix;
  }
  *p=0;
  #if 1
  for (--p; head < p ; ++head, --p)
  {
    char temp=*head;
    if (*(p-1) != '-')
    {
      *head=*p;
      *p=temp;
    }
  }
  #endif
  return str;
}
#endif

#ifdef OS_CPP
typedef std::list<std::string> TC;
#else
typedef TokenContainer TC;
#endif

// return given mumber as a string
//std::string str(long n) { std::ostringstream os; os << n; return os.str(); }

// return true iff given character is '0'..'9'
bool isdig(char c) { return isdigit(static_cast<int>(c)); }

Cell invalid_cell;
Cell null_cell;
Cell lambda_cell;
Cell define_cell;
Cell true_cell;
Cell false_cell;
Cell begin_cell;
Cell if_cell;

#ifdef LINUX
void check_timer_list(int signo)
{
  for (auto &i : timer_list)
  {
    if (i->is_enable())
      ++(i->cur_count_);
    if (i->is_enable() && i->timeup())
    {
      Cell *ret = apply(i->func_, &null_cell);
      cout << "timer ret: " << ret->val_ << endl;
      i->reset_counter();
    }
  }

#if 1
  static int i=1;
#if 0
  if (i%1000 == 0)
  {
    if (timer_cell && timer_cell->timer_)
    {
    Cell *ret = apply(timer_cell->timer_, &null_cell);
    cout << "timer ret: " << ret->val_ << endl;
    ret = apply(timer_cell->timer_, &null_cell);
    cout << "timer ret: " << ret->val_ << endl;
    ret = apply(timer_cell->timer_, &null_cell);
    cout << "timer ret: " << ret->val_ << endl;
    ret = apply(timer_cell->timer_, &null_cell);
    cout << "timer ret: " << ret->val_ << endl;
    i=1;
    }
  }
  ++i;
#endif
#endif  
  //cout << "timer check" << endl;  
}
#endif

//#define USE_CPP_MAP

#ifndef USE_CPP_MAP
int add_variable(Environment *env, const char *variable, Cell *cell)
{
  if (env->free_frame_index_ < FRAME_LEN)
  {
    EnvElement *env_element = &env->frame_[env->free_frame_index_++];

    strcpy(env_element->variable_, variable);
    env_element->value_ = cell;
    return 0;
  }
  else
  {
    return -1;
  }
}


// return EnvElement * pointer need not to free it.
EnvElement *find_variable(Environment *env, const char *variable)
{
  for (int i=0 ; i < env->free_frame_index_ ; ++i)
  {
    EnvElement *env_element = &env->frame_[i];
    if (strcmp(env_element->variable_, variable) == 0)
    {
      return env_element;
    }
  }
  return 0;
}

#endif

Environment environment_pool[MAX_ENVIRONMENT_POOL];
int free_env_index;
int previous_free_env_index;

Environment *get_env(Environment *outer, const char *name)
{
  if (free_env_index >= MAX_ENVIRONMENT_POOL) return 0;

  Environment *env = &environment_pool[free_env_index++];
  env->outer_ = outer;
  env->free_frame_index_ = 0;
  strcpy(env->name_, name);
  return env;
}


void extend_environment(Cell *vars, Cell *vals, Environment *env)
{
  //cout << "-----\n";
  //print_cell(vars);
  //cout << "\n";
#if 0
  print_cell(vals);
  cout << "\n-----\n";

  Cell para = car_cell(vars);
  Cell arg = car_cell(vals);

  cout << "ext env: \n";
  print_cell(arg);
  cout << "\n-----\n";
#endif

  Cell *rest_vars = vars;
  Cell *rest_vals = vals;
  // need check vars, vals length are the same.
  while (rest_vars->type_ != NULL_CELL)
  {
  #if 0
    cout << "\n%%%\n";
    print_cell(rest_vars);
    cout << "\n%%%\n";
    cout << "add to env : " << env->name_ << endl;
    cout << "car_cell(rest_vars)->val_: " << car_cell(rest_vars)->val_ << endl;
    cout << "value of " << car_cell(rest_vars)->val_ << ":" << endl;
  #endif
    print_cell(car_cell(rest_vals));
    //cout << "\n====\n";

#ifdef USE_CPP_MAP
    //env->frame_.insert(Frame::value_type( car_cell(rest_vars)->val_, car_cell(rest_vals)));
    env->frame_.insert({car_cell(rest_vars)->val_, car_cell(rest_vals)});
#else
  if (add_variable(env, car_cell(rest_vars)->val_, car_cell(rest_vals)) == -1)
  {
    //cout << "add_variable faile\n";
  }

#endif

    rest_vars = cdr_cell(rest_vars);
    rest_vals = cdr_cell(rest_vals);
  }
  
  // need check vars.list.size() == vals.list.size()
  // vars/vals is a List
  //for (int i = 0 ; i < vars.list.size() ; ++i)
    //env->frame_.insert(Frame::value_type(vars.list[i].val, vals.list[i]));
}

Cell* lookup_variable_value(const Cell *exp, Environment *env)
{
#ifdef DEBUG_OP
  {
  cout << "lookup: " << exp->val_ << " in environment ## "<< env->name_ << " val_: " << exp->val_ << endl;
  auto it = env->frame_.begin();
  for ( ; it != env->frame_.end() ; ++it)
  {
    cout << it->first << endl;
  }
  }
#endif

#ifdef USE_CPP_MAP
  auto it = env->frame_.find(exp->val_);
  #ifdef USE_MYMAP
  if (it != 0) // find it
  #else
  if (it != env->frame_.end()) // find it
  #endif
  {
    return (*it).second;
  }
  else
  {
    if (env->outer_ != 0)
      return lookup_variable_value(exp, env->outer_);
  }
#else
  EnvElement *env_e = find_variable(env, exp->val_);

  if (env_e) // found
  {
    return env_e->value_;
  }
  else
  {
    if (env->outer_ != 0)
      return lookup_variable_value(exp, env->outer_);
  }

#if 0
  for (int i=0 ; i < env->free_frame_index_ ; ++i)
  {
    const EnvElement *env_element = &env->frame_[i];
    if (strcmp(exp->val_, env_element->variable_) == 0)
    {
      cout << "found it" << endl;
      return env_element->value_;
    }
  }
#endif

#endif
  //cout << "not found it" << endl;
  return &invalid_cell;
}

Cell *eval(Cell *exp, Environment *env);


#if 0
// sign version
char* s32_itoa_s(int n, char* str, int radix)
{
  char digit[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char* p=str;
  char* head=str;
  //int radix = 10;

//  if(!p || radix < 2 || radix > 36)
//    return p;
  if (n==0)
  {
    *p++='0';
    *p=0;
    return str;
  }
  if (radix == 10 && n < 0)
  {
    *p++='-';
    n= -n;
  }
  while(n)
  {
    *p++=digit[n%radix];
    //s32_put_char(*(p-1), (u8*)(0xb8000+80*2));
    n/=radix;
  }
  *p=0;
  #if 1
  for (--p; head < p ; ++head, --p)
  {
    char temp=*head;
    if (*(p-1) != '-')
    {
      *head=*p;
      *p=temp;
    }
  }
  #endif
  return str;
}
#endif

int add_cell(Cell *c)
{
  Cell *cell = car_cell(c);

  int sum =0 ;
  if (cell->type_ == PAIR)
    return add_cell(cdr_cell(c));
  else if (cell->type_ != NULL_CELL)
       {
         //cout << cell->val_ << endl;
         return sum += atoi(cell->val_);
       }
       else
         return 0;
}

Cell *proc_exit(Cell *cell)
{
#ifndef OS_CPP
  void exit(int status);
#endif
  exit(0);
  return &true_cell;
}

Cell *proc_get_timer(Cell *cell)
{
  Cell *timer_name = car_cell(cell);
  Cell *interval = car_cell(cdr_cell(cell));
  Cell *call_back_func = car_cell(cdr_cell(cdr_cell(cell)));

  cout << "timer_name: " << timer_name->val_ << endl;
  cout << "interval: " << interval->val_ << endl;

  return get_cell(timer_name->val_, TIMER);
}

Cell *proc_add(Cell *cell)
{
  int sum=0;
  Cell *c = car_cell(cell);
  Cell *rest=cell;

  while (rest->type_ != NULL_CELL)
  {
#if 0
    cout << "\n%%%\n";
    print_cell(rest);
    cout << "\n%%%\n";
#endif
    sum += atoi(car_cell(rest)->val_);
    rest = cdr_cell(rest);
  }
  //cout << "sum: " << sum << endl;
  char str[20];
  return get_cell(s32_itoa_s(sum, str, 10), NUMBER);
}

Cell *proc_sub(Cell *cell)
{
  Cell *c = car_cell(cell);
  Cell *rest=cdr_cell(cell);
  int sub = atoi(c->val_);

  while (rest->type_ != NULL_CELL)
  {
    sub -= atoi(car_cell(rest)->val_);
    rest = cdr_cell(rest);
  }
  //cout << "sub: " << sub << endl;
  char str[20];
  return get_cell(s32_itoa_s(sub, str, 10), NUMBER);
#if 0
  int result = sub_cell(c);
  cout << "result: " << result << endl;
  return Cell(Number, str(result));
#endif
}

Cell *proc_mul(Cell *cell)
{
  int product=1;
  Cell *c = car_cell(cell);
  Cell *rest=cell;

  while (rest->type_ != NULL_CELL)
  {
    product *= atoi(car_cell(rest)->val_);
    rest = cdr_cell(rest);
  }
  //cout << "product: " << product << endl;
  char str[20];
  return get_cell(s32_itoa_s(product, str, 10), NUMBER);
}

#ifndef X86_16 // support less proc_xxx, the block will don't support in x86/16bit mode
Cell *proc_less(Cell *cell)
{
  // (< x1 x2 x3 ...) 理解成數學上的 x1 < x2 < x3 
  // ref: http://www.r6rs.org/final/html/r6rs/r6rs-Z-H-14.html#node_idx_466
  Cell *first = car_cell(cell);
  Cell *second = car_cell(cdr_cell(cell));
  if (first->type_ == NUMBER && second->type_ == NUMBER)
  {
    if (atoi(first->val_) < atoi(second->val_))
      return &true_cell;
  }
  return &false_cell;
}

Cell *proc_greater(Cell *cell)
{
  // (> x1 x2 x3 ...) 理解成數學上的 x1 > x2 > x3 
  // ref: http://www.r6rs.org/final/html/r6rs/r6rs-Z-H-14.html#node_idx_466
  Cell *first = car_cell(cell);
  Cell *second = car_cell(cdr_cell(cell));
  if (first->type_ == NUMBER && second->type_ == NUMBER)
  {
    if (atoi(first->val_) > atoi(second->val_))
      return &true_cell;
  }
  return &false_cell;
}

Cell *proc_equal(Cell *cell)
{
  // (= x1 x2 x3 ...) 理解成數學上的 x1 = x2 = x3 
  // ref: http://www.r6rs.org/final/html/r6rs/r6rs-Z-H-14.html#node_idx_466
  Cell *first = car_cell(cell);
  Cell *second = car_cell(cdr_cell(cell));
  if (first->type_ == NUMBER && second->type_ == NUMBER)
  {
    if (atoi(first->val_) == atoi(second->val_))
      return &true_cell;
  }
  return &false_cell;

}

// print frame variable/value
void print_env_content(const Environment *env)
{
#if 0
#ifdef USE_CPP_MAP
  Frame::const_iterator it = env->frame_.begin();
  for (; it != env->frame_.end() ; ++it)
  {
    cout << "(" << it->first << "," << it->second->val_ << ")" << endl;
  }
#else
  for (int i=0 ; i < env->free_frame_index_ ; ++i)
  {
    const EnvElement *env_element = &env->frame_[i];
    cout << "(" << env_element->variable_ << "," << env_element->value_->val_ << ")" << endl;
  }

#endif
#endif
}

int print_env(Environment *env, int count)
{
#if 0
  for (int i=0 ; i < count*2 ; ++i)
    cout << ' ';
  cout << "env name: " << env->name_ << endl;

  if (env->outer_ != 0)
  {
    print_env(env->outer_, count+1);
  }
  return 0;
#endif
}

#ifdef RPI2

#define GPIO_BASE       0x3F200000UL
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;


Cell *proc_led_init(Cell *cell)
{
  cout << "led init" << endl;

  /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
#if 1
  gpio[4] &= (~(7 << 21));
  gpio[4] |= (1 << 21);
#endif
  return &true_cell;
}

void led_on()
{
  gpio[8] = (1 << 15); // led on
}

void led_off()
{
  gpio[11] = (1 << 15); // led off
}

Cell *proc_led(Cell *cell)
{
  Cell *led_cmd = car_cell(cell);
  cout << "cmd: " << led_cmd->val_ << endl;
  if (strcmp(led_cmd->val_, "1") == 0)
  {
    cout << "led on" << endl;
    led_on();  
  }
  else
  {
    cout << "led off" << endl;
    led_off();  
  }

  return &true_cell;
}
#endif

// (md addr   [width]    [count])
// (md 0x1234 8/16/*32   *1/2/3/4)
// (md 0x1234 32         1)
// * is default value
Cell *proc_md(Cell *cell)
{
  Cell *addr = car_cell(cell);
  Cell *width = car_cell(cdr_cell(cell));
  Cell *count = car_cell(cdr_cell(cdr_cell(cell)));
  int w=32;
  int c=1;
  int dump_addr;
  if (addr != &null_cell)
  {
    dump_addr = strtoul(addr->val_, 0, 16);
  }
  if (width != &null_cell)
  {
    w = strtol(width->val_, 0, 16);
  }
  if (count != &null_cell)
  {
    c = strtol(count->val_, 0, 16);
  }
  #if 0
  if (third != &null_cell)
  {
    strcpy(invalid_cell.val_, "requires exactly 2 argument");
    return &invalid_cell;
  }
  cout << "addr: " << addr->val_ << endl;
  cout << "width: " << width->val_ << endl;
  cout << "count: " << count->val_ << endl;
  #endif
  cout << hex << dump_addr << ": " << *((volatile unsigned int*)dump_addr) << dec << endl;
  return &true_cell;
}

// (mm addr   value   [width]    [count])
// (mm 0x1234         8/16/*32   *1/2/3/4)
// (mm 0x1234 0xffee  32         1)
// * is default value
Cell *proc_mm(Cell *cell)
{
  Cell *addr = car_cell(cell);
  Cell *value = car_cell(cdr_cell(cell));
  Cell *width = car_cell(cdr_cell(cdr_cell(cell)));
  Cell *count = car_cell(cdr_cell(cdr_cell(cdr_cell(cell))));
  static int w=32;
  static int c=1;
  static int w_addr;
  static int w_val;;
  if (addr != &null_cell)
  {
    w_addr = strtol(addr->val_, 0, 16);
  }
  if (value != &null_cell)
  {
    w_val = strtol(value->val_, 0, 16);
  }
  if (width != &null_cell)
  {
    w = strtol(width->val_, 0, 16);
  }
  if (count != &null_cell)
  {
    c = strtol(count->val_, 0, 16);
  }
  cout << hex << "w_addr: " << w_addr << ", w_val: " << w_val << dec << ", width: " << w << ", count: " << c << endl;
  *((volatile unsigned int*)w_addr) = w_val;

  return &true_cell;
}

Cell *proc_env(Cell *cell)
{
  Cell *first = car_cell(cell);
  int i= atoi(first->val_);
#if 0
  cout << "environment hierarchy:" << endl;
  print_env(environment_pool + i, 0);
  print_env_content(environment_pool + i);
  cout << endl;
#endif
  return &true_cell;
#if 0
  // need check first is null_cell or invalid_cell
  for (int i=0 ; i < free_env_index ; ++i)
  {
    if (strcmp(first->val_, environment_pool[i].name_) == 0)
    {
      cout << "env name: " << environment_pool[i].name_ << endl;
      cout << "env outer: " << (environment_pool[i].outer_ == 0 ? "no outer env" : environment_pool[i].outer_->name_);
      return &true_cell;
    }
  }
  
  return &false_cell;
#endif
}

// list all environment
Cell *proc_env_list(Cell *cell)
{
#if 0
  cout << "=============" << endl;
  for (int i=0 ; i < free_env_index ; ++i)
  {
    cout << i << ": " << environment_pool[i].name_ << endl;
  }
  cout << "=============" << endl;
#endif
  return &true_cell;
}

Cell *proc_list(Cell *cell)
{
  return cell;
}

Cell *proc_cons(Cell *cell)
{
  Cell *first = car_cell(cell);
  Cell *second = car_cell(cdr_cell(cell));
  Cell *third = cdr_cell(cdr_cell(cell));
  if (third != &null_cell)
  {
    strcpy(invalid_cell.val_, "requires exactly 2 argument");
    return &invalid_cell;
  }
  return cons_cell(first, second);
}

Cell *proc_car(Cell *cell)
{
  // should one argument
  Cell *first = car_cell(cell);
  Cell *second = cdr_cell(cell);
  if (second != &null_cell)
  {
    strcpy(invalid_cell.val_, "requires exactly 1 argument");
    return &invalid_cell;
  }
  return car_cell(first);
}

Cell *proc_cdr(Cell *cell)
{
  // should one argument
  Cell *first = car_cell(cell);
  Cell *second = cdr_cell(cell);
  if (second != &null_cell)
  {
    strcpy(invalid_cell.val_, "requires exactly 1 argument");
    return &invalid_cell;
  }
  return cdr_cell(first);
}

Cell *proc_pool_status(Cell *cell)
{
  // create (+ 1 2) list
  // use 3 cells
  // use 3 pairs

  extern Cell pair_pool[];
  extern Cell cell_pool[];

  extern int free_pair_index;
  extern int free_cell_index;

  cout << "cell pool max: " << MAX_POOL << endl;
  cout << "pair pool max: " << MAX_POOL << endl;
  cout << "Cell size: " << sizeof(Cell) << " bytes" << endl;
  cout << "cell pool index: " << free_cell_index << endl;
  cout << "pair pool index: " << free_pair_index << endl;
  cout << "env pool max: " << MAX_ENVIRONMENT_POOL << endl;
  cout << "Environment size: " << sizeof(Environment) << " bytes" << endl;
  cout << "free_env_index: " << free_env_index << endl;

#ifdef OS_CPP
#else
#ifdef SP_STATUS
  extern u32 sp_val;
  u32 get_stack_reg();

  u32 cur_sr = get_stack_reg();

  myprint("min stack reg: ");
  myprint(sp_val, 16);
  myprint("\r\n");

  myprint("current stack reg: ");
  myprint(cur_sr, 16);
  myprint("\r\n");

#endif

#endif
  return &true_cell;
}

#endif

#ifdef USE_CPP_MAP
  #define ADD_VAR(env, var, val) env->frame_.insert({var, op});
#else
  #define ADD_VAR(env, var, val) add_variable(env, var, op);
#endif

void create_primitive_procedure(Environment *env)
{
  Cell *op = get_cell("primitive add", proc_add);
  ADD_VAR(env, "+", op)

  op = get_cell("primitive mul", proc_mul);
  ADD_VAR(env, "*", op)

  op = get_cell("primitive sub", proc_sub);
  ADD_VAR(env, "-", op)

  op = get_cell("primitive exit", proc_exit);
  ADD_VAR(env, "exit", op)
  ADD_VAR(env, "quit", op)

#ifdef RPI2
  op = get_cell("primitive led_init", proc_led_init); 
  ADD_VAR(env, "led_init", op)

  op = get_cell("primitive led", proc_led); 
  ADD_VAR(env, "led", op)
#endif

#ifndef X86_16 // support less proc_xxx, the block will don't support in x86/16bit mode
  op = get_cell("primitive less", proc_less);
  ADD_VAR(env, "<", op)

  op = get_cell("primitive greater", proc_greater);
  ADD_VAR(env, ">", op)

  op = get_cell("primitive equal", proc_equal);
  ADD_VAR(env, "=", op)

  op = get_cell("primitive pool_status", proc_pool_status);
  ADD_VAR(env, "pool_status", op)

  op = get_cell("primitive car", proc_car);
  ADD_VAR(env, "car", op)

  op = get_cell("primitive cdr", proc_cdr);
  ADD_VAR(env, "cdr", op)

  op = get_cell("primitive cons", proc_cons);
  ADD_VAR(env, "cons", op)

  op = get_cell("primitive list", proc_list);
  ADD_VAR(env, "list", op)

  op = get_cell("primitive env_list", proc_env_list);
  ADD_VAR(env, "env_list", op)

  op = get_cell("primitive env", proc_env);
  ADD_VAR(env, "env", op)

  op = get_cell("primitive md", proc_md); // memory display
  ADD_VAR(env, "md", op)

  op = get_cell("primitive mm", proc_mm); // memory modify
  ADD_VAR(env, "mm", op)

#endif

  op = get_cell("primitive get-timer", proc_get_timer);
  ADD_VAR(env, "get-timer", op)

  //frame.insert(Frame::value_type("true", &true_cell));
  ADD_VAR(env, "true", &true_cell)
  //frame.insert(Frame::value_type("false", &false_cell));
  ADD_VAR(env, "false", &false_cell)
}

#if 0
void create_primitive_procedure(Environment *env)
{
  int ret=-1;

  Cell *op = get_cell("primitive add", proc_add);
  ret = add_variable(env, "+", op);

  op = get_cell("primitive mul", proc_mul);
  ret = add_variable(env, "*", op);
}

#endif

#if 0
// a dictionary that (a) associates symbols with cells, and
// (b) can chain to an "outer" dictionary
struct environment {
    environment(environment * outer = 0) : outer_(outer) {}

    environment(const cells & parms, const cells & args, environment * outer)
    : outer_(outer)
    {
        cellit a = args.begin();
        for (cellit p = parms.begin(); p != parms.end(); ++p)
            env_[p->val] = *a++;
    }

    // map a variable name onto a cell
    typedef std::map<std::string, cell> map;

    // return a reference to the innermost environment where 'var' appears
    map & find(const std::string & var)
    {
        if (env_.find(var) != env_.end())
            return env_; // the symbol exists in this environment
        if (outer_)
            return outer_->find(var); // attempt to find the symbol in some "outer" env
        std::cout << "unbound symbol '" << var << "'\n";
        exit(1);
    }

    // return a reference to the cell associated with the given symbol 'var'
    cell & operator[] (const std::string & var)
    {
        return env_[var];
    }
    
private:
    map env_; // inner symbol->cell mapping
    environment * outer_; // next adjacent outer env, or 0 if there are no further environments
};
#endif

#if 0
////////////////////// built-in primitive procedures

cell proc_div(const cells & c)
{
    long n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i) n /= atol(i->val.c_str());
    return cell(Number, str(n));
}

cell proc_greater(const cells & c)
{
    long n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n <= atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_less(const cells & c)
{
    long n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n >= atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_less_equal(const cells & c)
{
    long n(atol(c[0].val.c_str()));
    for (cellit i = c.begin()+1; i != c.end(); ++i)
        if (n > atol(i->val.c_str()))
            return false_sym;
    return true_sym;
}

cell proc_length(const cells & c) { return cell(Number, str(c[0].list.size())); }
cell proc_nullp(const cells & c)  { return c[0].list.empty() ? true_sym : false_sym; }
cell proc_car(const cells & c)    { return c[0].list[0]; }

cell proc_cdr(const cells & c)
{
    if (c[0].list.size() < 2)
        return nil;
    cell result(c[0]);
    result.list.erase(result.list.begin());
    return result;
}

cell proc_append(const cells & c)
{
    cell result(List);
    result.list = c[0].list;
    for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) result.list.push_back(*i);
    return result;
}

cell proc_cons(const cells & c)
{
    cell result(List);
    result.list.push_back(c[0]);
    for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) result.list.push_back(*i);
    return result;
}

cell proc_list(const cells & c)
{
    cell result(List); result.list = c;
    return result;
}

// define the bare minimum set of primintives necessary to pass the unit tests
void add_globals(environment & env)
{
    env["nil"] = nil;   env["#f"] = false_sym;  env["#t"] = true_sym;
    env["append"] = cell(&proc_append);   env["car"]  = cell(&proc_car);
    env["cdr"]    = cell(&proc_cdr);      env["cons"] = cell(&proc_cons);
    env["length"] = cell(&proc_length);   env["list"] = cell(&proc_list);
    env["null?"]  = cell(&proc_nullp);    env["+"]    = cell(&proc_add);
    env["-"]      = cell(&proc_sub);      env["*"]    = cell(&proc_mul);
    env["/"]      = cell(&proc_div);      env[">"]    = cell(&proc_greater);
    env["<"]      = cell(&proc_less);     env["<="]   = cell(&proc_less_equal);
}
#endif


////////////////////// parse, read and user interaction

#if 0
static int parenthesis_count=0;

// convert given string to list of tokens
int tokenize(const std::string & str, std::list<std::string> &tokens)
{
  //cout << "org str: " << str << endl;
  const char * s = str.c_str();
  while (*s) 
  {
    while (*s == ' ')
      ++s;
    if (*s == '(' || *s == ')')
    {
      if (*s == '(')
        ++parenthesis_count;
      if (*s == ')')
        --parenthesis_count;
      tokens.push_back(*s++ == '(' ? "(" : ")");
    }
    else 
    {
      const char * t = s;
      while (*t && *t != ' ' && *t != '(' && *t != ')')
      {
        if (*s == '(')
          ++parenthesis_count;
        if (*s == ')')
          --parenthesis_count;
        ++t;
      }
      tokens.push_back(std::string(s, t));
      //cout << "str: " << s << endl;
      s = t;
    }
  }
  //cout << "parenthesis_count:" << parenthesis_count << endl;

  return parenthesis_count;
}
#endif


#ifdef OS_CPP
Cell *make_list(vector<Cell *> cells)
{
  Cell *c;
  c = cons_cell(cells[cells.size()-1], &null_cell); 

  for (int i = cells.size()-2 ; i>=0 ; --i)
  {
    c = cons_cell(cells[i], c);
  }
  return c;
}


#endif


Cell *make_list(Cell *cells[], int len)
{
  if (len == 0)
    return &null_cell;
    //return get_cell("empty", NUMBER);

  Cell *c;
  c = cons_cell(cells[len-1], &null_cell); 

  for (int i = len-2 ; i>=0 ; --i)
  {
    c = cons_cell(cells[i], c);
  }
  return c;
}


  const int CELL_SIZE = 256;

// return the Lisp expression in the given tokens
// return the Lisp expression represented by the given string
Cell *read_from(TC &tokens)
{
#ifdef OS_CPP
  const char *token = tokens.front().c_str();
#else
  const char *token = tokens.front();
#endif
  int total_cells = 0; 
  //char *token = tokens;
  tokens.pop_front();
  if (strcmp(token, "(") == 0)
  {
    //vector<Cell *> cells;
    Cell *cells[CELL_SIZE];

#ifdef OS_CPP
    while (strcmp(tokens.front().c_str(), ")") != 0)
#else
    while (strcmp(tokens.front(), ")") != 0)
#endif
    {
      cells[total_cells++] = read_from(tokens);
    }
    tokens.pop_front();
    return make_list(cells, total_cells);
  }
  else
  {
    Cell *cell;
    if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
    {
      //myprint("\r\nnumber\r\n");
      cell = get_cell(token, NUMBER);
    }
    else
    {
      //myprint("\r\nsymbol\r\n");
      cell = get_cell(token, SYMBOL);
    }
    //cout << "cell val_: " << cell->val_ << endl;
    return cell;
  }
}

Cell *read(TC &tokens)
{
  if (tokens.size() > 0)
    return read_from(tokens);
  else
  {
    strcpy(invalid_cell.val_, "no input string");
    return &invalid_cell;
  }
}

Cell *list_of_values(Cell *exp, Environment *env)
{
  Cell *first_operand = car_cell(exp);
  Cell *rest_operands = cdr_cell(exp);
#if 0
  cout << "\nlist_of_values" << endl;
  print_cell(exp);
  cout << "\nfirst" << endl;
  print_cell(first_operand);
  cout << "\nrest" << endl;
  print_cell(rest_operands);
  cout << endl;
#endif

  if (exp->type_ == NULL_CELL)
  {
    //cout << "i am null" << endl;
    return &null_cell;
  }
  else
    return cons_cell(eval(first_operand, env), list_of_values(rest_operands, env));
#if 0
  //cout << "list_of_values" << endl;
  Cell ret_cell(List);

  if (exp.list.size() != 0)
  {

    Cell rear(List);
    std::copy(exp.list.begin()+1, exp.list.end(), back_inserter(rear.list));

    Cell eval_cell = eval(exp.list[0], env);
    Cell list_cell = list_of_values(rear, env);

    if (rear.list.size() != 0)
    {
      if (eval_cell.kind() == List)
      {
        for (int i=0 ; i < eval_cell.list.size() ; ++i)
          ret_cell.list.push_back(eval_cell.list[i]);
      }
      else
        ret_cell.list.push_back(eval_cell);

      if (list_cell.kind() == List)
      {
        for (int i=0 ; i < list_cell.list.size() ; ++i)
          ret_cell.list.push_back(list_cell.list[i]);
      }
      else
      {
        ret_cell.list.push_back(list_cell);
      }
    }
    else
    {
      return eval_cell;
    }
    //cout << ret_cell.list[0].kind_str() << endl;
    //cout << ret_cell.list[1].kind_str() << endl;
  }
  return ret_cell;
#endif
}

Cell *eval_sequence(Cell *exp, Environment *env)
{
#if 0
  cout << "eval_sequence: " << endl;
  cout << endl;
  print_cell(exp);
  cout << endl;

  //for (int i=0 ; i < exp.list.size()-1 ; ++i)
    //eval(exp.list[i], env);
#endif
  Cell *first = car_cell(exp);
  Cell *rest = cdr_cell(exp);

#if 0
  cout << "\nfirst\n";
  print_cell(first);
  cout << "\nrest\n";
  print_cell(rest);
  cout << endl;
#endif

  if (rest->type_ == NULL_CELL)
  {
    return eval(first, env);
  }
  else
  {
    eval(first, env);
    return eval_sequence(rest, env);
  }
}

// args is a list which terminals by '(). ex: (1 2 '()) or ((1 2) (3 4) '())
Cell *apply(Cell *func, Cell *args)
{
  //Cell *eval(const Cell &exp, Environment *env);
  //cout << "apply:" << func->type_str() << endl;
  //cout << endl;
  //return func->proc_(args);
  //print_cell(func);
  //cout << endl;
#if 0
      cout << "\napply args: \n";
      print_cell(args);
      cout << endl;
#endif
  if (func->lambda_ == true)
  {
      // new a Environment
      // add parameters and arguments pair

      Cell *body = cdr_cell(func);
      Cell *parameters = car_cell(func);
#if 0
      cout << "\napply func: \n";
      print_cell(func);
      cout << "\napply body: \n";
      print_cell(body);
      cout << "\napply parameters: \n";
      print_cell(parameters);
#endif  
      static int env_counter=0;
      char env_name[255];
      sprintf(env_name, "e%d", env_counter++);
      //cout << "\nouter env: " << func->env_->name_ << endl;
      Environment *env = get_env(func->env_, env_name);
      if (env == 0)
      {
        strcpy(invalid_cell.val_, "env pool is empty");
        return &invalid_cell;
      }

      //if (args != &null_cell)
      if (parameters != &null_cell)
        extend_environment(parameters, args, env);

      return eval_sequence(body, env);

  }
  else if (func->type_ == PRIMITIVE_PROC)
       {
         //cout << "primitive proc: " << func->val_ << endl;
         return func->proc_(args);
       }

  return &invalid_cell;
}

Cell *make_procedure(Cell *parameters, Cell *body, Environment *env)
{
  Cell *lambda_proc = cons_cell(parameters, body);

#if 0
          cout << "xx parameters: " << endl;
          print_cell(parameters);
          cout << endl;
          cout << "xx body: " << endl;
          print_cell(body);
          cout << endl;
#endif
  //lambda_proc->type_ = LAMBDA_PROC;
  lambda_proc->lambda_ = true;
  lambda_proc->env_ = env;
#if 0
  cout << "lambda proc: " << endl;
  print_cell(lambda_proc);
  cout << endl;
#endif
  return lambda_proc;
}

bool tagged_list(Cell *exp, const char *tag)
{
  if (exp->type_ == PAIR)
  {
    if (strcmp(car_cell(exp)->val_, tag) == 0)
      return true;
    else
      return false;
  }
  else
  {
    return false;
  }
}

Cell *definition_variable(Cell *exp)
{
  if (car_cell(cdr_cell(exp))->type_ == SYMBOL)
  {
    // ex: (define a 9)
    return car_cell(cdr_cell(exp));
  }
  else
  {
    // ex: (define (plus4 x) (+ 4 x))
    // ex: (define (bb) 6)
    return car_cell(car_cell(cdr_cell(exp)));
  }
}

Cell *make_lambda(Cell *parameters, Cell *body)
{
  return cons_cell(&lambda_cell, cons_cell(parameters, body));
}

Cell *definition_value(Cell *exp)
{
  if (car_cell(cdr_cell(exp))->type_ == SYMBOL)
  {
    return car_cell(cdr_cell((cdr_cell(exp))));
  }
  else
  {
    // ex: (define (plus4 x) (+ 4 x))
    Cell *para = cdr_cell(car_cell(cdr_cell(exp)));
    Cell *body = cdr_cell(cdr_cell(exp));
    return make_lambda(para, body);
  }
}

// I rename it to set_variable.
// The name of define-variable! is in sicp scheme code.
void set_variable(Cell *var, Cell *val, Environment *env)
{
#ifdef USE_CPP_MAP
  auto it = env->frame_.find(var->val_);
  #ifdef USE_MYMAP
  if (it != 0) // find it
  #else
  if (it != env->frame_.end()) // find it
  #endif
  {
    it->second = val;
  }
  else // not fount
  {
    env->frame_.insert(Frame::value_type(var->val_, val));
  }
#else
  EnvElement *env_e = find_variable(env, var->val_);

  if (env_e) // found
  {
    env_e->value_ = val;
  }
  else
  {
    if (add_variable(env, var->val_, val) != -1)
    {
      //cout << "add variable\n";
    }
    else
    {
      //cout << "add variable fail\n";
    }
  }

#endif
#if 0
  cout << "var:" << endl;
  print_cell(var);
  cout << endl;
  cout << "val:" << endl;
  print_cell(val);
  cout << endl;
  //exit(0);
#endif
}

Cell *eval_definition(Cell *exp, Environment *env)
{
  set_variable(definition_variable(exp), eval(definition_value(exp), env), env);
  return &define_cell;
}

Cell *eval_if(Cell *exp, Environment *env)
{
  // (if (< 1 0) -1 1)
  if (eval(car_cell(cdr_cell(exp)), env) == &true_cell)
  {
    return eval(car_cell(cdr_cell(cdr_cell(exp))), env);
  }
  else
  {
    if (cdr_cell(cdr_cell(cdr_cell(exp))) != &null_cell)
    {
      return eval(car_cell(cdr_cell(cdr_cell(cdr_cell(exp)))), env);
    }
    else
    {
      return &false_cell;
    }
  }
}

Cell *make_begin(Cell *seq)
{
  return cons_cell(&begin_cell, seq);
}

Cell *sequence_to_exp(Cell *seq)
{
  if (seq == &null_cell)
  {
    return seq;
  }
  else if (cdr_cell(seq) == &null_cell)
       {
         return car_cell(seq);
       }
       else
       {
         return make_begin(seq);
       }
}

Cell *make_if(Cell *predicate, Cell *consequent, Cell *alternative)
{
  return cons_cell(&if_cell, cons_cell(predicate, cons_cell(consequent, cons_cell(alternative, &null_cell))));
}



Cell *expand_clauses(Cell *clauses)
{
  // (cond ((< 0 1) (+ 1 2) (* 3 5)) ((> 2 1) (+ 3 5)))
  // (cond ((> 0 1) (+ 1 2)) ((< 2 1) (+ 3 5)) (else (+ 8 9)))
  // when pass to the function, don't have cond
  // (((< 0 1) (+ 1 2) (* 3 5)) ((> 2 1) (+ 3 5)))
  // (((> 0 1) (+ 1 2)) ((< 2 1) (+ 3 5)) (else (+ 8 9)))
  if (clauses == &null_cell)
  {
    return &false_cell;
  }
  else
  {
    Cell *first = car_cell(clauses);
    Cell *rest = cdr_cell(clauses);
    if (tagged_list(first, "else"))
    {
      if (rest == &null_cell)
      {
        return sequence_to_exp(cdr_cell(first));
      }
      else
      { // error ELSE clause isn't last -- COND->IF
        return &invalid_cell;
      }
    }
    else
    {
#if 0
      cout << "\ncar first\n";
      print_cell(car_cell(first));
      cout << "\ncdr first\n";
      print_cell(cdr_cell(first));
      cout << endl;
#endif
      make_if(car_cell(first), sequence_to_exp(cdr_cell(first)), expand_clauses(rest));
    }

  }
}

// for set!
bool set_variable_value(Cell *var, Cell *val, Environment * env)
{
#ifdef USE_CPP_MAP
  auto it = env->frame_.find(var->val_);
  #ifdef USE_MYMAP
  if (it != 0) // find it
  {
    it->second = val;
    return true;
  }
  #else
  if (it != env->frame_.end()) // find it
  {
    it->second = val;
    return true;
  }
  #endif
  else // not fount
  {
    if (env->outer_ != 0)
      return set_variable_value(var, val, env->outer_);
  }
#else
  EnvElement *env_e = find_variable(env, var->val_);

  if (env_e) // found
  {
    env_e->value_ = val;
  }
  else
  {
    if (env->outer_ != 0)
      return set_variable_value(var, val, env->outer_);
  }
#endif
  return false;
}

Cell *stop_timer_cell(Cell *exp, Environment *env)
{
#ifdef LINUX
  Cell *cell = car_cell(exp); // start-timer
  Cell *timer_name = car_cell(cdr_cell(exp)); // a-timer

  //cout << "stop timer: " << timer_name->val_ << endl;

  for (auto &i : timer_list)
  {
    if (strcmp(i->name().c_str(), timer_name->val_) == 0)
    {
      i->enable(false);
      i->reset_counter();
    }
  }
#endif
  return &true_cell;
}

Cell *start_timer_cell(Cell *exp, Environment *env)
{
#ifdef LINUX
  Cell *cell = car_cell(exp); // start-timer
  Cell *timer_name = car_cell(cdr_cell(exp)); // a-timer

  cout << "start timer: " << timer_name->val_ << endl;

  for (auto &i : timer_list)
  {
    if (strcmp(i->name().c_str(), timer_name->val_) == 0)
    {
      cout << "i->name(): " << i->name() << " enable" << endl;
      i->enable(true);
      cout << "i->is_enable: " << i->is_enable() << endl;
    }
  }
#endif
  return &true_cell;
}

Cell *get_timer_cell(Cell *exp, Environment *env)
{
#ifdef LINUX
  Cell *cell = car_cell(exp);
  Cell *timer_name = car_cell(cdr_cell(exp));
  Cell *interval = car_cell(cdr_cell(cdr_cell(exp)));
  Cell *call_back_func = car_cell(cdr_cell(cdr_cell(cdr_cell(exp))));

  cout << "timer_name: " << timer_name->val_ << endl;
  cout << "interval: " << interval->val_ << endl;

  Cell *eval_cell = eval(call_back_func, env);

  Timer *timer = new Timer(timer_name->val_, atoi(interval->val_), eval_cell);

  //timer_cell = get_cell(timer_name->val_, TIMER);
  //timer_cell->timer_ = eval_cell;

  timer_list.push_back(timer);

#if 0
  Cell *ret = apply(timer_cell->timer_, &null_cell);
  cout << "ret in get_timer_cell: " << ret->val_ << endl;
  ret = apply(timer_cell->timer_, &null_cell);
  cout << "ret in get_timer_cell: " << ret->val_ << endl;
  ret = apply(timer_cell->timer_, &null_cell);
  cout << "ret in get_timer_cell: " << ret->val_ << endl;
#endif
  //return timer_cell;
#endif
  return &true_cell;
}

Cell *eval_assignment(Cell *exp, Environment *env)
{
  // (set! a 9)
  if (set_variable_value(car_cell(cdr_cell(exp)), eval(car_cell(cdr_cell(cdr_cell(exp))), env), env))
    return &true_cell;
  else
    return &false_cell;
}

Cell *eval(Cell *exp, Environment *env)
{
#if 0
  cout << "\nexp: !!!\n";
  print_cell(exp);
  cout << "\n!!!\n";

  cout << "cell length:" << length_cell(exp) << endl;

  exit(0);
  cout << "\ncar exp: !!!\n";
  print_cell(car_cell(exp));
  cout << "\n!!!\n";

  cout << "\ncdr exp: !!!\n";
  print_cell(cdr_cell(exp));
  cout << "\n!!!\n";
#endif

#if 0
  self-evaluating
  variable       //? exp) (lookup-variable-value exp env))
  ((application? exp)
#endif

  switch (exp->type_)
  {
    //case SelfEval: // number or string
    case NUMBER: 
    case STRING: 
    {
      //cout << "in exp:" << exp.val << endl;
      return exp;
      break;
    }
    //case Variable: // symbol
    case SYMBOL: // symbol
    {
      //cout << "SYMBOL:" << exp->val_ << endl;
      //return env->frame_[exp->val_];
      return lookup_variable_value(exp, env);
      //return get_cell("primitive add", proc_add);
      // lookup environment
      //return func;
      break;
    }
    case PAIR:
    {
      //cout << "is pair" << endl;
      if (tagged_list(exp, "lambda"))
      {
#if 0
        cout << "lambda expression" << endl;
        print_cell(exp);
        cout << endl;
#endif
          Cell *parameters = car_cell(cdr_cell(exp));
          Cell *body = cdr_cell(cdr_cell(exp));

#if 0
          cout << "parameters: " << endl;
          print_cell(parameters);
          cout << endl;
          cout << "body: " << endl;
          print_cell(body);
          cout << endl;
#endif
          return make_procedure(parameters, body, env);
      }
      else if (tagged_list(exp, "define"))
           {
#if 0
             cout << "define expression" << endl;
             print_cell(exp);
             cout << endl;
#endif
             return eval_definition(exp, env);
           }
           else if (tagged_list(exp, "if"))
                {
                  //cout << "if expression" << endl;
                  return eval_if(exp, env);
                }
                else if (tagged_list(exp, "begin"))
                     {
                       return eval_sequence(cdr_cell(exp), env);
                     } 
                     else if (tagged_list(exp, "cond"))
                          {
                             return eval(expand_clauses(cdr_cell(exp)), env);
                          }
                          else if (tagged_list(exp, "quote"))
                               {
                                 Cell *result = car_cell(cdr_cell(exp));

                                 if (result->type_ != PAIR)
                                 {
                                   strcpy(invalid_cell.val_, "quote fail");
                                   //strcpy(invalid_cell.val_, "quote argument should be list. ex: (quote (1 2 3))");
                                   return &invalid_cell;
                                 }
                                 else
                                   return result;
                               }
                               else if (tagged_list(exp, "set!"))
                                    {
                                      return eval_assignment(exp, env);
                                    }
                                    else if (tagged_list(exp, "get-timer"))
                                         {
                                           return get_timer_cell(exp, env);
                                         }
                                         else if (tagged_list(exp, "start-timer"))
                                              {
                                                return start_timer_cell(exp, env);
                                              }
                                              else if (tagged_list(exp, "stop-timer"))
                                                   {
                                                     return stop_timer_cell(exp, env);
                                                   }

#if 0
      if (exp->type_ == SYMBOL)
      {
        // (lambda (x) (+ x 4))
        // ((lambda (x) (+ x 4)) 5)
        if (strcmp(car_cell(exp)->val_, "lambda") == 0)
        {
          cout << "lambda expression" << endl;
          //exit(0);
        }
        else if (strcmp(car_cell(exp)->val_, "define") == 0) // (define (plus4 y) (+ y 4))
             {
               cout << "define expression" << endl;
               exit(0);
             }
      }
#endif
      // application
      {
        // need check exp.list.size() >= 2
        return apply(eval(car_cell(exp), env), list_of_values(cdr_cell(exp), env));
      }
      //apply(eval(exp.list[0]));
      #if 0
      cell cur = exp.list[0];
      return apply(eval(cur));
      #endif
      //cout << "after apply" << endl;
      //return exp;
      break;
    }
  }


}

#if 0
// convert given cell to a Lisp-readable string
std::string to_string(const Cell & exp)
{
    if (exp.type == List) {
        std::string s("(");
        for (cell::iter e = exp.list.begin(); e != exp.list.end(); ++e)
            s += to_string(*e) + ' ';
        if (s[s.size() - 1] == ' ')
            s.erase(s.size() - 1);
        return s + ')';
    }
    else if (exp.type == Lambda)
        return "<Lambda>";
    else if (exp.type == Proc)
        return "<Proc>";
    return exp.val;
}
#endif

//enum {BEGIN, SPACE, WORD, END};

void do_eval(TC &tc, Environment * env)
{
  cout << "tc.size(): " << tc.size() << endl;

#if 1
    previous_free_pair_index = free_pair_index;
    previous_free_cell_index = free_cell_index;

    previous_free_env_index = free_env_index;
#endif
    Cell *exp = read(tc);
    if (exp->type_ == INVALID) // no input string
    {
      cout << "parse expression fail" << endl;
      return;
    }
    if (exp == &null_cell)
      return;
    exp = eval(exp, env);
    if (exp == &define_cell)
    {
      cout << "define: ok" << endl;
    }
    else if (exp->type_ != INVALID)
         {
           cout << "result:" << endl;
           print_cell(exp);
           cout << endl;
           #ifndef X86_16
           if (exp->env_ != 0)
             print_env(exp->env_, 0);
           #endif
#ifdef FREE_CELL
           free_pair_index = previous_free_pair_index;
           free_cell_index = previous_free_cell_index;

           free_env_index = previous_free_env_index;
#endif
         }
         else
         {
           cout << "expression fail" << endl << "error message: " << invalid_cell.val_ << endl;
#ifdef FREE_CELL
           free_pair_index = previous_free_pair_index;
           free_cell_index = previous_free_cell_index;

           free_env_index = previous_free_env_index;
#endif
         }
}

#ifdef OS_CPP
// the default read-eval-print-loop
void repl(const char *prompt, Environment *env)
{
  for (;;) 
  {
    cout << prompt;

    TC tc;
    int parenthesis_count=0;

    //myprint("before free_pair_index: %d\n", free_pair_index);
    //myprint("before free_cell_index: %d\n", free_cell_index);
    while(1)
    {
      // get_byte
      //std::string line; 
      //std::getline(std::cin, line);
      char line[LINE_SIZE];
      int i=0;
      int state;

      while(i < LINE_SIZE)
      {
        int ch;

        ch = getchar();
        switch (ch)
        {
          case '(':
          {
            ++parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          case ')':
          {
            --parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          #if 0
          case EOF:
          {
            exit(1);
          }
          #endif
          case ' ':
          {
            while(1)
            {
              ch=getchar();
              if (ch != ' ')
              {
                i=0;
                #ifdef OS_CPP
                ungetc(ch, stdin);
                #else
                ungetch(ch);
                #endif
                break;
              }
            }
            break;
          }
          case ENTER:
          {
            //cout << "\\n" << endl;;
            //myprint("\r\nenter\r\n");
            goto end_line;
            break;
          }
          default:
          {
            line[i++] = ch;
            while(1)
            {
              ch=getchar();

              if (ch == ' ' || ch == ')' || ch == '(' || ch == ENTER)
              {
                //if (ch == ')')
                //if (ch == '(')
                #ifdef OS_CPP
                ungetc(ch, stdin);
                #else
                ungetch(ch);
                #endif
                line[i] = 0;
                i = 0;
                break;
              }
              else
              {
                line[i++] = ch;
              }
            }
          }
        }

        if (line[0] != ' ' && line[0] != 0)
        {
          //cout << line << endl;
          tc.push_back(line);
          line[0] = 0;
        }

      }

end_line:


#ifdef OS_CPP
      cout << "parenthesis_count: " << parenthesis_count << endl;
#else
      myprint("parenthesis_count: "); 
      myprint(parenthesis_count);
      myprint("\r\n");
#endif
        if (parenthesis_count == 0)
          break;

      //if (0 >= tokenize(line, tokens))
        //break;
    }
    //parenthesis_count=0;


    do_eval(tc, env);

  }
}

// readline 
void rl_repl(const char *prompt, Environment *env)
{
#ifdef RL
  for (;;) 
  {
    TC tc;
    int parenthesis_count=0;

    //myprint("before free_pair_index: %d\n", free_pair_index);
    //myprint("before free_cell_index: %d\n", free_cell_index);
    while(1)
    {
      // get_byte
      //std::string line; 
      //std::getline(std::cin, line);
      char line[LINE_SIZE];
      char *input_line = readline(prompt);

      add_history(input_line);

      int i=0;
      int state;
      int idx=0;

      while(idx < strlen(input_line))
      {
        int ch;

        ch = input_line[idx++];
        switch (ch)
        {
          case '(':
          {
            ++parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          case ')':
          {
            --parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          #if 0
          case EOF:
          {
            exit(1);
          }
          #endif
          case ' ':
          {
            while(1)
            {
              ch=input_line[idx++];
              if (ch != ' ')
              {
                i=0;
                --idx;
                break;
              }
            }
            break;
          }
          case ENTER:
          {
            //cout << "\\n" << endl;;
            //myprint("\r\nenter\r\n");
            goto end_line;
            break;
          }
          default:
          {
            line[i++] = ch;
            while(1)
            {
              ch=input_line[idx++];

              if (ch == ' ' || ch == ')' || ch == '(' || ch == ENTER || ch == '\0')
              {
                --idx;
                line[i] = 0;
                i = 0;
                break;
              }
              else
              {
                line[i++] = ch;
              }
            }
          }
        }

        if (line[0] != ' ' && line[0] != 0)
        {
          //cout << line << endl;
          tc.push_back(line);
          line[0] = 0;
        }

      }

end_line:
      free(input_line);


#ifdef OS_CPP
      cout << "parenthesis_count: " << parenthesis_count << endl;
#else
      myprint("parenthesis_count: "); 
      myprint(parenthesis_count);
      myprint("\r\n");
#endif
        if (parenthesis_count == 0)
          break;

      //if (0 >= tokenize(line, tokens))
        //break;
    }
    //parenthesis_count=0;


    do_eval(tc, env);

  }
#endif
}
#endif // end #ifdef OS_CPP

//extern DS::Deque<int, 128> line_buf;

// convert given string to list of tokens
int tokenize(const char *s, TokenContainer &tokens, int &parenthesis_count)
{
  //cout << "org str: " << str << endl;
  //const char * s = str.c_str();
  while (*s) 
  {
    while (*s == ' ')
      ++s;
    if (*s == '(' || *s == ')')
    {
      if (*s == '(')
        ++parenthesis_count;
      if (*s == ')')
        --parenthesis_count;
      tokens.push_back(*s++ == '(' ? "(" : ")");
    }
    else 
    {
      const char * t = s;
      while (*t && *t != ' ' && *t != '(' && *t != ')')
      {
        if (*s == '(')
          ++parenthesis_count;
        if (*s == ')')
          --parenthesis_count;
        ++t;
      }
      char substr[STRING_SIZE];
      s_strncpy(substr, s, t-s);
      tokens.push_back(substr);
      //tokens.push_back(std::string(s, t));
      //cout << "str: " << s << endl;
      s = t;
    }
  }
  //cout << "parenthesis_count:" << parenthesis_count << endl;

  return parenthesis_count;
}

#ifndef OS_CPP
void non_os_repl(const char *prompt, Environment *env)
{
  DS::Deque<int, 128> line_buf;
  //line_buf.init();

  DS::Deque<DS::CString> deque;

  //deque.init(); // history buffer

  for (;;) 
  {
#ifndef OS_CPP
    cout << prompt;
    int up_index=0;
    int down_index=0;
    DS::CString ps;

    ps = "";

    int parenthesis_count=0;
    TokenContainer tc;

#ifdef UEFI
    while(1)
    {
      // get_byte
      //std::string line; 
      //std::getline(std::cin, line);
      char line[LINE_SIZE];
      int i=0;
      int state;

      while(i < LINE_SIZE)
      {
        int ch;

        ch = getchar();
        switch (ch)
        {
          case '(':
          {
            ++parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          case ')':
          {
            --parenthesis_count;
            line[0] = ch;
            line[1] = 0;
            i=0;
            break;
          }
          #if 0
          case EOF:
          {
            exit(1);
          }
          #endif
          case ' ':
          {
            while(1)
            {
              ch=getchar();
              if (ch != ' ')
              {
                i=0;
                #ifdef OS_CPP
                ungetc(ch, stdin);
                #else
                ungetch(ch);
                #endif
                break;
              }
            }
            break;
          }
          case ENTER:
          {
            //cout << "\\n" << endl;;
            //myprint("\r\nenter\r\n");
            goto end_line;
            break;
          }
          default:
          {
            line[i++] = ch;
            while(1)
            {
              ch=getchar();

              if (ch == ' ' || ch == ')' || ch == '(' || ch == ENTER)
              {
                //if (ch == ')')
                //if (ch == '(')
                #ifdef OS_CPP
                ungetc(ch, stdin);
                #else
                ungetch(ch);
                #endif
                line[i] = 0;
                i = 0;
                break;
              }
              else
              {
                line[i++] = ch;
              }
            }
          }
        }

        if (line[0] != ' ' && line[0] != 0)
        {
          //cout << line << endl;
          tc.push_back(line);
          line[0] = 0;
        }

      }

end_line:


      cout << "parenthesis_count: " << parenthesis_count << endl;
        if (parenthesis_count == 0)
          break;

      //if (0 >= tokenize(line, tokens))
        //break;
    }

#else // non os
    while(1)
    {
      // get_byte
      //std::string line; 
      //std::getline(std::cin, line);
      char line[LINE_SIZE];
      int i=0;
      int state;
      int ch;

      while(i < LINE_SIZE)
      {
        ch = DS::getch();
        switch (ch)
        {
          case LEFT_KEY:
          {
            break;
          }
          case RIGHT_KEY:
          {
            break;
          }
          case UP_KEY:
          {
            //ps.init(line);
            if (deque.back(up_index, ps) == true)
            {
              int lb_size = line_buf.size();
              int ps_size = ps.length();

              go_left(line_buf.size());

              myprint(ps.c_str());
              if (ps_size < lb_size)
              {
                int str_diff = lb_size - ps_size;

                for (int i=0 ; i < str_diff ; ++i)
                  putchar(' '); 
                go_left(str_diff);
              }

              line_buf.clear();
              const char *c_str = ps.c_str();
              // copy to line buffer
              for (int i=0 ; i < ps.length() ; ++i)
              {
                int c = c_str[i];
                line_buf.push_back(c);
              }
              ++up_index;
            }
            break;
          }
          case DOWN_KEY:
          {
            if (deque.back(up_index-2, ps) == true)
            {
              int lb_size = line_buf.size();
              int ps_size = ps.length();

              go_left(line_buf.size());

              myprint(ps.c_str());
              if (ps_size < lb_size)
              {
                int str_diff = lb_size - ps_size;

                for (int i=0 ; i < str_diff ; ++i)
                  putchar(' ');
                go_left(str_diff);
              }

              line_buf.clear();
              const char *c_str = ps.c_str();
              // copy to line buffer
              for (int i=0 ; i < ps.length() ; ++i)
              {
                int c = c_str[i];
                line_buf.push_back(c);
              }
              --up_index;
            }
            else
            {
              if (ps.length() == 0) break;

              int ps_size = ps.length();
              go_left(ps_size);
              for (int i=0 ; i < ps_size ; ++i)
                putchar(' '); 

              go_left(ps_size);
              line_buf.clear();
              ps="";
            }

            break;
          }
          case ENTER:
          {
            //cout << "\\n" << endl;;
            //myprint("\r\nenter\r\n");
            up_index = 0;
            goto end_line;
            break;
          }
          case BACKSPACE:
          {
            if (!line_buf.empty())
            {
              putchar(8);
              putchar(' '); // fixed me: if use cout << ' ', use backspace will got wrong behavior
              putchar(8);
              line_buf.pop_back();
            }
            break;
          }
          default:
          {
            //line[i++] = ch;
            line_buf.push_back(ch);
            break;
          }
        }
      }
end_line:

      int l = 0;
      int c;
      int line_buf_len = line_buf.size();
      for ( ; l < line_buf_len ; ++l)
      {
        line_buf.pop_front(c);
        line[l] = c;
      }
      line[l] = 0;
      ps.init(line);
      deque.push_back(ps);

#if 0
      myprint("\r\ninput str: ");
      myprint(ps.c_str());
      myprint("\r\n");
#endif
      // parse input string
      tokenize(line, tc, parenthesis_count);


      if (parenthesis_count == 0 )
        break;
    }  // end while(1)
#endif
    do_eval(tc, env);

#endif
  } // end for (;;) 
}
#endif

int init_eval()
{
#if 1
  invalid_cell.type_ = INVALID;
  null_cell.type_ = NULL_CELL;
  lambda_cell.type_ = SYMBOL;
  strcpy(lambda_cell.val_, "lambda");
  define_cell.type_ = SYMBOL;

  true_cell.type_ = SYMBOL;
  strcpy(true_cell.val_, "#t");
  false_cell.type_ = SYMBOL;
  strcpy(false_cell.val_, "#f");

  begin_cell.type_ = SYMBOL;
  strcpy(begin_cell.val_, "begin");

  if_cell.type_ = SYMBOL;
  strcpy(if_cell.val_, "if");

  //Environment *global_env = get_env(0, "global");

  //create_primitive_procedure(global_env);
  //repl("simple scheme> ", global_env);
#endif

#if 0
  TokenContainer tc;

  tc.push_back("abc");
  tc.push_back("xyz");
  tc.print();
  const char *str = tc.front();
  cout << str << endl;
  tc.pop_front();
  tc.print();
#endif
  return 0;
}

