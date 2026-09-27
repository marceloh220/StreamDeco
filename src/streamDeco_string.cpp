
#include "streamDeco_string.hpp"

#include <cctype>
#include <cstdlib>

namespace streamDeco
{

    namespace monitor
    {

  static int param_strs[YEAR + 1] = {0};

    namespace
    {
      void trim_in_place(string &text)
      {
        size_t start = 0;
        while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
        {
          ++start;
        }

        size_t end = text.size();
        while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
        {
          --end;
        }

        text = text.substr(start, end - start);
      }
    } // namespace

  
    string nextFrameToken(const string &frame, int &start)
    {
      if (start < 0 || static_cast<size_t>(start) >= frame.length())
      {
        return "";
      }

      const size_t search_pos = static_cast<size_t>(start);
      const size_t sep = frame.find(',', search_pos);
      string token;

      if (sep == string::npos)
      {
        token = frame.substr(search_pos);
        start = static_cast<int>(frame.length());
      }
      else
      {
        token = frame.substr(search_pos, sep - search_pos);
        start = static_cast<int>(sep + 1);
      }

      trim_in_place(token);
      return token;
    } // nextFrameToken

    bool parseMonitorFrame(const string &frame)
    {
      int start = 0;

      string token = nextFrameToken(frame, start);
      for (int i = 0; i <= YEAR; i++)
      {
        if (!token.empty())
          param_strs[i] = static_cast<int>(std::strtol(token.c_str(), nullptr, 10));
        else
          param_strs[i] = 0;

        token = nextFrameToken(frame, start);
      }
      return true;
    } // parseMonitorFrame

    int getMonitorParam(FrameIndex_t index)
    {
      return param_strs[index];
    } // getMonitorParam

  } // namespace monitor

} // namespace streamDeco