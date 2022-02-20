#include "codegen.hpp"

Codegen::Codegen(Parser* parser, string output_file)
{
	this->parser = parser;
	this->output_file = output_file;
}

void Codegen::generate()
{

}

string Codegen::assemble_parts() const
{
	// TODO: Implement
	string str;
	str.append(prolog);
	str.append(epilog);
	str.append(data);
	str.append(bss);
	return str;
}

// Nodes codegen polymorphic function

void Nodes::RootStatement::codegen(Codegen& codegen) const
{
	codegen.prolog ="\
	section .text\n\
	global _start\n\
	_start:\n\
	jmp main\n";
}

void Nodes::EofStatement::codegen(Codegen& codegen) const
{

}