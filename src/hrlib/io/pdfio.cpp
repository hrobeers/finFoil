/****************************************************************************

 Copyright (c) 2018, Hans Robeers
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

#include "hrlib/io/pdfio.hpp"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <cctype>
#include <cmath>

#include "hrlib/io/vertexio.hpp"

using namespace hrlib;
using namespace pdf;

namespace {
  const std::string HDR_BEGIN("<<");
  const std::string HDR_END(">>");
  const std::string LENGTH("/Length");
  const std::string FLATE("/FlateDecode");
  const std::string LINEARIZED("/Linearized");
  const std::string ASCII85("/ASCII85Decode"); // TODO not implemented
  const std::string L("/L");
  const std::string N("/N");

  bool is_path_char(char c) { return std::any_of(path_chars.cbegin(), path_chars.cend(), [c](char f){ return c==f; }); }
  bool is_path_line(const std::string &str)
  {
    return std::all_of(str.begin(), str.end(), [](char c){
        if (utf8::is_float_char(c)) return true;
        return is_path_char(c);
      });
  }

  bool try_read_property(const std::string &line, const std::string &prop, bool &out) {
    if(line.find(prop) != std::string::npos)
      return out=true;
    return false;
  }

  bool try_read_property(const std::string &line, const std::string &prop, size_t &out) {
    size_t pos=line.find(prop);
    if (pos != std::string::npos)
    {
        // offset by Length position
        auto it=line.cbegin()+pos+prop.size()+1;
        // skip whitespace
        while(std::any_of(whitespace.cbegin(), whitespace.cend(), [&it](char f){ return *it==f; })) ++it;
        auto length_begin = it;
        // iterate until non-digit
        while(std::isdigit(*it)) ++it;
        // substring and write to size_t
        std::istringstream(std::string(length_begin, it)) >> out;
        return true;
    }
    return false;
  }
}

std::istream& hrlib::pdf::read_linear_dict(std::istream &stream, linear_dict &dict)
{
  std::string line;
  while(true)
  {
    getline_safe(stream, line);
    if (!stream) return stream;

    bool linearized = false;
    if (line.find(HDR_BEGIN) != std::string::npos)
    {
      do
      {
        try_read_property(line, LINEARIZED, linearized);
        try_read_property(line, L, dict.file_length);
        try_read_property(line, N, dict.num_pages);
        getline_safe(stream, line);
      } while (line.find(HDR_END)==std::string::npos);
    }

    if (linearized)
      return stream;
  }
  return stream;
}

std::istream& hrlib::pdf::read_next_binary(std::istream &stream, std::vector<char> &bin)
{
  std::string line;
  while(true)
  {
    getline_safe(stream, line);
    if (!stream) return stream;

    if (line.find(HDR_BEGIN) != std::string::npos)
    {
        size_t length=0;
        bool ascii85,compressed=false;

        do
        {
          try_read_property(line, ASCII85, ascii85);
          try_read_property(line, FLATE, compressed);
          try_read_property(line, LENGTH, length);
          if (line.find(HDR_END)!=std::string::npos)
            break;
        } while (getline_safe(stream, line));

        // Continue if not a stream object
        if (length==0 || (ascii85 && !compressed)) // compressed referenced here purely to avoid optimizing out :S
          // TODO ascii85 not implemented
          continue;

        // Read until "stream" keyword
        while (line.find("stream")==std::string::npos)
        { getline_safe(stream,line); }

        // Read the binary data
        //if (length<32) { // TODO better length decoding (this case is to allow length references)
          bin.clear();
          size_t pos, slast = 0;
          while ((pos=line.find("endstream"))==std::string::npos)
          {
             getline_safe(stream,line, true);
             bin.insert(bin.end(), line.cbegin(), line.cend());
             slast=line.size();
          }
          bin.resize(bin.size()-(slast-pos));
          while (bin.back()=='\n' || bin.back()=='\r')
            bin.pop_back();
        /*
        }
        else {
          bin.resize(length);
          stream.read(bin.data(), length);
        }
        */

        // Decompress if compressed
        if (compressed)
        {
          boost::iostreams::array_source src {bin.data(), bin.size()};
          boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
          in.push(boost::iostreams::zlib_decompressor());
          in.push(src);
          std::vector<char> out;
          out.assign(std::istreambuf_iterator<char>{&in}, {});
          bin.swap(out);
        }

        return stream;
    }
  }
}

std::optional<path_cmd> hrlib::pdf::parse_path_line(const std::string &line)
{
  if (!is_path_line(line))
    return std::optional<path_cmd>();

  // Push coordinates & extract command
  path_cmd retval;
  size_t prev = 0;
  size_t coord_idx = 0;
  while (true)
  {
    auto pos = line.find_first_of(delimiters, prev);

    std::string word = line.substr(prev, pos-prev);
    if (hrlib::utf8::is_floats(word))
    {
      double v;
      std::istringstream(line.substr(prev, pos-prev)) >> v;
      if (coord_idx % 2 == 0)
        retval.vals.push_back({v, NAN});
      else
        retval.vals.back()[1] = v;
      ++coord_idx;
    }
    else if (word.size()==1 && is_path_char(word[0]) && retval.vals.size()>0)
    {
      retval.cmd = word[0];
      return retval;
    }

    prev = pos+1;
    if (pos==std::string::npos)
      break;
  }

  return std::optional<path_cmd>();
}
