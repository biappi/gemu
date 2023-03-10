#include <stdint.h>
#include "bit.h"
#include "msl-timings.h"

#define MSL_COMMANDS_INCLUDED_BY_MSL_STATES
#include "msl-commands.c"
#undef MSL_COMMANDS_INCLUDED_BY_MSL_STATES

#ifndef MSL_STATES_INCLUDED_BY_MSL_TIMINGS
#   error This file should be include by msl-timings.c and not compiled directly
#endif

/* Common Conditions */
/* ----------------- */

static uint8_t not_RO00(struct ge *ge) { return !BIT(ge->rRO, 0); }
static uint8_t not_RO01(struct ge *ge) { return !BIT(ge->rRO, 1); }
static uint8_t not_RO02(struct ge *ge) { return !BIT(ge->rRO, 2); }
static uint8_t not_RO03(struct ge *ge) { return !BIT(ge->rRO, 3); }
static uint8_t not_RO04(struct ge *ge) { return !BIT(ge->rRO, 4); }
static uint8_t not_RO05(struct ge *ge) { return !BIT(ge->rRO, 5); }
static uint8_t not_RO06(struct ge *ge) { return !BIT(ge->rRO, 6); }
static uint8_t not_RO07(struct ge *ge) { return !BIT(ge->rRO, 7); }


/* Initialitiation */
/* --------------- */

// to state E2+E3 if !AINI
//          C8    if AINI

static uint8_t AINI(struct ge *ge) { return ge->AINI; }
static uint8_t not_AINI(struct ge *ge) { return !AINI(ge); }

static const struct msl_timing_chart state_80[] = {
    { TO30, CI19, 0 },
    { TO30, CO96, 0 },
    { TO30, CO97, 0 },
    { TO40, CO00, 0 },
    { TO40, CO02, 0 },
    { TO50, CI32, 0 },
    { TI05, CI05, 0 },
    { TI05, CI08, 0 },
    { TI06, CI76, 0 },
    { TI06, CI80, 0 },
    { TI06, CI81, 0 },
    { TI06, CI82, 0 },
    { TI06, CU01, not_AINI },
    { TI06, CU03, AINI },
    { TI06, CU05, not_AINI },
    { TI06, CU06, 0 },
    { END_OF_STATUS, 0, 0 }
};

// Alpha phase

// (to state F0 if RINT & !FA06
//           E0 if !RINT | FA06)

static uint8_t state_E2_E3_TO80_CI89(struct ge *ge) {
    /* (deltaRO = HLT + ASIN(ATOC+!ADIR)) */
    return ge->rRO == HLT_OPCODE;
}

static uint8_t state_E2_E3_TI06_CI82(struct ge *ge) {
    return ge->rRO == PER_OPCODE;
}

static uint8_t state_E2_E3_TI06_CU04(struct ge *ge) {
    return ge->RINT && !BIT(ge->ffFA, 6);
}

static const struct msl_timing_chart state_E2_E3[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO02, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO80, CI89, state_E2_E3_TO80_CI89 },
    { TI05, CI08, 0 },
    { TI06, CI80, 0 },
    { TI06, CI82, state_E2_E3_TI06_CI82 },
    { TI06, CI83, 0 },
    { TI06, CU04, state_E2_E3_TI06_CU04 },
    { TI06, CU10, 0 },
    { TI06, CU11, 0 },
    { END_OF_STATUS, 0, 0 }
};

// to state E4    if FO06 | FO07
//          64+65 if !(FO06 | FO07)

static uint8_t state_E0_TI06_CU17(struct ge *ge) {
    return !(BIT(ge->rFO, 6) || BIT(ge->rFO, 7));
}

static const struct msl_timing_chart state_E0[] = {
    { TO10, CO12, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO80, CI39, 0 },
    { TI05, CI05, 0 },
    { TI06, CU02, 0 },
    { TI06, CU17, state_E0_TI06_CU17 },
    { END_OF_STATUS, 0, 0 }
};

// to state E6

static uint8_t state_E4_TO70_CI60(struct ge *ge) { return 0; }

static const struct msl_timing_chart state_E4[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO70, CI65, 0 },
    { TO70, CI60, state_E4_TO70_CI60 },
    { TI05, CI02, 0 },
    { TI06, CI06, 0 },
    { TI06, CU01, 0 },
    { END_OF_STATUS, 0, 0 }
};

// to state E5 if !L207 & (FO07 & FO06)
//          ED+EC if L207
//          64+65 if !L207 & (!FO07 | !FO06)

static uint8_t state_E6_TO80_CI38(struct ge *ge) { /* DO01? */ return 0; }
static uint8_t state_E6_TI06_CU03(struct ge *ge) { return BIT(ge->rL2, 7); }

