/****************************************************************************
  
 Copyright (c) 2017, Hans Robeers
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

#ifndef HRLIB_CURVEREADER_HPP
#define HRLIB_CURVEREADER_HPP

#include <sstream>
#include <vector>
#include <algorithm>

#include "hrlib/io/vertex.hpp"
#include "hrlib/io/getlinesafe.hpp"

namespace hrlib
{
  namespace utf8
  {
    const std::string delimiters = " \t,;";

    inline bool is_float_char(char c)
    {
      if (isdigit(c)) return true;
      const std::string nondigit = "-.eE";
      return std::any_of(nondigit.cbegin(), nondigit.cend(), [c](char f){ return c==f; });
    }

    inline bool is_floats(const std::string &str)
    {
      auto is_floats_test = [](char c){
                              if (is_float_char(c)) return true;
                              return std::any_of(delimiters.cbegin(), delimiters.cend(), [c](char f){ return c==f; });
                            };
      if (std::none_of(str.begin(), str.end(), is_floats_test))
        return false;
      return std::all_of(str.begin(), str.end(), is_floats_test);
    }

    template<int Dim>
    std::istream& read_next_vertex_line(std::istream& str, vertex<Dim>& v)
    {
      while (str.peek()!=EOF) {
      // find next line containing floats
      std::string line;
      do
        {
          getline_safe(str, line);
        } while(str.peek()!=EOF && !is_floats(line));

      // split line by delimiters
      int i = 0;
      size_t prev = 0;
      while (true)
        {
          auto pos = line.find_first_of(delimiters, prev);
          if (pos > prev+1)
            std::istringstream(line.substr(prev, pos-prev)) >> v[i++];
          prev = pos+1;
          if (i==Dim || pos==std::string::npos)
            break;
        }

      if (i>=Dim)
        break;
      }
      return str;
    }

    template<int Dim>
    std::istream& read_next_vertex(std::istream& str, vertex<Dim>& v)
    {
      char c;
      int i=0;

      while (i<Dim) {
        bool prefix=true;
        std::stringstream fstr;
        while (str.get(c)) {
          if (is_float_char(c)) {
            prefix = false;
            fstr << c;
          }
          else if (!prefix)
            break;
        }
        fstr >> v[i++];
      }

      return str;
    }
  }
}

#endif // HRLIB_CURVEREADER_HPP
