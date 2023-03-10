/**
 * @file  signals.h
 * @brief Signals
 *
 * This file implements the combinatorial signals of the machine as
 * inline helpers.
 */

#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdint.h>
#include "bit.h"
#include "ge.h"

/**
 * @defgroup priority-network Cycle attribution priority network
 * @{
 */

/**
 * Cycle assigned to channel 1
 */
static inline uint8_t RES0(struct ge *ge) {
    /* cpu fo. 116 */
    return ge->RESI;
}

/**
 * Cycle assigned to channel 2
 */
static inline uint8_t RES2(struct ge *ge) {
    /* cpu fo. 116 */
    /* maybe this equation is incorrect in manual? it's
     * documented as `!RIA2`, but it seems it should not
     * be negated. */
    return !ge->RIA3 & !ge->RESI & ge->RIA2;
}

/**
 * Cycle assigned to channel 3
 */
static inline uint8_t RES3(struct ge *ge) {
    /* cpu fo. 115 */
    return ge->RIA3 & !ge->RESI;
}

/**
 * Cycle assigned to CPU
 */
static inline uint8_t RIUC(struct ge *ge) {
    return ge->RIA0 & !ge->RESI & !ge->RIA3 & !ge->RIA2;
}

/** @} */

/**
 * DC16 - Jump Condition Verified
 *
 * This signal is true when the currently executing jump condition is
 * verified.
 */
static inline uint8_t verified_condition(struct ge *ge) {
    /* cpu fo 56, 57 */
    uint8_t M = ge->rL1;
    uint8_t M7 = BIT(M, 7);
    uint8_t M6 = BIT(M, 6);
    uint8_t M5 = BIT(M, 5);
    uint8_t M4 = BIT(M, 4);

    uint8_t FA5 = BIT(ge->ffFA, 5);
    uint8_t FA4 = BIT(ge->ffFA, 4);

    return (((ge->rFO == JC_OPCODE) &&
             ((M7 && !FA4 && !FA5) ||
              (M6 && !FA4 &&  FA5) ||
              (M5 &&  FA4 && !FA5) ||
              (M4 &&  FA4 &&  FA5))) ||
            (ge->rFO == JS1_OPCODE && ge->rL1 == JS1_2NDCHAR && ge->JS1) ||
            (ge->rFO == JS2_OPCODE && ge->rL1 == JS2_2NDCHAR && ge->JS2) ||
            0);
}

/**
 * @defgroup selector Console Register Selector Signals
 * @{
 */

/** Selected register V4 */
static inline uint8_t AF10(struct ge *ge) { return ge->register_selector == RS_V4;      }

/** Selected register L3 */
static inline uint8_t AF20(struct ge *ge) { return ge->register_selector == RS_L3;      }

/** Selected register L1 */
static inline uint8_t AF21(struct ge *ge) { return ge->register_selector == RS_L1;      }

/** Selected register V3 */
static inline uint8_t AF30(struct ge *ge) { return ge->register_selector == RS_V3;      }

/** Selected register V1 */
static inline uint8_t AF31(struct ge *ge) { return ge->register_selector == RS_V1;      }

/** Selected normal operation */
static inline uint8_t AF32(struct ge *ge) { return ge->register_selector == RS_NORM;    }

/** Selected register R1 - L2 */
static inline uint8_t AF40(struct ge *ge) { return ge->register_selector == RS_R1_L2;   }

/** Selected register V1 - SCR */
static inline uint8_t AF41(struct ge *ge) { return ge->register_selector == RS_V1_SCR;  }

/** Selected register PO */
static inline uint8_t AF42(struct ge *ge) { return ge->register_selector == RS_PO;      }

/** Selected register SO */
static inline uint8_t AF43(struct ge *ge) { return ge->register_selector == RS_SO;      }

/** Selected register V2 */
static inline uint8_t AF50(struct ge *ge) { return ge->register_selector == RS_V2;      }

/** Selected register V1 - LETT */
static inline uint8_t AF51(struct ge *ge) { return ge->register_selector == RS_V1_LETT; }

/** Selected register FI-UR */
static inline uint8_t AF52(struct ge *ge) { return ge->register_selector == RS_FI_UR;   }

/** Selected register FO */
static inline uint8_t AF53(struct ge *ge) { return ge->register_selector == RS_FO;      }
/** @} */

static inline uint16_t ge_counting_network_output(struct ge *ge) {
    if (ge->counting_network.cmds.from_zero) {
        return ge->rBO + 1;
    }
    else {
        return ge->rBO;
    }
}

/**
 * @defgroup knots Knots
 * @{
 */

