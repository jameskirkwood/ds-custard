#ifndef CUSTARD_DISPLAY_HPP
#define CUSTARD_DISPLAY_HPP

#include <nds/arm9/video.h>

namespace custard {

/// Display control overlays
//-----------------------------------------------------------------------------
/// This namespace contains functions and classes for configuring the main (A)
/// and sub (B) 2D rendering engines. Structures \ref config and
/// \ref main_config store display control register overlays, allowing to
/// configure the rendering engines in an object-oriented way. Inline functions
/// are provided for immediately setting well known configurations.
///
/// \see GBATEK's documentation for NDS %display control:
///   http://problemkaputt.de/gbatek.htm#dsvideobgmodescontrol
//-----------------------------------------------------------------------------
  namespace display {
//-----------------------------------------------------------------------------

// DISPCNT Bit Field Overlay
//-----------------------------------------------------------------------------
/// \brief This structure is a bit field overlay for the display control
/// registers. \details
///
/// You should not use _dispcnt_overlay directly. Use the derived classes
/// custard::display::config and custard::display::main_config instead, as
/// these provide convenient methods for configuring overlays and updating the
/// display control registers. However, you should still expect to access some
/// of the fields inherited from _dispcnt_overlay directly.
///
/// The NDS has two display control (DISPCNT) registers, one for each 2D
/// rendering engine. The libnds macros for these registers are REG_DISPCNT and
/// REG_DISPCNT_SUB. Each register is 32 bits and controls the function of its
/// corresponding rendering engine. The registers contain the same fields, but
/// some fields are only used by the main (A) engine.
///
/// The _dispcnt_overlay structure contains a member for each of the fields
/// present in the DISPCNT registers. The member \b mask encapsulates all of
/// the fields in a 32-bit unsigned integer that may be directly read from, or
/// written to, either of the DISPCNT registers.
///
/// \see GBATEK's documentation for the DISPCNT registers:
///   http://problemkaputt.de/gbatek.htm#dsvideobgmodescontrol
//-----------------------------------------------------------------------------
    struct _dispcnt_overlay {
//-----------------------------------------------------------------------------
      union {
        struct { uint32_t

          /// \brief BG Mode: Assigns graphics modes to all backgrounds
          /// \details
          /// Value | BG0 & BG1 | BG2          | BG3      |
          /// ----- | --------- | ------------ | -------- |
          /// 0     | Text      | Text         | Text     |
          /// 1     | Text      | Text         | Affine   |
          /// 2     | Text      | Affine       | Affine   |
          /// 3     | Text      | Text         | Extended |
          /// 4     | Text      | Affine       | Extended |
          /// 5     | Text      | Extended     | Extended |
          /// 6 *   | -         | Large Bitmap | -        |
          /// (*) Only a 512K 8bpp rotoscale bitmap and 3D engine output may be
          /// displayed (MAIN ENGINE ONLY)
          /// \todo Verify that mode 6 indeed uses only BG2
          bg_mode: 3,

          bg0_3d: 1, ///< Enable 3D engine output on BG0 (MAIN ENGINE ONLY)

          tile_obj_mapping: 1, ///< Tile Sprite Mapping: 0=2D, 1=1D

          /// Bitmap Sprite 2D Source Width: 0=128px, 1=256px
          bmp_obj_2d_dimension: 1,

          bmp_obj_mapping: 1, ///< Bitmap Sprite Mapping: 0=2D, 1=1D

          forced_blank: 1, ///< Force blanking to suppress memory contention

          bg0_enable: 1, ///< Enable display of BG0 (background 0)
          bg1_enable: 1, ///< Enable display of BG1 (background 1)
          bg2_enable: 1, ///< Enable display of BG2 (background 2)
          bg3_enable: 1, ///< Enable display of BG3 (background 3)
          obj_enable: 1, ///< Enable display of sprites

          window_0_enable:   1, ///< Enable clipping window 0
          window_1_enable:   1, ///< Enable clipping window 1
          obj_window_enable: 1, ///< Enable sprite clipping window

