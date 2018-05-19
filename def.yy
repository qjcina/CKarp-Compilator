%{
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
%}
%union
{
    char *text;
    int	ival;
    double fval;
};
%type <text> wyr
%token <text> ID
%token <text> TEXT
%token <ival> LC
%token <fval> LZ
%token PRINT
%token INPUT
%token INT
%token FLT
%token STR
%token LEQ
%token EQ
%token NEQ
%token GEQ
%token WHILE
%token TAB
%token <text> STRING
%token <text> IF
%token <text> ELSE
%right THEN ELSE
//%start wyr
%%
srednik
	:srednik rownanie ';'				{}
	|rownanie ';'								{}
	|srednik fun ';'										{}
	|fun ';'										{}
rownanie
	:czynnik '=' wyr						{ operations::stack_operation('=');}
	;
fun
	:PRINT '(' wyr ')'					{operations::stack_IO(true);}
	|INPUT '(' wyr ')'					{operations::stack_IO(false);}
	|INT czynnik								{string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"int");}
	|FLT czynnik								{string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"flt");}
	|STR czynnik '=' czynnik		{string var, type; memoryStack::pullFromStack(&var, &type); add_id(var,"str");}
	|INT ID '[' LC ']'					{declare_array($2, "int",to_string($4));}
	|FLT ID '[' LC ']'					{declare_array($2, "flt",to_string($4));}
	|IF '(' comp ')' '{' srednik '}' 											{operations::stack_label();}
	|IF '(' comp ')' '{' srednik '}' elsestack '{' srednik '}' {operations::stack_label();}
	|whilestack '(' comp ')' '{' srednik '}' {operations::stack_while(false);operations::stack_label();}
	;
elsestack
	:ELSE 											{operations::stack_jump();}
	;
whilestack
	:WHILE											{operations::stack_while(true);}
	;
comp
	: wyr '>' wyr 							{operations::stack_comparation('>');}
	| wyr '<' wyr								{operations::stack_comparation('<');}
	| wyr LEQ wyr								{operations::stack_comparation('l');}
	| wyr GEQ wyr								{operations::stack_comparation('g');}
	| wyr EQ wyr								{operations::stack_comparation('e');}
	| wyr NEQ wyr								{operations::stack_comparation('n');}
	;
wyr
	:wyr '+' skladnik 	     		{operations::stack_operation('+');}
	|wyr '-' skladnik	       		{operations::stack_operation('-');}
	|skladnik		             	{}
	;
skladnik
	:skladnik '*' czynnik	   		{operations::stack_operation('*');}
	|skladnik '/' czynnik	   		{operations::stack_operation('/');}
	|czynnik		             	{}
	;
czynnik
	:ID			                 	{memoryStack::pushToStack($1, "var");}
	|ID '[' wyr ']'							{memoryStack::pushToStack($1, "var");}
	|LC			               		{memoryStack::pushToStack(to_string($1), "int", true);}
	|LZ			               		{memoryStack::pushToStack(to_string($1), "flt", true); }
	|TEXT			               		{memoryStack::pushToStack($1, "str", true); }
	|'('wyr')'		           		{}
	;
%%
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
