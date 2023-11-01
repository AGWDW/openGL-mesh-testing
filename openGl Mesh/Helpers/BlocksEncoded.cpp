#include "BlocksEncoded.h"
#include "../Helpers/Constants.h"

BlocksEncoded::BlocksEncoded() : blocks_(), counts_(), height_()
{
}

void BlocksEncoded::push(const Block block, const unsigned int count)
{
	blocks_.push_back(block);
	counts_.push_back(count);
	height_ += count;
}

const unsigned int BlocksEncoded::height() const
{
	return height_;
}

const size_t BlocksEncoded::size() const
{
	return counts_.size();
}

const Block BlocksEncoded::block(const unsigned int i) const
{
	return blocks_[i];
}

const unsigned int BlocksEncoded::count(const unsigned int i) const
{
	return counts_[i];
}

const Block BlocksEncoded::operator[](int height) const
{
	for (unsigned int i = 0; i < counts_.size(); i++) {
		height -= counts_[i];
		if (height <= 0) {
			return blocks_[i];
		}
	}
	return Block::AIR;
}
