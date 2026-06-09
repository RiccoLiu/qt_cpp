#ifndef PIPELINE_SYS_MSG_H
#define PIPELINE_SYS_MSG_H

#include "pipeline_msg.h"

#ifndef PIPELINE_FOURCC
#define PIPELINE_FOURCC(a, b, c, d) \
((((uint32_t)a) << 0) | (((uint32_t)b) << 8) | (((uint32_t)c) << 16) | (((uint32_t)d) << 24))
#endif

#define PIPELINE_SYSMSG PIPELINE_FOURCC('s', 'y', 's', '\0')

enum { PIPELINE_SYSTEM_STARTUP = 1, PIPELINE_SYSTEM_COUNT };

class PipelineSysMsg : public PipelineMsg {
public:
    PipelineSysMsg(std::weak_ptr<PipelineSource> &source) : PipelineMsg(source) {
        fourcc_ = PIPELINE_SYSMSG;
    }
    virtual ~PipelineSysMsg() {}

    void *GetArg1() {
        return arg1_;
    }
    void *GetArg2() {
        return arg2_;
    }
    int GetType() {
        return type_;
    }
    void Set(int t, void *arg1, void *arg2) {
        type_ = t;
        arg1_ = arg1;
        arg2_ = arg2;
    }

    static const char *ClassName() {
        return "PipelineSysMsg";
    }

protected:
    int type_;
    void *arg1_;
    void *arg2_;

private:
    DISALLOW_COPY_AND_ASSIGN(PipelineSysMsg);
};


#endif // PIPELINE_SYS_MSG_H
