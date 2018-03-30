#ifndef ANALYSISTASK_H
#define ANALYSISTASK_H

#include "task.h"
#include <stdint.h>
#include <vector>

namespace mt {

///
/// \class  AnalysisTask
///
class AnalysisTask : public tasks::TaskNode
{
public:
    AnalysisTask(const std::vector<uint8_t> & Feedback);

    typedef std::pair<uint32_t, size_t> pair_t;

    /// does the actual work
    virtual void Compute();

    const std::vector<pair_t> & result() const { return _result; }

protected:
    const std::vector<uint8_t> & _feedback;
    std::vector<pair_t>          _result;
};

} // namespace mt

#endif