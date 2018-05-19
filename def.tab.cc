/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "def.yy" /* yacc.c:339  */

#include <string.h>
#include <stdio.h>
#include <stack>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
extern "C" int yylex();
extern "C" int yyerror(const char *msg, ...);
extern FILE* yyin;
extern FILE* yyout;
using namespace std;
int results=0, floats=0, labels=0;
stack <string> stos;
stack <string> types_stos;
stack <string> labels_stos;
stack <string> loop_stos;
stack <string> table_stos;
struct ids_val{
	int size;
	string type;
	string id;
	ids_val(int a, string b, string c):size(a),type(b),id(c){};
};
class out{
	vector<string> dataBlock;
	vector<string> textBlock;
	ofstream fileStream;
public:
	out(string file): fileStream(file.c_str()){
	}
	out(char* file): fileStream(file){
	}
	void add(string obj){
	    this->textBlock.push_back(obj);
	}
	void addData(string obj){
	    this->dataBlock.push_back(obj);
	}
	void save(){
		fileStream<<".data \n";
		for(auto text : dataBlock){
			(fileStream)<<text.c_str();
		}
		fileStream<<"\n";
		fileStream<<".text \n";
		for(auto text : textBlock){
			(fileStream)<<text.c_str();
		}
	}
};
// DATA
map <string, ids_val> ids;
out file_out(string("out.asm"));
// FUNCTIONS
constexpr unsigned int str2int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}
string determineAction(string type, string variable){
	auto var = ids.find(variable);
	if(var!=ids.end()){
		return "w";
	}
	switch(str2int(type.c_str())){
		case str2int("int"):
			return "i";
		case str2int("flt"):
			return ".s";
		case str2int("var"):
			return "w";
	}
}
string determineArrType(string arrType){
	if(arrType == "arrflt")
		return "flt";
	if(arrType == "arrint")
		return "int";
}
namespace utils{
	string getNextTemp(){
		return " __temp"+to_string(results++);
	}
	string getNextLabel(){
		return "__lbl"+to_string(labels++);
	}
}
namespace asembler{
	void write_asm_label(string label){
		stringstream ss;
		ss<<label<<": nop\n";
		file_out.add(ss.str());
	}
	void write_asm_jump(string jump, string type, string value, string label){
		stringstream ss;
		if(jump == "j"){
			ss<<jump<<" "<<label<<"\n";
			file_out.add(ss.str());
			return;
		}
		const string registerNumber = "7";
		string registerName, variableReg;
		if(type == "int"){
			registerName = "$t";
			variableReg = "$t0";
			ss<<"li "<<registerName<<registerNumber<<", 0"<<"\n";
			ss<<"lw "<<variableReg<<", "<<value<<"\n";
		} else {
			registerName = "$f";
			variableReg = "$f0";
			ss<<"l.s "<<registerName<<registerNumber<<", 0.0"<<"\n";
			ss<<"l.s "<<variableReg<<", "<<value<<"\n";
		}
		ss<<jump<<" "<<registerName<<registerNumber<<", "<<variableReg<<", "<<label<<"\n";
		file_out.add(ss.str());
	}
	void write_asm_break_line(){
		stringstream ss;
		ss<<"addi $a0, $0, 0xA \n"
			<<"addi $v0, $0, 0xB \n"
			<<"syscall\n";
		file_out.add(ss.str());
	}
	string asm_type(string type, string variable){
		if(type=="int")
			return ".word";
		if(type=="flt")
			return ".float";
		if(type=="str")
			return ".asciiz";
		if(type=="arrint")
			return ".word";
		if(type=="arrflt")
			return ".float";
		try{
			return asm_type(ids.at(variable).type, variable);
		} catch (const std::out_of_range& oor){
			std::cout << "Type is errorneus, returning integer type. Type: "<<type << " Variable: "<< variable  << '\n';
			return ".word";
		}
	}
	void write_asm_var(string var, string type, string data){
		stringstream ss;
		auto id = ids.find(var);
		if(id!= ids.end())
			type = id->second.type;
		ss<<var+":	"+asm_type(type, var)+" "+data+"\n";
		file_out.addData(ss.str());
	}
	void write_asm_print(string variable, string type){
		string reg, value;
		stringstream ss;
		if(type=="int"){
			reg = "$a0";
			value="1";
			ss<<"l"<<determineAction(type, variable)<<" "+reg+", "+variable+"\n";
		} else if(type=="flt"){
			reg = "$f12";
			value="2";
			ss<<"l.s "+reg+", "+variable+"\n";
		} else if(type=="str"){
			reg = "$a0";
			value = "4";
			ss<<"la $a0 , "+variable+"\n";
		}
		ss<<"li $v0, "+value+"\n";
		ss<<"syscall\n";
		file_out.add(ss.str());
		write_asm_break_line();
	}
	void write_asm_read(string variable, string type){
		string reg, value, operation;
		stringstream ss;
		if(type=="int"){
			reg = "$a0";
			value="5";
			operation = "sw";
		} else if(type=="flt"){
			reg = "$f0";
			value="6";
			operation = "s.s";
		}
		ss<<"li $v0, "+value+"\n";
		ss<<"syscall\n";
		ss<<operation<<" "<<(type=="int"?"$v0":reg)<<", "<<variable<<"\n";
		file_out.add(ss.str());
	}
	void write_asm(string x, string y, string action, string out, string x_type, string y_type){
		stringstream ss;
		ss<<"l"<<determineAction(x_type, x)<<" $t0, "+x+" \n";
		ss<<"l"<<determineAction(y_type, y)<<" $t1, "+y+" \n";
		if(action != "cha"){
		ss<<action+" $t0, $t0, $t1 \n";
		ss<<"sw $t0, "+out+"\n";
		} else {
		ss<<"sw $t1, "+out+"\n";
		}
		file_out.add(ss.str());
	}
	void write_asm_floating(string x, string y, string action, string out, string x_type, string y_type){
		stringstream ss;
		if(action!="cha")
			ss<<"l.s"<<" $f0, "+x+" \n";
		ss<<"l.s"<<" $f1, "+y+" \n";
		if(action != "cha"){
		ss<<action+".s $f0, $f0, $f1 \n";
		ss<<"s.s $f0, "+out+"\n";
		} else {
		ss<<"s.s $f1, "+out+"\n";
		}
		file_out.add(ss.str());
	}
	void write_asm_int_to_flt(string x, string out){
		stringstream ss;
		ss<<"#konwersja int na flt\n";
		ss<<"l"<<determineAction("int", x)<<" $t0, "<<x <<"\n";
		ss<<"mtc1 $t0 , $f1"<<"\n";
		ss<<"cvt.s.w $f1, $f1"<<"\n";
		ss<<"s.s $f1, "<<out<<"\n";
		file_out.add(ss.str());
	}
	void write_asm_flt_to_int(string x, string out){
		stringstream ss;
		ss<<"#konwersja flt na int\n";
		ss<<"l.s"<<" $f0, "<<x <<"\n";
		ss<<"cvt.w.s $f0, $f0"<<"\n";
		ss<<"mfc1 $t0, $f0"<<"\n";
		ss<<"sw $t0,"<<out<<"\n";
		file_out.add(ss.str());
	}
	void write_asm_table_assign(string table, string position, string valueType, string value){
		stringstream ss;
		ss<<"l"<<determineAction(valueType, value)<<" $t0,"<<value<<"\n";
		ss<<"la $t4, "<<table<<"\n";
		ss<<"li $t5, "<<position<<"\n";
		ss<<"mul $t5, $t5, 4\n";
		ss<<"add $t4, $t4, $t5\n";
		ss<<"sw $t0, ($t4)\n";
		file_out.add(ss.str());
	}
	void write_asm_table_get_value(string table, string position, string variable){
		stringstream ss;
		ss<<"#wyciaganie z tablicy{\n";
		ss<<"la $t4, "<<table<<"\n";
		ss<<"li $t5, "<<position<<"\n";
		ss<<"mul $t5, $t5, 4\n";
		ss<<"add $t4, $t4, $t5\n";
		ss<<"lw $t0, ( $t4 )\n";
		ss<<"sw $t0,"<<variable<<"\n";
		ss<<"#}\n";
		file_out.add(ss.str());
	}
}
void add_id(string id,string type,string data = "0"){
	if(ids.find(id) != ids.end())
		return;
	std::cout << ">> Added id: "<<id<< " : "<<type << '\n';
	asembler::write_asm_var(id, type, data);
	ids.insert(pair<string, ids_val>(id, ids_val(1,type,id)));
}
void save_ids(){
	for(auto idpair : ids){

	}
	file_out.save();
}
string determineActionMark(char mark){
	switch(mark){
		case '*': return "mul";
		case '+': return "add";
		case '-': return "sub";
		case '/': return "div";
		case '=': return "cha";
	}
}
namespace memoryStack{
	void pullFromStack(string* var, string* type){
		*var = stos.top();
		*type = types_stos.top();
		stos.pop();
		types_stos.pop();
		std::cout << "\\" << stos.size() <<"/ Pulling variable "<< *var << " : " << *type << " from stack." << '\n';
	}
	void pushToStack(string varName, string type, bool init = false){
		std::cout << "/" << stos.size()+1 <<"\\ Pushing variable "<< varName << " : " << type << " to stack." << '\n';
		if(type == "var"){
			varName.erase(0, 1);
			auto var = ids.find(varName);
			if(var != ids.end()){
				type = var->second.type;
			}
		}
		if(init == true && type == "flt"){
			string var = " __float"+to_string(floats++);
			add_id(var,"flt", varName);
			varName = var;
		}
		if(init == true && type == "str"){
			string data, type;
			memoryStack::pullFromStack(&data, &type);
			add_id(data,"str", varName);
			varName=data;
		}
		types_stos.push(type);
		stos.push(varName);
	}
}
namespace types{
	bool compareTypes(string typea, string typeb){
		return (typea == "int" || typea == "arrint") && (typeb == "flt" || typeb == "arrflt")
			|| (typea == "flt" || typea == "arrflt") && (typeb == "int" || typeb == "arrint");
	}
	string typeTranslation(string type, string variable){
		std::cout << ">!< Translating "<<variable<<" to "<<type << '\n';
		if(type=="flt"){
			string localVar = utils::getNextTemp()+"_t";
			add_id(localVar, "flt");
			asembler::write_asm_int_to_flt(variable, localVar);
			return localVar;
		} else {
			string localVar = utils::getNextTemp()+"_t";
			add_id(localVar, "flt");
			asembler::write_asm_flt_to_int(variable, localVar);
			return localVar;
		}
	}
	void integerOperation(char sign, string a, string b, string typea, string typeb){
		if(sign == '='){
			asembler::write_asm(b,a,determineActionMark(sign),b,typeb,typea);
			add_id(b,typea);
		} else {
			string localVar = utils::getNextTemp();
			add_id(localVar,typeb);
			asembler::write_asm(b,a,determineActionMark(sign),localVar, typeb, typea);
			memoryStack::pushToStack(localVar, typea);
		}
	}
	void floatingOperation(char sign, string a, string b, string typea, string typeb){
		if(sign == '='){
			asembler::write_asm_floating(b,a,determineActionMark(sign),b,typeb,typea);
			add_id(b,typea);

		} else {
			string localVar = utils::getNextTemp();
			add_id(localVar,typeb);
			asembler::write_asm_floating(b,a,determineActionMark(sign),localVar, typeb, typea);

			memoryStack::pushToStack(localVar, typea);
		}
	}
	void getValueFromTable(string tableType, string tableName, string tablePos, string otherValue, string otherType){
		std::cout << "<> Getting "<<tableName<<"["<<tablePos<<"]" << '\n';
		if(otherValue!="")
			memoryStack::pushToStack(otherValue, otherType);
		string localVar = utils::getNextTemp();
		add_id(localVar,tableType=="arrint"?"int":"flt");
		asembler::write_asm_table_get_value(tableName,tablePos,localVar);
		memoryStack::pushToStack(localVar, tableType=="arrint"?"int":"flt");
	}
	bool integerTable(char sign, string a, string b, string typea, string typeb, string tablePos){
		if(sign == '='){
				if(typeb=="arrint" || typeb=="arrflt"){
					if(types::compareTypes(typea,typeb))
						a = typeTranslation("flt", a);
					else if(types::compareTypes(typea,typeb))
						a = typeTranslation("int", a);
					asembler::write_asm_table_assign(b,tablePos,typea,a);
					return true;
				}
				getValueFromTable(typea, a, tablePos, b, typeb);
				return false;
		} else {
			if(typeb=="arrint" || typeb=="arrflt"){
				getValueFromTable(typeb, b, tablePos, a, typea);
				return false;
			} else {
				getValueFromTable(typea, a, tablePos, b, typeb);
				return false;
			}
		}
	}
}
namespace operations{
	void stack_operation(char sign){
		if(stos.size() > 0){
			string a, typea, b, typeb;
			memoryStack::pullFromStack(&a, &typea);
			string tablePos, type;
			if(typea == "arrint" || typea == "arrflt"){
				memoryStack::pullFromStack(&tablePos, &type);
			}
			memoryStack::pullFromStack(&b, &typeb);
			if(typeb == "arrint" || typeb == "arrflt"){
				memoryStack::pullFromStack(&tablePos, &type);
			}
			std::cout << a<<":"<<typea<<sign<<b<<":"<<typeb<< '\n';
			if(typeb == "arrint" || typeb == "arrflt" || typea == "arrint" || typea == "arrflt"){
				if(types::integerTable(sign,a,b,typea,typeb,tablePos))
					return;
				memoryStack::pullFromStack(&a, &typea);
				memoryStack::pullFromStack(&b, &typeb);
			}
			if(types::compareTypes(typea, typeb)){
				b = types::typeTranslation(typea, b);
			}
			if(typea == "int")
				types::integerOperation(sign, a, b, typea, typeb);
			if(typea == "flt")
				types::floatingOperation(sign, a, b, typea, typeb);
		}
	}
	void stack_comparation(char sign){
		operations::stack_operation('-');
		string val, type;
		memoryStack::pullFromStack(&val, &type);
		/*
		* val osiąga następujące wartości:
		* 1. ujemne gdy 2 wartość jest większa
		* 2. dodatnie gdy 2 wartość jest większa
		* 3. 0 gdy są równe
		*/
		string label = utils::getNextLabel();
		std::cout << "[LABEL START] "<<label<< '\n';
		switch(sign){
			case '<': asembler::write_asm_jump("ble", type, val, label);
								break;
			case '>': asembler::write_asm_jump("bge", type, val, label);
								break;
			case 'l': asembler::write_asm_jump("bgt", type, val, label);
								break;
			case 'g': asembler::write_asm_jump("blt", type, val, label);
								break;
			case 'e': asembler::write_asm_jump("bne", type, val, label);
								break;
			case 'n': asembler::write_asm_jump("beq", type, val, label);
								break;
		}
		labels_stos.push(label);
	}
	void stack_label(){
		std::cout << "[LABEL STOP]"<<labels_stos.top()<< '\n';
		asembler::write_asm_label(labels_stos.top());
		labels_stos.pop();
	}
	void stack_jump(){
		string label = utils::getNextLabel();
		asembler::write_asm_jump("j", "", "", label);
		operations::stack_label();
		labels_stos.push(label);
	}
	void stack_while(bool start){
		if(start){
			string label = utils::getNextLabel();
			asembler::write_asm_label(label);
			loop_stos.push(label);
		} else {
			asembler::write_asm_jump("j", "", "", loop_stos.top());
			loop_stos.pop();
		}
	}
	void stack_IO(bool output){
		string var, type;
		memoryStack::pullFromStack(&var, &type);
		if(type == "arrflt" || type == "arrint"){
			string tablePos, tablePosType;
			memoryStack::pullFromStack(&tablePos, &tablePosType);
			types::getValueFromTable(type, var, tablePos, "", type=="arrint"?"int":"flt");
			memoryStack::pullFromStack(&var, &type);
		}
		std::cout << "IO: "<<var<<" : "<<type << '\n';
		if(output){
			asembler::write_asm_print(var,type);
		} else {
			asembler::write_asm_read(var,type);
		}
	}
}
void declare_array(string name, string type, string size){
	string varType = "arr"+type;
	name.erase(0, 1);
	add_id(name, varType, "0:"+size);
	std::cout << ">> Declared array: "<<name<<" : "<<varType << '\n';
}