static uint8_t state_E6_TI06_CU17(struct ge *ge) {
    return (!BIT(ge->rL2, 7) &&
            (!BIT(ge->rFO, 7) || !BIT(ge->rFO, 6)));
}

static const struct msl_timing_chart state_E6[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO30, CI12, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO80, CI38, state_E6_TO80_CI38 },
    { TI05, CI01, 0 },
    { TI05, CI02, 0 },
    { TI06, CU00, 0 },
    { TI06, CU03, state_E6_TI06_CU03 },

    /* in the manual this is CU10, but it maybe a mistake.. there's no way to reach
     * the alpha states if we don't reset this bit 1 instead of bit 0 */
    { TI06, CU11, 0 },

    { TI06, CU17, state_E6_TI06_CU17 },
    { END_OF_STATUS, 0, 0 }
};

// to state E7

static const struct msl_timing_chart state_E5[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO70, CI65, 0 },
    { TO70, CI60, not_RO07 },
    { TI05, CI02, 0 },
    { TI06, CI06, 0 },
    { TI06, CU01, 0 },
    { END_OF_STATUS, 0, 0 }
};

// to state 64+65 if !L207
//          ED+EC if L207

static uint8_t state_E7_TO80_CI38(struct ge *ge) { return 1; /* DO01 ?!? */ }
static uint8_t state_E7_TI06_CU03(struct ge *ge) { return BIT(ge->rL2, 7); }

static uint8_t state_E7_TI06_CU17(struct ge *ge) {
    return BIT(ge->rL2, 7) && (BIT(ge->rFO, 7) || BIT(ge->rFO, 6));
}

static const struct msl_timing_chart state_E7[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO30, CI12, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO80, CI38, state_E7_TO80_CI38 },
    { TI05, CI02, 0 },
    { TI06, CU00, 0 },
    { TI06, CU03, state_E7_TI06_CU03 },
    { TI06, CU10, 0 },
    { TI06, CU17, state_E7_TI06_CU17 },
    { END_OF_STATUS, 0, 0 }
};

/* Beta Phase */
/* ---------- */

static uint8_t jc_js1_js2_jie(struct ge *ge) {
    return ((ge->rFO == JC_OPCODE) ||
            (ge->rFO == JS1_OPCODE && ge->rL1 == JS1_2NDCHAR) ||
            (ge->rFO == JS2_OPCODE && ge->rL1 == JS2_2NDCHAR) ||
            (ge->rFO == JIE_OPCODE && ge->rL1 == JIE_2NDCHAR));
}

static uint8_t lon_loll(struct ge *ge) {
    return ((ge->rFO == LON_OPCODE && ge->rL1 == LON_2NDCHAR) ||
            (ge->rFO == LOLL_OPCODE && ge->rL1 == LOLL_OPCODE));
}

static uint8_t ins(struct ge *ge) {
    return ge->rFO == INS_OPCODE && ge->rL1 == INS_2NDCHAR;
}

static uint8_t jie(struct ge *ge) {
    return ge->rFO == JIE_OPCODE && ge->rL1 == JIE_2NDCHAR;
}

static uint8_t ens(struct ge *ge) {
    return ge->rFO == ENS_OPCODE && ge->rL1 == ENS_2NDCHAR;
}

static uint8_t loff(struct ge *ge) {
    return ge->rFO == LOFF_OPCODE && ge->rL1 == LOFF_2NDCHAR;
}

static uint8_t state_64_65_TI05_CI00(struct ge *ge) {
    return ge->AVER && jc_js1_js2_jie(ge);
}

static const struct msl_timing_chart state_64_65[] = {
    { TO10, CO10, jc_js1_js2_jie },
    { TO20, CI87, lon_loll },
    { TO20, CI77, ins },
    { TO30, CI12, jc_js1_js2_jie },
    { TO40, CO01, jc_js1_js2_jie },
    { TO60, CO35, jie },
    { TO65, CO49, 0 },
    { TO70, CI78, ens },
    { TO89, CI88, loff },
    { TI05, CI00, state_64_65_TI05_CI00 },
    { TI06, CU01, 0 },
    { TI06, CU10, 0 },
    { TI06, CU07, 0 },
    { TI06, CU12, 0 },
    { END_OF_STATUS, 0, 0 }
};

/* Display */
/* ------- */

static uint8_t state_80_TO10_CO10(struct ge *ge) { return AF32(ge) || AF42(ge); }
static uint8_t state_80_TO10_CO11(struct ge *ge) { return AF31(ge) || AF41(ge) || AF51(ge); }
static uint8_t state_80_TO30_CI15(struct ge *ge) { return !AF20(ge) && !AF40(ge); }
static uint8_t state_80_TO50_CI33(struct ge *ge) { return !AF20(ge) && !AF21(ge) && !AF40(ge); }

