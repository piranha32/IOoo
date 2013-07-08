.origin 0
.entrypoint START

#define GPIO0 0x44E07000
#define GPIO1 0x4804c000
#define GPIO2 0x481AC000
#define GPIO3 0x481AE000

#define DATA_OUT_REG    0x13C
#define DATA_IN_REG     0x138
#define GPIO_OE_REG     0x134
//spru73h pg 4093
#define DATA_CLEAR_REG  0x190
//spru73h pg 4094
#define DATA_SET_REG    0x194



//#define GSCLK_PORT GPIO1
//#define GSCLK_BIT 18

//#define BLANK_PORT GPIO0
//#define BLANK_BIT 2


#define GSCLK_PORT GPIO1
#define GSCLK_BIT 19

#define BLANK_PORT GPIO0
#define BLANK_BIT 5

.macro NOP
        mov r1, r1
.endm


// 8 clock delay + 2 check
.macro BLANK_DELAY
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
.endm

.macro GSCLK_DELAY1
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
.endm

.macro GSCLK_DELAY0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
.endm

START:
        // enable master ocp
        LBCO r0, C4, 4, 4
        CLR r0, r0, 4
        SBCO r0, C4, 4, 4

//r0 - gsclk pulse counter
//r1 - gsclk bit
//r2 - blank bit
//r3 - gpio address
        mov r1,1<<GSCLK_BIT
        mov r2,1<<BLANK_BIT
//enable outputs
//gsclk
        mov r3, GSCLK_PORT|GPIO_OE_REG
        lbbo r4,r3,0,4
        clr r4,GSCLK_BIT
        sbbo r4,r3,0,4
//blank
        mov r3, BLANK_PORT|GPIO_OE_REG
        lbbo r4,r3,0,4
        clr r4,BLANK_BIT
        sbbo r4,r3,0,4

main_loop:
        mov r0,4096

gsclk_loop:
        //set GSCLK bit
        mov r3, GSCLK_PORT|DATA_SET_REG
        sbbo r1,r3,0,4
        //wait for high pulse time
        //GSCLK_DELAY1

        //clear GSCLK bit
        mov r3, GSCLK_PORT|DATA_CLEAR_REG
        sbbo r1,r3,0,4
        //wait for low pulse time
        //GSCLK_DELAY0
        sub r0, r0, 1
        qbne gsclk_loop,r0,0

//blank pulse
        //set BLANK bit
        mov r3, BLANK_PORT|DATA_SET_REG
        sbbo r2,r3,0,4
        //wait for high pulse time
        BLANK_DELAY

        //clear BLANK bit
        mov r3, BLANK_PORT|DATA_CLEAR_REG
        sbbo r2,r3,0,4

        jmp main_loop

