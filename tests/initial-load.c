#include "utest.h"
#include "../ge.h"
#include "../bit.h"

#define ASSERT_CYCLE(x, name) \
    do {                      \
        printf("\n" name "\n\n"); \
        ASSERT_EQ(g.rSO, x);  \
        ge_run_cycle(&g);     \
    } while (0)

UTEST(load, initial_load) {
    struct ge g;
    struct ge_console c;
    
    ge_init(&g);
    ge_clear(&g);
    ge_load(&g);
    ge_start(&g);

    ASSERT_CYCLE(0x00, "Display");
    ASSERT_CYCLE(0x80, "Initialisation");
    ASSERT_TRUE(BIT(g.ffFI, 6)); /* always true */
    ASSERT_EQ(g.ffFI, 0x40);

    ASSERT_CYCLE(0xc8, "PER-PERI 1");
    ASSERT_FALSE(BIT(g.rL2, 6)); /* false == input transfer */
    ASSERT_FALSE(BIT(g.rL2, 5)); /* false == increasing addresses */
    ASSERT_FALSE(BIT(g.rL2, 4)); /* false == input with packing */
    ASSERT_FALSE(BIT(g.rL2, 3)); /* false == channel 1 or 3 */
    ASSERT_FALSE(BIT(g.rL2, 2)); /* false == wait for peripheral availability */
    ASSERT_FALSE(BIT(g.rL2, 1)); /* always false  */
    ASSERT_FALSE(BIT(g.rL2, 0)); /* false == channel 1 or 2 */
    ASSERT_EQ(g.rL2, 0x00);

    ASSERT_FALSE(BIT(g.ffFI, 4)); /* always false */
    ASSERT_FALSE(BIT(g.ffFI, 5)); /* connector and channel not busy */

    ASSERT_CYCLE(0xd8, "PER-PERI 2");
    ASSERT_EQ(g.rRE, 0xc0); /* peripheral command(?) for "load 1" initial load */

    ASSERT_CYCLE(0xd9, "PER-PERI 3");
    ASSERT_CYCLE(0xda, "PER-PERI 4");
    ASSERT_CYCLE(0xdb, "PER-PERI 5");
    ASSERT_TRUE(BIT(g.ffFI, 4)); /* always true */
    ASSERT_EQ(g.ffFI, 0x50);

    ASSERT_CYCLE(0xdc, "PER-PERI 6");
    ASSERT_EQ(g.rV1, g.rV3);

    ASSERT_CYCLE(0xcc, "PER-PERI 7");
    ASSERT_EQ(g.rRE, 0x40);
    ASSERT_EQ(g.rRA, 0x40);

    ASSERT_CYCLE(0xca, "TPER-CPER 1");

    ASSERT_CYCLE(0xa8, "TPER-CPER 2");
    ASSERT_EQ((g.rL1 & 0xff00) >> 8, 0);

    ASSERT_CYCLE(0xa9, "TPER-CPER 3");
    ASSERT_EQ(g.rL1, 0);

    ASSERT_CYCLE(0xaa, "TPER-CPER 4");
    ASSERT_EQ((g.rV1 & 0xff00) >> 8, 0);

    ASSERT_CYCLE(0xab, "TPER-CPER 5");
    ASSERT_EQ(g.rV1, 0);

    ASSERT_TRUE(g.RIA0);
    ASSERT_FALSE(g.RESI);
    ASSERT_FALSE(g.RIA2);
    ASSERT_FALSE(g.RIA3);

    ASSERT_CYCLE(0xb8, "TPER-CPER 6");
    ASSERT_TRUE(BIT(g.ffFI, 0)); /* always true */
    ASSERT_EQ(g.ffFI, 0x51);

    reader_setup_to_send(&g, 0xAB, 0);

    /* actually state is b9 but it's not in SO, so the assert is misleading */
    ASSERT_CYCLE(0xb8 /* b9 */, "TPER INPUT 1 - 1");
    ASSERT_EQ(g.mem[0], 0xAB); /* loading of first nibble from reader */
    ASSERT_EQ(g.rV1, 0x00); /* dst pointer should not advance now */

    reader_clear_sending(&g);
    ASSERT_CYCLE(0xb1, "TPER-INPUT 2 - 1");
    ASSERT_TRUE(BIT(g.ffFI, 1)); /* received first nibble */
    ASSERT_EQ(g.ffFI, 0x53);

    reader_setup_to_send(&g, 0xCD, 0);
    ASSERT_CYCLE(0xb8 /* b9 */, "TPER INPUT 1 - 2");

    reader_clear_sending(&g);
    ASSERT_CYCLE(0xb1, "TPER-INPUT 2 - 2");
    ASSERT_TRUE(BIT(g.ffFI, 0)); /* received second nibble */
    ASSERT_EQ(g.ffFI, 0x51);
    ASSERT_EQ(g.mem[0], 0xBD); /* load second nibble from reader */
    ASSERT_EQ(g.rV1, 0x01); /* advance dst pointer  */

    reader_setup_to_send(&g, 0xEF, 0);
    ASSERT_CYCLE(0xb8 /* b9 */, "TPER INPUT 1 - 3");
    ASSERT_EQ(g.mem[1], 0xEF); /* loading of first nibble from reader */
    ASSERT_EQ(g.rV1, 0x01); /* dst pointer should not advance now */

    reader_clear_sending(&g);
    ASSERT_CYCLE(0xb1, "TPER-INPUT 2 - 3");
    ASSERT_TRUE(BIT(g.ffFI, 1)); /* received first nibble */
    ASSERT_EQ(g.ffFI, 0x53);

    reader_setup_to_send(&g, 0xAA, 1);
    ASSERT_CYCLE(0xb8 /* b9 */, "TPER INPUT 1 - 4");

    reader_clear_sending(&g);
    ASSERT_CYCLE(0xb1, "TPER-INPUT 2 - 4");
    ASSERT_TRUE(BIT(g.ffFI, 0)); /* received second nibble */
    ASSERT_EQ(g.ffFI, 0x51);
    ASSERT_EQ(g.mem[1], 0xFA); /* load second nibble from reader */
    ASSERT_EQ(g.rV1, 0x02); /* advance dst pointer  */

    ASSERT_CYCLE(0xb8, "WAIT 1");
    ASSERT_CYCLE(0xea, "TPER END 1");
    ASSERT_CYCLE(0xeb, "TPER END 2");
    ASSERT_CYCLE(0xe3, "Alpha 1");
}