#line 555 "def.tab.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "def.tab.hh".  */
#ifndef YY_YY_DEF_TAB_HH_INCLUDED
# define YY_YY_DEF_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    TEXT = 259,
    LC = 260,
    LZ = 261,
    PRINT = 262,
    INPUT = 263,
    INT = 264,
    FLT = 265,
    STR = 266,
    LEQ = 267,
    EQ = 268,
    NEQ = 269,
    GEQ = 270,
    WHILE = 271,
    TAB = 272,
    STRING = 273,
    IF = 274,
    ELSE = 275,
    THEN = 276
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 490 "def.yy" /* yacc.c:355  */

    char *text;
    int	ival;
    double fval;

#line 623 "def.tab.cc" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_DEF_TAB_HH_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 640 "def.tab.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  30
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   195

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  10
/* YYNRULES -- Number of rules.  */
#define YYNRULES  36
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  91

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      24,    25,    34,    32,     2,    33,     2,    35,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    22,
      31,    23,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    26,     2,    27,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,    29,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   518,   518,   519,   520,   521,   523,   526,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   538,   541,   544,
     545,   546,   547,   548,   549,   552,   553,   554,   557,   558,
     559,   562,   563,   564,   565,   566,   567
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "TEXT", "LC", "LZ", "PRINT",
  "INPUT", "INT", "FLT", "STR", "LEQ", "EQ", "NEQ", "GEQ", "WHILE", "TAB",
  "STRING", "IF", "ELSE", "THEN", "';'", "'='", "'('", "')'", "'['", "']'",
  "'{'", "'}'", "'>'", "'<'", "'+'", "'-'", "'*'", "'/'", "$accept",
  "srednik", "rownanie", "fun", "elsestack", "whilestack", "comp", "wyr",
  "skladnik", "czynnik", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    59,    61,    40,    41,    91,    93,   123,   125,
      62,    60,    43,    45,    42,    47
};
# endif

