#ifndef __IF_STATEMENT_H__
#define __IF_STATEMENT_H__

#include "AST.h"
#include "Block.h"
#include "CondBlock.h"

class IfStatement : public Expression {
private:
public:
	std::vector<Block *> blocks;

	Value* Codegen();
	Type *getType();

	virtual std::string getClass() { return "IfStatement"; }

	virtual std::string string() {  
		std::stringstream ss;

		for (Block *b : blocks) {
			ss << "\tifblock: " << std::endl;
			for (Statement *s : b->statements) {
				ss << "\t\t" << s->string() << std::endl;
			}
		}

		return ss.str();
	}


	IfStatement() { }
};

#endif 