static const struct msl_timing_chart state_00[] = {
    { TO10, CO10, state_80_TO10_CO10 }, /* RS_NORM or RS_PO */
    { TO10, CO11, state_80_TO10_CO11 }, /* RS_V1 or RS_V1_SCR or RS_V1_LETT */
    { TO10, CO12, AF50 },               /* RS_V2 */
    { TO10, CO13, AF30 },               /* RS_V3 */
    { TO10, CO14, AF10 },               /* RS_V4 */
    { TO30, CI15, state_80_TO30_CI15 }, /* not RS_L3 and not RS_R1_L2 */
    { TO30, CI17, AF20 },               /* RES_L3 */
    { TO30, CI21, AF40 },               /* RS_R1_R2 */
    { TO30, CI16, AF40 },               /* RS_V1_SCR */
    { TO50, CI33, state_80_TO50_CI33 }, /* not RS_L3 and not RS_L1 and not RS_R1_L2 */
    { TI06, CU07, 0 },
    { END_OF_STATUS, 0, 0 }
};

/* Forcing */
/* ------- */

static uint8_t AF52_not_RO00(struct ge *ge) { return AF52(ge) && not_RO00(ge); }
static uint8_t AF52_not_RO01(struct ge *ge) { return AF52(ge) && not_RO01(ge); }
static uint8_t AF52_not_RO02(struct ge *ge) { return AF52(ge) && not_RO02(ge); }
static uint8_t AF52_not_RO03(struct ge *ge) { return AF52(ge) && not_RO03(ge); }
static uint8_t AF52_not_RO04(struct ge *ge) { return AF52(ge) && not_RO04(ge); }
static uint8_t AF52_not_RO05(struct ge *ge) { return AF52(ge) && not_RO05(ge); }
static uint8_t AF52_not_RO06(struct ge *ge) { return AF52(ge) && not_RO06(ge); }
static uint8_t AF52_not_RO07(struct ge *ge) { return AF52(ge) && not_RO07(ge); }

static const struct msl_timing_chart state_08[] = {
    { TO10, CO11, AF41 }, /* fo. 18 */
    { TO10, CO11, AF51 },
    { TO10, CO41, 0 },
    { TO25, CO30, AF51 },
    { TO25, CO31, AF41 },
    { TO30, CI20, 0 },
    { TO40, CO01, AF41 },
    { TO40, CO01, AF51 },
    { TO50, CO48, AF52 },
    /* NO -> BO */
    { TO50, CI33, AF41 },
    { TO50, CI33, AF43 },
    { TO64, CO49, AF52_not_RO07 },
    { TO70, CI62, AF51 },
    { TO70, CI67, AF51 }, /* fo. 19 */
    { TI05, CI04, AF10 },
    { TI05, CI02, AF50 },
    { TI05, CI05, AF21 },
    { TI05, CI05, AF51 },
    { TI05, CI01, AF31 },
    { TI05, CI00, AF42 },
    { TI05, CI08, AF53 },
    { TI06, CI07, AF20 },
    { TI06, CI03, AF30 },
    { TI06, CI06, AF40 },
    { TI06, CI09, AF40 },
    { TI06, CI70, AF52 },
    { TI06, CI71, AF52 },
    { TI06, CI72, AF52 },
    { TI06, CI73, AF52 }, /* fo. 20 */
    { TI06, CI74, AF52 },
    { TI06, CI75, AF52 },
    { TI06, CI76, AF52 },
    { TI06, CI80, AF52_not_RO00 },
    { TI06, CI81, AF52_not_RO01 },
    { TI06, CI82, AF52_not_RO02 },
    { TI06, CI83, AF52_not_RO03 },
    { TI06, CI84, AF52_not_RO04 },
    { TI06, CI85, AF52_not_RO05 },
    { TI06, CI86, AF52_not_RO06 },
    { TI06, CU00, 0 },
    { TI06, CU01, 0 },
    { TI06, CU02, 0 },
    { TI06, CU03, 0 },
    { TI06, CU04, 0 },
    { TI06, CU05, 0 },
    { TI06, CU06, 0 },
    { TI06, CU07, 0 },
    { TI06, CU10, not_RO00 }, /* fo. 21 */
    { TI06, CU11, not_RO01 },
    { TI06, CU12, not_RO02 },
    { TI06, CU13, not_RO03 },
    { TI06, CU14, not_RO04 },
    { TI06, CU15, not_RO05 },
    { TI06, CU16, not_RO06 },
    { TI06, CU17, not_RO07 },
    { END_OF_STATUS, 0, 0 }
};