#define YYPACT_NINF -30

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-30)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      27,   -24,   -30,   -30,   -30,    -7,     5,    16,    20,    56,
     -30,    15,    56,   168,    19,    23,    26,    -9,    56,    56,
      56,    21,   -30,    39,   -30,    55,    56,    80,   -29,   -30,
     -30,    30,    77,   -30,   -30,    56,    56,   -17,   129,   134,
     104,   131,    56,    75,    54,   -30,    56,    56,    56,    56,
     -30,   -30,    76,    31,   -30,   -30,   -30,    88,    99,   -30,
      74,    56,    56,    56,    56,    56,    56,   -29,   -29,   -30,
     -30,    86,   -30,   -30,    27,    31,    31,    31,    31,    31,
      31,    27,    87,   114,    84,   -30,   -30,   101,    27,   141,
     -30
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    31,    35,    33,    34,     0,     0,     0,     0,     0,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,     9,    31,    10,     0,     0,     0,    27,    30,
       1,     0,     0,     3,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,     0,
       2,     4,     0,     6,    32,     7,     8,    33,    33,    11,
       0,     0,     0,     0,     0,     0,     0,    25,    26,    28,
      29,     0,    12,    13,     0,    21,    23,    24,    22,    19,
      20,     0,     0,     0,    14,    16,    17,     0,     0,     0,
      15
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -30,   107,   -12,   -10,   -30,   -30,    92,    -8,    29,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    13,    14,    15,    87,    16,    43,    37,    28,    29
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      17,    31,    18,    32,    27,    48,    49,    22,    24,    25,
      54,    38,    39,    17,    36,    46,    47,    19,    44,    21,
       2,     3,     4,    23,     2,     3,     4,    44,    53,    20,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    26,
      12,    33,    59,    10,    12,    34,    11,    40,    69,    70,
      35,    12,    50,    75,    76,    77,    78,    79,    80,     1,
       2,     3,     4,    46,    47,    41,    61,    62,    63,    64,
      31,    31,    32,    32,    17,    67,    68,    31,    42,    32,
      12,    17,    17,    17,    65,    66,    46,    47,    17,    17,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    51,
      60,    71,    74,    10,    86,    45,    11,     1,     2,    57,
       4,    12,    46,    47,    81,    72,    84,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    73,    52,    12,    88,
      10,     0,     0,    11,     1,     2,    58,     4,    12,     0,
       0,     0,     0,    85,     1,     2,     3,     4,     5,     6,
       7,     8,     9,     0,    55,    12,     0,    10,     0,    56,
      11,    46,    47,     0,     0,    12,    46,    47,    30,     0,
      90,     1,     2,     3,     4,     5,     6,     7,     8,     9,
       0,    82,     0,     0,    10,     0,     0,    11,    83,     0,
       0,     0,    12,     0,     0,    89
};

