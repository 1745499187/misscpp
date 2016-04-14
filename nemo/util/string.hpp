/*
 * string.hpp
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef STRING_HPP_
#define STRING_HPP_

#include <nemo/config.hpp>

#include <string>
#include <stdio.h>

namespace nemo
{

namespace util
{

namespace str
{

std::string
trim(std::string& origi, const std::string &to_be_trimed = " ")
{
  size_t first = 0, last = 0;

  first = origi.find_first_not_of(to_be_trimed);
  if (first != origi.npos && first > 0 && origi.size() > 0) {
      origi.erase(origi.begin(), origi.begin() + first);
  }

  last = origi.find_last_not_of(to_be_trimed);
  if (last != origi.npos && origi.size() > 0) {
      origi.erase(origi.begin() + last + 1, origi.end());
  }

  return origi;
}


/*
 * print hex string of a BYTE buffer
 */
std::string
str_to_hexstr(const char *instr, size_t len, bool truncated = true)
{
    unsigned int count = len;
    const unsigned char *buf = (unsigned char *)instr;

    if (truncated && len > 36)
        count = 36;

    std::string str;
    char s[NEMO_BUF_SZ_L];

    //print header
    sprintf(s, "Hex dump length=%d (0x%x) (%d dumped)\n", len, len, count);
    str += s;
    unsigned int j = 0, k = 0;
    for (j = 0; j < count; j++)
    {
        if (j % 16 == 0)
        {
            if (j > 0)
            {
                // print printable chars
                str += " ";
                for (k = j - 16; k < j; k++)
                {
                    sprintf(s, "%c", (buf[k] >= ' ' && buf[k] <= '~') ? buf[k] : '.');
                    str += s;
                }
                str += "\n";
            }

            sprintf(s, "%03x", j);
            str += s;
        }
        if (j % 16 == 4 || j % 16 == 8 || j % 16 == 12)
            str += " -";

        sprintf(s, " %02x", buf[j]);
        str += s;
    }

    //print printable chars for the last line
    if (j > 0 && j % 16 != 0)
    {
        //align
        int spaces = (16 - j % 16) * 3 + (16 - j % 16) / 4 * 2 + 1;

        for (int i = 0; i < spaces; i++)
            str += " ";

        for (k = (j - j % 16); k < j; k++)
        {
            sprintf(s, "%c", (buf[k] >= ' ' && buf[k] <= '~') ? buf[k] : '.');
            str += s;
        }
    }
    if (j > 0 && j % 16 == 0)
    {
        //align
        str += " ";

        for (k = (j - 16); k < j; k++)
        {
            sprintf(s, "%c", (buf[k] >= ' ' && buf[k] <= '~') ? buf[k] : '.');
            str += s;
        }
    }

    if (count < len)
        str += "\n...";

    str += "\n";
    return str;
}

}

}

}

#endif /* STRING_HPP_ */
