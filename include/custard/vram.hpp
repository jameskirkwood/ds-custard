#ifndef CUSTARD_VRAM_HPP
#define CUSTARD_VRAM_HPP

#include <nds/arm9/video.h>

// TODO: texture and texture palette memory, LCDC and ARM7 WRAM mapping

namespace custard {
  namespace vram {

    // INTERNAL
    //-------------------------------------------------------------------------

    struct _vram_helper {
    protected:

      static void A(int type) { VRAM_A_CR = VRAM_ENABLE | type; }
      static void B(int type) { VRAM_B_CR = VRAM_ENABLE | type; }
      static void C(int type) { VRAM_C_CR = VRAM_ENABLE | type; }
      static void D(int type) { VRAM_D_CR = VRAM_ENABLE | type; }
      static void E(int type) { VRAM_E_CR = VRAM_ENABLE | type; }
      static void F(int type) { VRAM_F_CR = VRAM_ENABLE | type; }
      static void G(int type) { VRAM_G_CR = VRAM_ENABLE | type; }
      static void H(int type) { VRAM_H_CR = VRAM_ENABLE | type; }
      static void I(int type) { VRAM_I_CR = VRAM_ENABLE | type; }
    };

    // Disable all VRAM Banks
    //-------------------------------------------------------------------------

    inline void none() {
      VRAM_CR = VRAM_E_CR = VRAM_F_CR = VRAM_G_CR = VRAM_H_CR = VRAM_I_CR = 0;
    }

    //-------------------------------------------------------------------------
    // Map VRAM Banks to MAIN ENGINE regions
    //-------------------------------------------------------------------------

    struct main : public _vram_helper {

      // Choosing a VRAM mapping for the main engine is quite complicated, as
      // there is a great degree of flexibility.

      // Common mapping options
      //-----------------------------------------------------------------------

      private:
      enum _MST { _BG = 1, _SPRITE = 2 };

      template<_MST MST>
      struct _common {

        // Mapping options common to both the main and sub engines

        static void E_64() { E(MST); }
        static void F_16(int slot = 0) { F(MST | VRAM_OFFSET(slot)); }
        static void G_16(int slot = 0) { G(MST | VRAM_OFFSET(slot)); }

        static void FG_32() { F_16(); G_16(1); }
        static void GF_32() { G_16(); F_16(1); }
        static void EF_80() { E_64(); F_16(2); }
        static void EG_80() { E_64(); G_16(2); }
        static void EFG_96() { EF_80(); G_16(3); }
        static void EGF_96() { EG_80(); F_16(3); }

        static void A_128(int slot = 0) { A(MST | VRAM_OFFSET(slot)); }
        static void B_128(int slot = 0) { B(MST | VRAM_OFFSET(slot)); }

        static void AB_256() { A_128(); B_128(1); }
      };
      public:

      // Map VRAM Banks to main engine BACKGROUND GRAPHICS
      //-----------------------------------------------------------------------

      struct bg : public _common<_BG> {

        // (see 'Common mapping options')

        // Banks C and D may be mapped, in addition to those in
        // 'Common mapping options'

        static void C_128(int slot = 0) { C(_BG | VRAM_OFFSET(slot)); }
        static void D_128(int slot = 0) { D(_BG | VRAM_OFFSET(slot)); }

        static void AC_256() { A_128(); C_128(1); }
        static void AD_256() { A_128(); D_128(1); }
        static void BC_256() { B_128(); C_128(1); }
        static void BD_256() { B_128(); D_128(1); }
        static void CD_256() { C_128(); D_128(1); }

        static void ABC_384() { AB_256(); C_128(2); }
        static void ABD_384() { AB_256(); D_128(2); }
        static void ACD_384() { AC_256(); D_128(2); }
        static void BCD_384() { BC_256(); D_128(2); }

        static void ABCD_512() { ABC_384(); D_128(3); }

        // Map VRAM Banks to main engine background EXTENDED PALETTES
        //---------------------------------------------------------------------

        struct ext_palette : public _vram_helper {

          static void E() { _vram_helper::E(VRAM_E_BG_EXT_PALETTE); }

          static void F_01() { F(VRAM_F_BG_EXT_PALETTE_SLOT01); }
          static void F_23() { F(VRAM_F_BG_EXT_PALETTE_SLOT23); }