static const yytype_int8 yycheck[] =
{
       0,    13,    26,    13,    12,    34,    35,     7,     8,     9,
      27,    19,    20,    13,    23,    32,    33,    24,    26,     3,
       4,     5,     6,     3,     4,     5,     6,    35,    36,    24,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    24,
      24,    22,    42,    16,    24,    22,    19,    26,    48,    49,
      24,    24,    22,    61,    62,    63,    64,    65,    66,     3,
       4,     5,     6,    32,    33,    26,    12,    13,    14,    15,
      82,    83,    82,    83,    74,    46,    47,    89,    23,    89,
      24,    81,    82,    83,    30,    31,    32,    33,    88,    89,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    22,
      25,    25,    28,    16,    20,    25,    19,     3,     4,     5,
       6,    24,    32,    33,    28,    27,    29,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    27,    35,    24,    28,
      16,    -1,    -1,    19,     3,     4,     5,     6,    24,    -1,
      -1,    -1,    -1,    29,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    -1,    25,    24,    -1,    16,    -1,    25,
      19,    32,    33,    -1,    -1,    24,    32,    33,     0,    -1,
      29,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      -1,    74,    -1,    -1,    16,    -1,    -1,    19,    81,    -1,
      -1,    -1,    24,    -1,    -1,    88
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      16,    19,    24,    37,    38,    39,    41,    45,    26,    24,
      24,     3,    45,     3,    45,    45,    24,    43,    44,    45,
       0,    38,    39,    22,    22,    24,    23,    43,    43,    43,
      26,    26,    23,    42,    43,    25,    32,    33,    34,    35,
      22,    22,    42,    43,    27,    25,    25,     5,     5,    45,
      25,    12,    13,    14,    15,    30,    31,    44,    44,    45,
      45,    25,    27,    27,    28,    43,    43,    43,    43,    43,
      43,    28,    37,    37,    29,    29,    20,    40,    28,    37,
      29
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    36,    37,    37,    37,    37,    38,    39,    39,    39,
      39,    39,    39,    39,    39,    39,    39,    40,    41,    42,
      42,    42,    42,    42,    42,    43,    43,    43,    44,    44,
      44,    45,    45,    45,    45,    45,    45
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     2,     3,     2,     3,     4,     4,     2,
       2,     4,     5,     5,     7,    11,     7,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       1,     1,     4,     1,     1,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 518 "def.yy" /* yacc.c:1646  */
    {}
#line 1788 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 519 "def.yy" /* yacc.c:1646  */
    {}
#line 1794 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 520 "def.yy" /* yacc.c:1646  */
    {}
#line 1800 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 521 "def.yy" /* yacc.c:1646  */
    {}
#line 1806 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 523 "def.yy" /* yacc.c:1646  */
    { operations::stack_operation('=');}
#line 1812 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 526 "def.yy" /* yacc.c:1646  */
    {operations::stack_IO(true);}
#line 1818 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 527 "def.yy" /* yacc.c:1646  */
    {operations::stack_IO(false);}
#line 1824 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 528 "def.yy" /* yacc.c:1646  */
    {string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"int");}
