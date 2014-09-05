#include <iterator>     // std::back_inserter
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <cstdlib>


using namespace std;

#include "cell.h"

// return given mumber as a string
std::string str(long n) { std::ostringstream os; os << n; return os.str(); }

// return true iff given character is '0'..'9'
bool isdig(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }

Cell invalid_cell;
Cell null_cell;

struct Environment;

#if 0
const Cell false_sym(Symbol, "#f");
const Cell true_sym(Symbol, "#t"); // anything that isn't false_sym is true
const Cell nil(Symbol, "nil");
#endif

////////////////////// environment
//

typedef std::map<std::string, Cell*> Frame;
struct Environment 
{
  public:
    Environment(): outer_(0) {}
    Environment *outer_;

    Frame frame_;
  private:
};


void extend_environment(const Cell &vars, const Cell &vals, Environment *env)
{
#if 0
  cout << "-----\n";
  print_cell(vars);
  cout << "\n";
  print_cell(vals);
  cout << "\n-----\n";

  Cell para = car_cell(vars);
  Cell arg = car_cell(vals);

  cout << "ext env: \n";
  print_cell(arg);
  cout << "\n-----\n";
#endif
  // need check vars.list.size() == vals.list.size()
  // vars/vals is a List
  //for (int i = 0 ; i < vars.list.size() ; ++i)
    //env->frame_.insert(Frame::value_type(vars.list[i].val, vals.list[i]));
}

const Cell& lookup_variable_value(const Cell &exp, const Environment *env)
{
#if 0
  Frame::const_iterator it = env->frame_.find(exp.val);
  if (it != env->frame_.end()) // find it
  {
    cout << "found it" << endl;
    return (*it).second;
  }
  else
  {
    if (env->outer_ != 0)
      return lookup_variable_value(exp, env->outer_);
  }
  cout << "not found it" << endl;
  return invalid_cell;
#endif
}

Cell *eval(Cell *exp, Environment *env);


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

int add_cell(Cell *c)
{
  Cell *cell = car_cell(c);

  int sum =0 ;
  if (cell->type_ == PAIR)
    return add_cell(cdr_cell(c));
  else if (cell->type_ != NULL_CELL)
       {
         cout << cell->val_ << endl;
         return sum += atol(cell->val_);
       }
       else
         return 0;
}

Cell *proc_add(Cell *cell)
{

  int sum=0;
  Cell *c = car_cell(cell);
  Cell *rest=cell;

  while (rest->type_ != NULL_CELL)
  {
    cout << "\n%%%\n";
    print_cell(rest);
    cout << "\n%%%\n";
    sum += atoi(car_cell(rest)->val_);
    rest = cdr_cell(rest);
  }
  cout << "sum: " << sum << endl;
  char str[20];
  return get_cell(s32_itoa_s(sum, str, 10), NUMBER);
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
  cout << "product: " << product << endl;
  char str[20];
  return get_cell(s32_itoa_s(product, str, 10), NUMBER);
}
#if 0

int sub_cell(const Cell &c)
{
  switch (c.kind())
  {
    case List: 
    {
      int result=0;
      if (c.list.size() >= 1)
        result = sub_cell(c.list[0]);

      for (int i=1 ; i < c.list.size() ; ++i)
        result -= sub_cell(c.list[i]);
      return result;
      break;
    }
    case Number:
    {
      int num = atol(c.val.c_str() );
      cout << "num: " << num << endl;
      return num;
      break;
    }
    default:
    {
      break;
    }
  }
}

Cell proc_sub(const Cell &c)
{
  int result = sub_cell(c);
  cout << "result: " << result << endl;
  return Cell(Number, str(result));
}

int mul_cell(const Cell &c)
{
  switch (c.kind())
  {
    case List: 
    {
      int product=1;

      for (int i=0 ; i < c.list.size() ; ++i)
        product *= mul_cell(c.list[i]);
      return product;
      break;
    }
    case Number:
    {
      int num = atol(c.val.c_str() );
      cout << "num: " << num << endl;
      return num;
      break;
    }
    default:
    {
      break;
    }
  }
}

#endif

void create_primitive_procedure(Frame &frame)
{
  Cell *op = get_cell("primitive add", proc_add);
  frame.insert(Frame::value_type("+", op));

  op = get_cell("primitive mul", proc_mul);
  frame.insert(Frame::value_type("*", op));
#if 0
  frame.insert(Frame::value_type("-", Cell(proc_sub, "primitive sub")));
  frame.insert(Frame::value_type("cons", Cell(proc_cons, "primitive cons")));
  frame.insert(Frame::value_type("car", Cell(car_cell, "primitive car")));
  frame.insert(Frame::value_type("cdr", Cell(cdr_cell, "primitive cdr")));
  Cell x(Symbol, "5");
  frame.insert(Frame::value_type("x", x));
#endif
}

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

