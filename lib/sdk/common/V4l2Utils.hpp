
#pragma once

#include <cstdint>

#include <linux/videodev2.h>




namespace v4l2 {


extern const char * getCapName(uint32_t cap) noexcept;
extern const char * getColorspaceName(v4l2_colorspace c) noexcept;
extern const char * getControlIdName(uint32_t id) noexcept;
extern const char * getControlTypeName(v4l2_ctrl_type type) noexcept;
extern const char * getControlFlagName(uint32_t flag) noexcept;
extern const char * getControlClassName(uint32_t id) noexcept;
extern const char * getFrameSizeTypeName(v4l2_frmsizetypes type) noexcept;
extern const char * getBufferFlagName(uint32_t flag) noexcept;
extern const char * getBufferTypeName(v4l2_buf_type type) noexcept;


}