#line 1830 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 529 "def.yy" /* yacc.c:1646  */
    {string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"flt");}
#line 1836 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 530 "def.yy" /* yacc.c:1646  */
    {string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"str");}
#line 1842 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 531 "def.yy" /* yacc.c:1646  */
    {declare_array((yyvsp[-3].text), "int",to_string((yyvsp[-1].ival)));}
#line 1848 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 532 "def.yy" /* yacc.c:1646  */
    {declare_array((yyvsp[-3].text), "flt",to_string((yyvsp[-1].ival)));}
#line 1854 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 533 "def.yy" /* yacc.c:1646  */
    {operations::stack_label();}
#line 1860 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 534 "def.yy" /* yacc.c:1646  */
    {operations::stack_label();}
#line 1866 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 535 "def.yy" /* yacc.c:1646  */
    {operations::stack_while(false);operations::stack_label();}
#line 1872 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 538 "def.yy" /* yacc.c:1646  */
    {operations::stack_jump();}
#line 1878 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 541 "def.yy" /* yacc.c:1646  */
    {operations::stack_while(true);}
#line 1884 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 544 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('>');}
#line 1890 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 545 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('<');}
#line 1896 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 546 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('l');}
#line 1902 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 547 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('g');}
#line 1908 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 548 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('e');}
#line 1914 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 549 "def.yy" /* yacc.c:1646  */
    {operations::stack_comparation('n');}
