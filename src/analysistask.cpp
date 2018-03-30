#include "analysistask.h"
#include <map>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

namespace mt {

AnalysisTask::AnalysisTask(const std::vector<uint8_t> & Feedback) :_feedback(Feedback)
{
}

void AnalysisTask::Compute()
{
    float mipSizes[] = {
        //256.0f,
        128.0f,
        64.0f,
        32.0f,
        16.0f,
        8.0f,
        4.0f,
        2.0f,
        1.0f
    };

    // forget unhandled request
    _result.clear();
    // each pixel which yields a mipmap level and a page identifier
    // pages that are referenced more has a higher priority
    std::map<uint32_t, size_t> ref;
    if (!_feedback.empty() && ((_feedback.size() % 3) == 0))
    {
        const uint8_t * p = &_feedback[0];
        for(size_t i = 0, count = _feedback.size(); i < count; i+= 3)
        {
            uint8_t mip = p[i];
            if (mip > 8) {
                continue;
            }
            uint8_t x = (float(p[i + 1]) / 256.0f) * mipSizes[mip];
            uint8_t y = (float(p[i + 2]) / 256.0f) * mipSizes[mip];
            
            uint32_t key = (unsigned(mip) << 16) | (unsigned(x) << 8) | y;
            ref[key]++;
        }
    }

    // copy data to a vector that we can sort
    _result.reserve(ref.size());
    copy(ref.begin(), ref.end(), back_inserter(_result));

    // sort on the count to that the most referenced is at the front
    sort(_result.begin(), _result.end(), [](const pair_t & lhs, const pair_t & rhs) {
        return lhs.second > rhs.second;
    });

    //std::cout << "Analysis yielded: " << _result.size() << std::endl;
}

} // namespace mt
