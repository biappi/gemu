#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "ge.h"
#include "msl.h"
#include "console_socket.h"
#include "peripherical.h"
#include "log.h"

#define MAX_PROGRAM_STORAGE_WORDS 129

void ge_init(struct ge *ge)
{
    memset(ge, 0, sizeof(*ge));
    ge->halted = 1;
    ge->powered = 1;
    ge->register_selector = RS_NORM;
}

void ge_clear(struct ge *ge)
{
    /* The pressure of the "CLEAR" push button only determines the continuous
     * performance of the "00" status (flowcharts fo. 5) */
    ge->rSO = 0;

    /* From Chapter 6.4 "Logic for the timing and for the panel" (cpu fo. 96, 97) */

    ge->AINI = 0;
    ge->ALAM = 0;
    ge->PODI = 0;
    ge->ADIR = 0;
    ge->ACIC = 1;

    /* After the powering on of the machine the timing starts pressing the
     * "Clear" switch (cpu fo. 99). */
    ge->halted = 0;

    /* (One of) the possible set conditions (is): or with
     * CLEAR and.. (cpu fo. 98) */
    ge->ALTO = 0;
}

int ge_load_program(struct ge *ge, uint8_t *program, uint8_t size)
{
    if (program == NULL && size != 0)
        return -1;

    if (size > MAX_PROGRAM_STORAGE_WORDS)
        size = MAX_PROGRAM_STORAGE_WORDS;

    /* simulate the loading for now */
    memcpy(ge->mem, program, size);
    return 0;
}

void ge_load(struct ge *ge)
{
    /* When pressing LOAD button, AINI is set. If AINI is set, the state 80
     * (initialitiation) goes to state c8, starting the loading of the program
     * (of max 129 words) from one of the peripherc unit. */

    /* set AINI FF to 1 (pag. 96) */
    ge->AINI = 1;
}

void ge_start(struct ge *ge)
{
    // From 14023130-0, sheet 5:
    // With the rotating switch in "NORM" position, after the operation
    // "CLEAR-LOAD-START" or "CLEAR-START", the 80 status is performed.

    // TODO: this should be forced using the ARES flip flop?! how
    ge->rSO = 0x80;

    ge->ALTO = 0; /* cpu fo. 97 */
}

static void ge_print_well_known_states(uint8_t state) {
    const char *name;
    switch (state) {
        case 0x00: name = "- Display sequence"; break;
        case 0x08: name = "- Forcing sequence"; break;
        case 0x64:
        case 0x65: name = "- Beta Phase"; break;
        case 0x80: name = "- Initialitiation"; break;
        case 0xE2:
        case 0xE3: name = "- Alpha Phase"; break;
        case 0xF0: name = "- Interruption"; break;
        default:   name = "";
    }

    ge_log(LOG_STATES, "Running state %02x %s\n", state, name);
}

void ge_print_registers(struct ge *ge)
{
    ge_log(LOG_REGS,
           "SO: %02x - PO: %04x - RO: %04x - FO: %04x  -  "
           "V1: %04x  V2: %04x  V3: %04x  V4: %04x - "
           "L1: %04x  L2: %04x L3 : %04x\n",
           ge->rSO, ge->rPO, ge->rRO, ge->rFO,
           ge->rV1, ge->rV2, ge->rV3, ge->rV4,
           ge->rL1, ge->rL2, ge->rL3);
}


void ge_clock_increment(struct ge* ge)
{
    ge->current_clock++;
    if (ge->current_clock == END_OF_STATUS)
        ge->current_clock = TO00;
}

uint8_t ge_clock_is_first(struct ge* ge)
{
    return ge->current_clock == TO00;
}

uint8_t ge_clock_is_last(struct ge* ge)
{
    return ge->current_clock == (END_OF_STATUS - 1);
}

int ge_run_pulse(struct ge *ge)
{
    int r;

    struct msl_timing_state *state = msl_get_state(ge->rSO);

    if (!state) {
        ge_log(LOG_ERR, "no timing charts found for state %02X\n", ge->rSO);
        return 1;
    }

    if (ge_clock_is_first(ge)) {
        ge->old_SO = ge->rSO;

        ge_print_well_known_states(ge->rSO);

        r = ge_peri_on_clock(ge);
        if (r != 0)
            return r;
    }

    /* Execute common pulse machine logic */
    pulse(ge);

    /* Execute peripherals pulse callbacks */
    r = ge_peri_on_pulses(ge);
    if (r != 0)
        return r;

    /* Execute the commands from the timing charts */
    msl_run_state(ge, state);

    if (ge_clock_is_last(ge)) {
        ge_print_registers(ge);

        if (ge->rSO == ge->old_SO) {
            ge_log(LOG_ERR, "State register SO did not change during an entire cycle, stopping emulation\n");
            return 1;
        }
    }

    ge_clock_increment(ge);
    return 0;
}

int ge_run_cycle(struct ge *ge)
{
    do {
        int r = ge_run_pulse(ge);
        if (r)
            return r;
    } while (!ge_clock_is_first(ge));

    return 0;
}

int ge_deinit(struct ge *ge)
{
    ge_peri_deinit(ge);
    return 0;
}