#line 1920 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 552 "def.yy" /* yacc.c:1646  */
    {operations::stack_operation('+');}
#line 1926 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 553 "def.yy" /* yacc.c:1646  */
    {operations::stack_operation('-');}
#line 1932 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 554 "def.yy" /* yacc.c:1646  */
    {}
#line 1938 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 557 "def.yy" /* yacc.c:1646  */
    {operations::stack_operation('*');}
#line 1944 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 558 "def.yy" /* yacc.c:1646  */
    {operations::stack_operation('/');}
#line 1950 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 559 "def.yy" /* yacc.c:1646  */
    {}
#line 1956 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 562 "def.yy" /* yacc.c:1646  */
    {memoryStack::pushToStack((yyvsp[0].text), "var");}
#line 1962 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 563 "def.yy" /* yacc.c:1646  */
    {memoryStack::pushToStack((yyvsp[-3].text), "var");}
#line 1968 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 564 "def.yy" /* yacc.c:1646  */
    {memoryStack::pushToStack(to_string((yyvsp[0].ival)), "int", true);}
#line 1974 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 565 "def.yy" /* yacc.c:1646  */
    {memoryStack::pushToStack(to_string((yyvsp[0].fval)), "flt", true); }
#line 1980 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 566 "def.yy" /* yacc.c:1646  */
    {memoryStack::pushToStack((yyvsp[0].text), "str", true); }
#line 1986 "def.tab.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 567 "def.yy" /* yacc.c:1646  */
    {}
#line 1992 "def.tab.cc" /* yacc.c:1646  */
    break;


#line 1996 "def.tab.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 569 "def.yy" /* yacc.c:1906  */

int main(int argc, char *argv[])
{
	if (argc>1){
		yyin=fopen(argv[1], "r");
		std::cout << "READING "<<yyin << '\n';
		if (argc>2)
		{
			file_out = out(argv[2]);
		}
	}
	yyparse();
	save_ids();
	return 0;
}