          /// \brief Display enable and mode selection \details
          /// Value | Function                                               |
          /// ----- | ------------------------------------------------------ |
          /// 0     | None (screen is white)                                 |
          /// 1     | Normal graphics display (backgrounds, sprites, 3D)     |
          /// 2     | Read frame buffer from VRAM (MAIN ENGINE ONLY)         |
          /// 3     | Transfer frame buffer from main RAM (MAIN ENGINE ONLY) |
          /// \see vram_block
          display_mode: 2,

          /// \brief VRAM bank in [A..D] to use for display capture or frame
          /// buffer display mode (MAIN ENGINE ONLY) \details
          /// Values: 0=A, 1=B, 2=C, 3=D\n
          /// The selected bank must be mapped to its LCDC address.
          vram_block: 2, 

          tile_obj_1d_boundary: 2, ///< Tile Sprite 1D Stride: {32,64,128,256}B
          bmp_obj_1d_boundary:  1, ///< Bitmap Sprite 1D Stride: 0=128B, 1=256B

          /// Enable access to OAM during horizontal blanking
          hblank_oam_access: 1,

          /// \brief Additional global tile base offset as multiple of 64K
          /// (MAIN ENGINE ONLY)
          char_base: 3,

          /// \brief Additional global map base offset as multiple of 64K
          /// (MAIN ENGINE ONLY)
          screen_base: 3,

          bg_ext_palettes:  1, ///< Enable background extended palettes
          obj_ext_palettes: 1; ///< Enable sprite extended palettes
        };
        uint32_t mask;
      };
    };

// Rendering Engine Configuration Overlay
//-----------------------------------------------------------------------------
/// \brief This structure is a user-friendly overlay for the display control
/// registers. \details
///
/// You may configure the overlay by setting the bit fields declared in the
/// base class, custard::display::_dispcnt_overlay. In addition, this structure
/// provides a collection of methods for setting configurations compatible with
/// both the main (A) and sub (B) 2D rendering engines.
///
/// Methods \ref set_main and \ref set_sub write the overlay to the main (A)
/// engine and sub (B) engine display control registers, respectively.
///
/// Methods of the form \b graphics_XXXX, where each X is one of T, A or E,
/// configure the overlay for 2D graphics display with background modes as
/// follows:
///
/// XXXX   | BG0   | BG1   | BG2      | BG3      |
/// ------ | ----- | ----- | -------- | -------- |
/// `TTTT` | Text  | Text  | Text     | Text     |
/// `TTTA` | Text  | Text  | Text     | Affine   |
/// `TTAA` | Text  | Text  | Affine   | Affine   |
/// `TTTE` | Text  | Text  | Extended | Extended |
/// `TTAE` | Text  | Text  | Affine   | Extended |
/// `TTEE` | Text  | Text  | Extended | Extended |
//-----------------------------------------------------------------------------
    struct config : public _dispcnt_overlay {
//-----------------------------------------------------------------------------

      /// Instate this configuration on the main (A) rendering engine.
      void set_main() { REG_DISPCNT = mask; }
      /// Instate this configuration on the sub (B) rendering engine.
      void set_sub() { REG_DISPCNT_SUB = mask; }

      // 2D graphics display modes
      void graphics_TTTT() { display_mode = 1; bg0_3d = false; bg_mode = 0; }
      void graphics_TTTA() { display_mode = 1; bg0_3d = false; bg_mode = 1; }
      void graphics_TTAA() { display_mode = 1; bg0_3d = false; bg_mode = 2; }
      void graphics_TTTE() { display_mode = 1; bg0_3d = false; bg_mode = 3; }
      void graphics_TTAE() { display_mode = 1; bg0_3d = false; bg_mode = 4; }
      void graphics_TTEE() { display_mode = 1; bg0_3d = false; bg_mode = 5; }
    };

