#include <string>

namespace streamDeco
{
    namespace monitor
    {

        typedef std::string string;

        typedef enum FrameIndex_e
        {
            CPU_LOAD = 0,
            CPU_TEMP,
            CPU_FREQ,
            GPU_LOAD,
            GPU_TEMP,
            GPU_FREQ,
            MEM_USED,
            MEM_MAX,
            DISK_USED,
            DISK_MAX,
            SEC,
            MIN,
            HOUR,
            WEEK_DAY,
            DAY,
            MONTH,
            YEAR
        } FrameIndex_t;

        bool parseMonitorFrame(const string &frame);
        int getMonitorParam(FrameIndex_t index);

    } // namespace monitor

} // namespace streamDeco