#pragma once
#include <list>

struct BlockEncoding;
using BlockColumn = std::list<BlockEncoding>;

struct BlockEncoding {
	unsigned char blockType;
	unsigned int blockCount;

	BlockEncoding() : blockType(0), blockCount(0) { }
	BlockEncoding(unsigned int bt, unsigned int bc) : blockType(bt), blockCount(bc) { }
};