          static void G_01() { G(VRAM_G_BG_EXT_PALETTE_SLOT01); }
          static void G_23() { G(VRAM_G_BG_EXT_PALETTE_SLOT23); }

          static void FG() { F_01(); G_23(); }
          static void GF() { G_01(); F_23(); }
        };
      };

      // Map VRAM Banks to main engine SPRITE GRAPHICS
      //-----------------------------------------------------------------------

      struct sprite : public _common<_SPRITE> {

        // (see 'Common mapping options')

        // Map VRAM Banks to main engine sprite EXTENDED PALETTES
        //---------------------------------------------------------------------

        struct ext_palette : public _vram_helper {

          static void F() { _vram_helper::F(VRAM_F_SPRITE_EXT_PALETTE); }
          static void G() { _vram_helper::G(VRAM_G_SPRITE_EXT_PALETTE); }
        };
      };
    };

    //-------------------------------------------------------------------------
    // Map VRAM Banks to SUB ENGINE regions
    //-------------------------------------------------------------------------

    struct sub : public _vram_helper {

      // Choosing a VRAM mapping for the sub engine is straightforward:
      // There are only a few possible configurations.

      // Map VRAM Banks to sub engine BACKGROUND GRAPHICS
      //-----------------------------------------------------------------------

      struct bg : public _vram_helper {

        static void H_32() { H(VRAM_H_SUB_BG); }
        static void I_16_0x8000() { I(VRAM_I_SUB_BG_0x06208000); }
        static void HI_48() { H_32(); I_16_0x8000(); }
        static void C_128() { C(VRAM_C_SUB_BG); }

        struct ext_palette : public _vram_helper {

          static void H() { _vram_helper::H(VRAM_H_SUB_BG_EXT_PALETTE); }
        };

        static void CH_128X() { C_128(); ext_palette::H(); }
      };

      // Map VRAM Banks to sub engine SPRITE GRAPHICS
      //-----------------------------------------------------------------------

      struct sprite : public _vram_helper {

        static void I_16() { I(VRAM_I_SUB_SPRITE); }
        static void D_128() { D(VRAM_D_SUB_SPRITE); }

        struct ext_palette : public _vram_helper {

          static void I() { _vram_helper::I(VRAM_I_SUB_SPRITE_EXT_PALETTE); }
        };

        static void DI_128X() { D_128(); ext_palette::I(); }
      };

      // Map VRAM Banks to sub engine regions in preset configurations
      //-----------------------------------------------------------------------

      // Calling one of the functions below will map the banks indicated in the
      // function name to the sub engine background graphics and sprite
      // graphics regions, respectively. The table below lists the functions
      // and the size of each region when mapped, as well as the banks
      // allocated to extended palettes, if any.

      // |     Sub Config.    |   BG   | Sprite | Extended Palettes |
      // |--------------------|--------|--------|-------------------|
      // | HI_BG48            |    48K |        |                   |
      // | H_BG32_I_S16       |    32K |    16K |                   |
      // | HI_BG48_D_S128     |    48K |   128K |                   |
      // | H_BG32_DI_S128X    |    32K |   128K |        I: Sprite  |
      // | CH_BG128X_I_S16    |   128K |    16K | H: BG             |
      // | CH_BG128X_DI_S128X |   128K |   128K | H: BG, I: Sprite  |

      static void HI_BG48()            { bg::HI_48(); }
      static void H_BG32_I_S16()       { bg::H_32();    sprite::I_16(); }
      static void HI_BG48_D_S128()     { bg::HI_48();   sprite::D_128(); }
      static void H_BG32_DI_S128X()    { bg::H_32();    sprite::DI_128X(); }
      static void CH_BG128X_I_S16()    { bg::CH_128X(); sprite::I_16(); }
      static void CH_BG128X_DI_S128X() { bg::CH_128X(); sprite::DI_128X(); }
    };

    //-------------------------------------------------------------------------
    // Map VRAM Banks to "LCDC mode" VRAM blocks
    //-------------------------------------------------------------------------

    struct lcdc : public _vram_helper {

      static void A() { _vram_helper::A(0); }
      static void B() { _vram_helper::B(0); }
      static void C() { _vram_helper::C(0); }
      static void D() { _vram_helper::D(0); }
    };

  }
}

#endif
