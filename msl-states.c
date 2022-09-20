#include <stdint.h>
#include "msl-commands.h"
#include "msl-states.h"

// Initialitiation

// to state E2+E3 if !AINI
//          C8    if AINI

uint8_t state_80_TI06_CU01(struct ge *ge) { return !ge->AINI; }
uint8_t state_80_TI06_CU03(struct ge *ge) { return ge->AINI; }
uint8_t state_80_TI06_CU05(struct ge *ge) { return !ge->AINI; }

const struct msl_timing_chart state_80[] = {
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
    { TI06, CU01, state_80_TI06_CU01 },
    { TI06, CU03, state_80_TI06_CU03 },
    { TI06, CU05, state_80_TI06_CU05 },
    { TI06, CU06, 0 },
    { MAX_CLOCK, 0 }
};

// Alpha phase

// (to state F0 if RINT & !FA06
//           E0 if !RINT | FA06)

uint8_t state_E2_E3_TO80_CI89(struct ge *ge) { return 0; }
uint8_t state_E2_E3_TI06_CI82(struct ge *ge) { return 0; }
uint8_t state_E2_E3_TI06_CU04(struct ge *ge) { return 0; }

const struct msl_timing_chart state_E2_E3[] = {
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
    { MAX_CLOCK, 0 }
};

// to state E4    if FO06 | FO07
//          64+65 if !(FO06 | FO07)

uint8_t state_E0_TO70_CI60(struct ge *ge) { return 0; }
uint8_t state_E0_TI06_CU17(struct ge *ge) { return 1; }

const struct msl_timing_chart state_E0[] = {
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
    { MAX_CLOCK, 0 }
};

// to state E6

uint8_t state_E6_TI06_CU03(struct ge *ge) { return 1; }
uint8_t state_E6_TI06_CU17(struct ge *ge) { return 1; }
uint8_t state_E6_TO80_CI38(struct ge *ge) { return 1; }

const struct msl_timing_chart state_E4[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO70, CI65, 0 },
    { TO70, CI60, state_E0_TO70_CI60 },
    { TI05, CI02, 0 },
    { TI06, CI06, 0 },
    { TI06, CU01, 0 },
    { MAX_CLOCK, 0 }
};

// to state E5 if !L207 & (FO07 & FO06)
//          ED+EC if L207
//          64+65 if !L207 & (!FO07 | !FO06)

const struct msl_timing_chart state_E6[] = {
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
    { TI06, CU10, 0 },
    { TI06, CU17, state_E6_TI06_CU17 },
    { MAX_CLOCK, 0 }
};

// to state E7

uint8_t state_E5_TI06_CU17(struct ge *ge) { return 1; }
uint8_t state_E5_TO70_CI60(struct ge *ge) { return 1; }
uint8_t state_E5_TO80_CI31(struct ge *ge) { return 1; }

const struct msl_timing_chart state_E5[] = {
    { TO10, CO10, 0 },
    { TO10, CO41, 0 },
    { TO25, CO30, 0 },
    { TO40, CO00, 0 },
    { TO70, CI67, 0 },
    { TO70, CI62, 0 },
    { TO70, CI65, 0 },
    { TO70, CI60, state_E5_TO70_CI60 },
    { TI05, CI02, 0 },
    { TI06, CI06, 0 },
    { TI06, CU01, 0 },
    { MAX_CLOCK, 0 }
};

// to state 64+65 if !L207
//          ED+EC if L207

uint8_t state_E7_TO80_CI38(struct ge *ge) { return 1; }
uint8_t state_E7_TI06_CU17(struct ge *ge) { return 1; }

const struct msl_timing_chart state_E7[] = {
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
    { TI06, CU03, 0 },
    { TI06, CU10, 0 },
    { TI06, CU17, state_E7_TI06_CU17 },
    { MAX_CLOCK, 0 }
};