// convert given string to list of tokens
std::list<std::string> tokenize(const std::string & str)
{
    std::list<std::string> tokens;
    const char * s = str.c_str();
    while (*s) {
        while (*s == ' ')
            ++s;
        if (*s == '(' || *s == ')')
            tokens.push_back(*s++ == '(' ? "(" : ")");
        else {
            const char * t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')')
                ++t;
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    return tokens;
}

#if 0
// numbers become Numbers; every other token is a Symbol
Cell atom(const std::string & token)
{
    if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
        return Cell(Number, token);
    return Cell(Symbol, token);
}
#endif

// return the Lisp expression in the given tokens
Cell *read_from(std::list<std::string> & tokens)
{
  const std::string token(tokens.front());
  //char *token = tokens;
  tokens.pop_front();
  if (token == "(") 
  {
    vector<Cell *> cells;
    while (tokens.front() != ")")
    {
      cells.push_back(read_from(tokens));
    }
    tokens.pop_front();
    return make_list(cells);
  }
  else
  {
    Cell *cell;
    if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
      cell = get_cell(token.c_str(), NUMBER);
    else
      cell = get_cell(token.c_str(), SYMBOL);
    cout << "cell val_: " << cell->val_ << endl;
    return cell;
  }
}

// return the Lisp expression represented by the given string
Cell *read(const std::string & s)
{
    std::list<std::string> tokens(tokenize(s));
    return read_from(tokens);
}

Cell *list_of_values(Cell *exp, Environment *env)
{
  Cell *first_operand = car_cell(exp);
  Cell *rest_operands = cdr_cell(exp);
  cout << "list_of_values" << endl;
  if (exp->type_ == NULL_CELL)
  {
    cout << "i am null" << endl;
    return &null_cell;
  }
  else
    return  cons_cell(eval(first_operand, env), list_of_values(rest_operands, env));
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

Cell eval_sequence(const Cell &exp, Environment *env)
{
#if 0
  cout << "eval_sequence: " << exp.kind_str() << endl;
  cout << endl;
  print_cell(exp);
  cout << endl;

  for (int i=0 ; i < exp.list.size()-1 ; ++i)
    eval(exp.list[i], env);
  return eval(exp.list[exp.list.size()-1], env);
#endif
}

Cell *apply(Cell *func, Cell *args)
{
  //Cell *eval(const Cell &exp, Environment *env);
  cout << "apply:" << func->type_str() << endl;
  cout << endl;
  return func->proc_(args);
  //print_cell(func);
  //cout << endl;


#if 0
  Cell args_list(List);
  if (args.kind() != List)
  {
    args_list.list.push_back(args);
    cout << "args_list:" << args_list.kind_str() << endl;
    print_cell(args_list);
    cout << endl;
  }
  else
  {
    cout << "args:" << args.kind_str() << endl;
    print_cell(args);
    cout << endl;
  }




  switch (func.proc_kind())
  {
    case LAMBDA:
    {
      // new a Environment
      // add parameters and arguments pair

      Cell body(List); 
      body = cdr_cell(func);

      Cell parameters(List); 
      parameters = car_cell(func);
      #if 1
      cout << "body: " << endl;
      print_cell(body);
      cout << endl;

      cout << "para: " << parameters.kind_str() << endl;
      print_cell(parameters);
      cout << endl;
      #endif

      
      Environment env;
      env.outer_ = func.env_;
      if (args.kind() != List)
        extend_environment(parameters, args_list, &env);
      else
        extend_environment(parameters, args, &env);
      return eval_sequence(body, &env);
    }
    case PRIMITIVE:
    {
      cout << "func name:" << func.val << endl;
      if (args.kind() != List)
        return func.proc_(args_list);
      else
        return func.proc_(args);
    }
  }

  return invalid_cell;
#endif
}

Cell make_procedure(const Cell &parameters, const Cell &body, Environment *env)
{
#if 0
  Cell lambda_proc(List);

  lambda_proc = cons_cell(parameters, body);
  lambda_proc.proc_kind_ = LAMBDA;
  lambda_proc.env_ = env;

  return lambda_proc;
#endif
}

Cell *eval(Cell *exp, Environment *env)
{
#if 0
  cout << "\nexp: !!!\n";
  print_cell(exp);
  cout << "\n!!!\n";

  cout << "\ncar exp: !!!\n";
  print_cell(car_cell(exp));
  cout << "\n!!!\n";

  cout << "\ncdr exp: !!!\n";
  print_cell(cdr_cell(exp));
  cout << "\n!!!\n";
  exit(0);
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
      cout << "SYMBOL:" << exp->val_ << endl;
      return env->frame_[exp->val_];
      //return get_cell("primitive add", proc_add);
      // lookup environment
      //Cell func = lookup_variable_value(exp, env);
      //return func;
      break;
    }
    case PAIR:
    {
      cout << "is pair" << endl;
#if 0
      if (exp.list[0].kind() == Symbol)
      {
        // (lambda (x) (+ x 4))
        // ((lambda (x) (+ x 4)) 5)
        if (exp.list[0].val == "lambda")
        {
          cout << "lambda expression" << endl;
          Cell parameters = car_cell(cdr_cell(exp));
          Cell body = cdr_cell(cdr_cell(exp));

          cout << "parameters: " << parameters.kind_str() << endl;
          print_cell(parameters);
          cout << endl;
          cout << "body: " << body.kind_str() << endl;
          print_cell(body);
          cout << endl;

          return make_procedure(parameters, body, env);
          //exit(0);
        }
        else if (exp.list[0].val == "define") // (define (plus4 y) (+ y 4))
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

// the default read-eval-print-loop
void repl(const std::string & prompt, Environment *env)
{
    for (;;) 
    {
        std::cout << prompt;
        std::string line; std::getline(std::cin, line);
        Cell *exp = read(line);


        cout << endl;
        print_cell(exp);
        cout << endl;
        exp = eval(exp, env);
        cout << "result:" << endl;

        if (exp->type_ != INVALID)
        {
          print_cell(exp);
          cout << endl;
        }
        else
        {
          cout << "expression fail!" << endl;
        }
        break;
    }
}

int main ()
{
  invalid_cell.type_ = INVALID;
  null_cell.type_ = NULL_CELL;

  Environment global_env; //add_globals(global_env);
  create_primitive_procedure(global_env.frame_);
  repl("90> ", &global_env);
}

