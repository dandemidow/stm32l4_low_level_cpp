#if !defined(LL_RCC_H_)
#define LL_RCC_H_

#include "reset_clock_control.h"

namespace ll::rcc {

using SysClkSourceStatus = ll::rcc::cfgr::Sws;
using SysClkSource = ll::rcc::cfgr::Sw;
using SysClkDiv = ll::rcc::cfgr::HPre;
using Apb1Div = rcc::cfgr::PPre1;
using Apb2Div = rcc::cfgr::PPre2;

class Msi {
  ResetClockControl &rcc;
 public:
  Msi() : rcc {*new ResetClockControl{}} {}
  inline void Enable() {
    bit::set(rcc.get<rcc::CR>(), kCrMsiOn);
  }
};

static inline bool rcc_msi_is_ready() {
  auto &rcc = *new ResetClockControl{};
  return ((bit::read(rcc.get<rcc::CR>(), kCrMsiRdy) == kCrMsiRdy) ? true : false);
}

static inline void rcc_msi_enable_range_selection() {
  auto &rcc = *new ResetClockControl{};
  bit::set(rcc.get<rcc::CR>(), kCrMsiRgSel);
}

static inline void rcc_msi_set_range(uint32_t range) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::CR>(), kCrMsiRange.value, range);
}

static inline void rcc_msi_set_calib_trimming(uint32_t value) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::ICSCR>(), kIcsCrMsiTrim.value, value << kIcsCrMsiTrim.position);
}

static inline void rcc_set_sys_clk_source(SysClkSource source) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::CFGR>(), rcc::cfgr::kSw.value, static_cast<uint32_t>(source));
}

static inline SysClkSourceStatus rcc_get_sys_clk_source() {
  auto &rcc = *new ResetClockControl{};
  return static_cast<SysClkSourceStatus>(bit::read(rcc.get<rcc::CFGR>(), rcc::cfgr::kSws));
}

static inline void rcc_set_ahb_prescaler(SysClkDiv prescaler) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::CFGR>(), rcc::cfgr::kHPre, static_cast<uint32_t>(prescaler));
}

static inline void rcc_set_apb1_prescaler(Apb1Div prescaler) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::CFGR>(), rcc::cfgr::kPPre1.value, static_cast<uint32_t>(prescaler));
}

static inline void rcc_set_apb2_prescaler(Apb2Div prescaler) {
  auto &rcc = *new ResetClockControl{};
  reg::modify(rcc.get<rcc::CFGR>(), rcc::cfgr::kPPre2.value, static_cast<uint32_t>(prescaler));
}

}  // namespace ll::rcc

#endif