/**
 * Knot driven by P0, V1, V2, V4, L1, R1, V3 and L3.
 *
 * In addition, the NO knot contains:
 *   - the forcings from program
 *   - the signals of forcing from console (AM switches)
 */
static inline uint16_t NO_knot(struct ge *ge)
{
    switch (ge->kNO.cmd) {
        case KNOT_PO_IN_NO:       return ge->rPO;
        case KNOT_V1_IN_NO:       return ge->rV1;
        case KNOT_V2_IN_NO:       return ge->rV2;
        case KNOT_V3_IN_NO:       return ge->rV3;
        case KNOT_V4_IN_NO:       return ge->rV4;
        case KNOT_L1_IN_NO:       return ge->rL1;
        case KNOT_L2_IN_NO:       return ge->rL2;
        case KNOT_L3_IN_NO:       return ge->rL3;
        case KNOT_FORCE_IN_NO_21: return ge->kNO.forcings;
        case KNOT_FORCE_IN_NO_43: return ge->kNO.forcings << 8;
        case KNOT_AM_IN_NO:       return ge->console_switches.AM;
        case KNOT_RI_IN_NO_43:    return ge->rRI << 8;
    }
}

/**
 * Knot driven by SO or SI. Content is stored to SA.
 *
 * Driven
 *  - by the SO register when the work cycle has been attributed to the
 *    CPU or to channel 1, if the rotary switch is the "central" position
 *    (AF326=1)
 *  - by the SI register (less four significant bits) when the cycle has
 *    been attributed to channel 2
 *
 * Additionally, individual bits might be set
 *  - NA00: forced to 1 when the work cycle is attributed to channel 1 or
 *    channel 3
 *  - NA03: forced to 1 when the work cycle has been attributed to the CPU
 *    and the rotary switch is not in the "central" position (AF32C = 1))
 *
 * Stored in SA register during T010. (cpu fo. 128)
 */
static inline uint8_t NA_knot(struct ge *ge) {
    uint8_t na = 0;

    if (RES0(ge) || (RIUC(ge) && AF32(ge)))
        na = ge->rSO;

    if (RES2(ge))
        na = ge->rSI & 0x0f;

    if (RES0(ge) || RES3(ge))
        na = na | 0x01;

    if (RIUC(ge) && !AF32(ge))
        na = na | 0x08;

    return na;
}

static inline uint8_t NI_source(struct ge *ge, enum knot_ni_source source) {
    uint8_t is_cn = ((ge->kNI.ni1 == NS_CN1) &&
                     (ge->kNI.ni2 == NS_CN2) &&
                     (ge->kNI.ni3 == NS_CN3) &&
                     (ge->kNI.ni4 == NS_CN4));

    uint16_t cn = is_cn ? ge_counting_network_output(ge) : 0;

    switch (source) {
        case NS_CN1: return (cn & 0x000f) >>  0;
        case NS_CN2: return (cn & 0x00f0) >>  4;
        case NS_CN3: return (cn & 0x0f00) >>  8;
        case NS_CN4: return (cn & 0xf000) >> 12;
        case NS_RO1: return (ge->rRO & 0x0f) >> 0;
        case NS_RO2: return (ge->rRO & 0xf0) >> 4;
        case NS_UA2: return 0;
        case NS_UA1: return 0;
    }
}

/**
 * NI Knot
 *
 * It may be driven by:
 *  - the outputs of the counting network. This occurs always during the
 *    1st phase. During the 2nd phase the driving occurs only if the
 *    corresponding commands of RO in NI and of UA in NI are not present.
 *
 *  - the 1st or And part of RO which may be transferred in anyone of the 4
 *    parts of NI (CI60 - CI67) during the 2nd phase;
 *
 *  - the UA output always during the 2nd phase. The UA may drive the 8 most
 *    significant (CI68) or the 8 less significant (CI69) bits. The same
 *    commands determine also which part of BO must drive the UA to perform
 *    the operation.
 *
 * The quartets of UA may drive the NI knot only if the corresponding RO
 * commands in NI are not present. These in fact have the highest priority.
 * The priority of the various load operations is obtained acting on the
 * signals generation (cpu fo. 125, 126).
 */
static inline uint16_t NI_knot(struct ge *ge) {
    uint16_t ni1 = NI_source(ge, ge->kNI.ni1);
    uint16_t ni2 = NI_source(ge, ge->kNI.ni2);
    uint16_t ni3 = NI_source(ge, ge->kNI.ni3);
    uint16_t ni4 = NI_source(ge, ge->kNI.ni4);

    return ((ni4 << 12) |
            (ni3 <<  8) |
            (ni2 <<  4) |
            (ni1 <<  0));
}

/** @} */

#endif
