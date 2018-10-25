/****************************************************************************

 Copyright (c) 2015, Hans Robeers
 All rights reserved.

 BSD 2-Clause License

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#include "hrlib/string/json_utils.hpp"

#include <regex>

namespace {
    const std::regex re_float("[-]*\\d+[.]\\d*[09]{5,}\\d");
    const std::regex re_zeros("[0]{5,}\\d");
    const std::regex re_nines("[9]{5,}\\d");
}

std::string hrlib::trim_json_floats(const std::string &json_utf8)
{
    std::stringstream retVal;

    std::sregex_iterator next(json_utf8.begin(), json_utf8.end(), re_float);
    std::sregex_iterator end;

    size_t prev_end = 0;

    while (next != end) {
        std::smatch match = *next;

        // append the non-float part
        retVal << json_utf8.substr(prev_end, match.position() - prev_end);

        // reduce float precision and append
        std::string reduced = std::regex_replace(std::regex_replace(match.str(), re_zeros, ""), re_nines, "9");
        if (reduced.back() == '.')
          reduced.erase(reduced.cend()-1);

        retVal << reduced;

        prev_end = match.position() + match.length();
        next++;
    }

    // append the last part
    retVal << json_utf8.substr(prev_end);

    return retVal.str();
}
