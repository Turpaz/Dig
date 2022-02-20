#include "codegen.hpp"

Codegen::Codegen(Parser* parser)
{
	this->parser = parser;

	prolog = "\
	section .text\n\
	global _start\n\
	_start:\n\
	jmp main\n";
	data = "section .data\n";
	bss = "section .bss\n";
}

void Codegen::generate()
{

}

string Codegen::tostr() const
{
	// TODO: Implement
	string str;
	str.append(prolog);
	str.append(data);
	str.append(bss);
	return str;
}

// Nodes codegen polymorphic function

void Nodes::RootStatement::codegen(Codegen& codegen) const {}

void Nodes::EofStatement::codegen(Codegen& codegen) const {}