#include "Program.h"

size_t args_container::args_size() const
{
    //if (this->empty()) return 0u;
    //size_t result = 0u;
    //for (const auto& arg : *this)
    //{
    //    result += arg.val.size() + 1;
    //}
    //result -= 1;
    //return result;

	if (this->empty())return 0u;
	uint8_t arg_size = (*this)[0].val.size();
	if (this->size() > 1) {
		for (size_t i = 1; i < this->size(); ++i) {
			arg_size += (*this)[i].val.size() + 1;
		}
		arg_size -= 1;
	}
	return arg_size;
}
