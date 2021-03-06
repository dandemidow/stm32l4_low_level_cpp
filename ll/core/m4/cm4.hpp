#if !defined(CORE_CM4_HPP_)
#define CORE_CM4_HPP_

#include "register/nested_vectored_interrupt_controller.hpp"
#include "register/system_control_block.h"
#include "irqs.h"

namespace ll::nvic {

using address::nvic::PriorityGroup;

static inline void set_priority_grouping(PriorityGroup priority_group) {
  auto &scb = *new SystemControlBlock{};
  uint32_t priority_group_tmp = (static_cast<uint32_t>(priority_group) & (uint32_t)0x07u);            /* only values 0..7 are used          */

  uint32_t reg_value = scb.get<scb::AIRCR>();                                  /* read old register configuration    */
  reg_value &= ~((uint32_t)(kScbAircrVectKey.mask | kScbAircrPriGroup.mask));  /* clear bits to change               */
  reg_value  =  (reg_value |
                ((uint32_t)0x5faul << kScbAircrVectKey.position) |
                (priority_group_tmp << kScbAircrPriGroup.position));           /* Insert write key and priority group */
  scb.set<scb::AIRCR>(reg_value);
}

static inline PriorityGroup get_priority_grouping() {
  auto &scb = *new SystemControlBlock{};
  return static_cast<PriorityGroup>(scb.And<scb::AIRCR>(kScbAircrPriGroup.mask) >> kScbAircrPriGroup.position);
}

static inline uint32_t encode_priority (PriorityGroup priority_group, uint32_t PreemptPriority, uint32_t SubPriority) {
  using address::nvic::kPrioBits;
  uint32_t priority_group_tmp = (static_cast<uint32_t>(priority_group) & (uint32_t)0x07UL);   /* only values 0..7 are used          */
  uint32_t preempt_priority_bits = ((7UL - priority_group_tmp) > kPrioBits) ? kPrioBits : (uint32_t)(7UL - priority_group_tmp);
  uint32_t sub_priority_bits     = ((priority_group_tmp + kPrioBits) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((priority_group_tmp - 7UL) + kPrioBits);

  return (
           ((PreemptPriority & (uint32_t)((1UL << (preempt_priority_bits)) - 1UL)) << sub_priority_bits) |
           ((SubPriority     & (uint32_t)((1UL << (sub_priority_bits    )) - 1UL)))
         );
}

static inline void set_priority(IRQn_Type IRQn, uint32_t priority = encode_priority(get_priority_grouping(), 0u, 0u)) {
  using address::nvic::kPrioBits;
  if (static_cast<int>(IRQn) >= 0) {
    auto &nvic = *new NestedVectoredInterruptController{};
    nvic.get<nvic::IP>()[static_cast<uint32_t>(IRQn)] = (uint8_t)((priority << (8u - kPrioBits)) & 0xffu);
  } else {
    auto &scb = *new SystemControlBlock{};
    scb.get<scb::SHP>()[(static_cast<uint32_t>(IRQn) & 0xful)-4ul] = (uint8_t)((priority << (8u - kPrioBits)) & (uint32_t)0xfful);
  }
}

static inline void enable_irq(IRQn_Type IRQn) {
  auto &nvic = *new NestedVectoredInterruptController{};
  nvic.get<ISER>()[static_cast<uint32_t>(IRQn) >> 5ul] = static_cast<uint32_t>(1UL << (static_cast<uint32_t>(IRQn) & 0x1ful));
}

}  // namespace ll::nvic
#endif
