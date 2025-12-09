#include "StringValidation.h"

bool ValidateInputString(std::string const& str, size_t size)
{
    //! move utf8::is_valid calls to here
    if (str.size() > size)
        return false;

    if ( !utf8::is_valid(str.begin(), str.end() ) )
         return false;

    return true;
}