    /// Disable the main (A) rendering engine (display is white)
    inline void set_main_off() { REG_DISPCNT = 0; }
    /// Disable the sub (B) rendering engine (display is white)
    inline void set_sub_off() { REG_DISPCNT_SUB = 0; }

// Main (A) Rendering Engine Exclusive Configuration Overlay
//-----------------------------------------------------------------------------
/// \brief This structure is a user-friendly overlay for the main (A) engine
/// display control register only. \details
///
/// You may configure the overlay by setting the bit fields declared in the
/// base class, custard::display::_dispcnt_overlay, or by calling the
/// configuration methods declared in the superclass custard::display::config.
/// In addition, this structure provides a collection of methods for setting
/// configurations compatible with only the main (A) rendering engine.
///
/// The method \ref set_main writes the overlay to the main (A) engine display
/// control register. \b set_sub is deleted because the configuration may not
/// be compatible with the sub (B) engine. If a configuration will be used on
/// both engines, custard::display::config may be used instead.
///
/// Methods of the form \b graphics_3dXXX, where each X is one of T, A or E,
/// configure the overlay for 3D+2D graphics display with background modes as
/// follows:
///
/// XXX   | BG0 | BG1   | BG2      | BG3      |
/// ----- | --- | ----- | -------- | -------- |
/// `TTT` | 3D  | Text  | Text     | Text     |
/// `TTA` | 3D  | Text  | Text     | Affine   |
/// `TAA` | 3D  | Text  | Affine   | Affine   |
/// `TTE` | 3D  | Text  | Extended | Extended |
/// `TAE` | 3D  | Text  | Affine   | Extended |
/// `TEE` | 3D  | Text  | Extended | Extended |
///
/// Note that background 0 is used as a rendering target for the 3D rendering
/// engine, which must be configured separately. This feature is exclusive to
/// the main (A) engine.
///
/// The method \b graphics_Bmp configures the overlay for "large bitmap"
/// display mode (mode 6, exclusive to the main (A) engine). The method
/// \b graphics_3dBmp is similar, but also enables 3D rendering to BG0.
//-----------------------------------------------------------------------------
    struct main_config : public config {
//-----------------------------------------------------------------------------

      /// Instate this configuration on the main (A) rendering engine.
      void set_main() { REG_DISPCNT = mask; }
      void set_sub() = delete; // cannot set main configuration to sub display

      // Large bitmap graphics display mode
      void graphics_Bmp() { display_mode = 1; bg0_3d = false; bg_mode = 6; }

      // 3D graphics display modes
      void graphics_3dTTT() { display_mode = 1; bg0_3d = true; bg_mode = 0; }
      void graphics_3dTTA() { display_mode = 1; bg0_3d = true; bg_mode = 1; }
      void graphics_3dTAA() { display_mode = 1; bg0_3d = true; bg_mode = 2; }
      void graphics_3dTTE() { display_mode = 1; bg0_3d = true; bg_mode = 3; }
      void graphics_3dTAE() { display_mode = 1; bg0_3d = true; bg_mode = 4; }
      void graphics_3dTEE() { display_mode = 1; bg0_3d = true; bg_mode = 5; }
      void graphics_3dBmp() { display_mode = 1; bg0_3d = true; bg_mode = 6; }
    };

    /// \brief Set the main (A) rendering engine to display an RGB15 frame
    /// buffer at the base of VRAM bank A mapped to its LCDC address
    inline void set_main_vram_A() { REG_DISPCNT = (2 << 16) | (0 << 18); }
    /// \brief Set the main (A) rendering engine to display an RGB15 frame
    /// buffer at the base of VRAM bank B mapped to its LCDC address
    inline void set_main_vram_B() { REG_DISPCNT = (2 << 16) | (1 << 18); }
    /// \brief Set the main (A) rendering engine to display an RGB15 frame
    /// buffer at the base of VRAM bank C mapped to its LCDC address
    inline void set_main_vram_C() { REG_DISPCNT = (2 << 16) | (2 << 18); }
    /// \brief Set the main (A) rendering engine to display an RGB15 frame
    /// buffer at the base of VRAM bank D mapped to its LCDC address
    inline void set_main_vram_D() { REG_DISPCNT = (2 << 16) | (3 << 18); }

    /// \brief Set the main (A) rendering engine to display an RGB15 frame
    /// buffer in main memory by DMA transfer
    inline void set_main_fifo() { REG_DISPCNT = 3 << 16; }
  }
}

#endif
