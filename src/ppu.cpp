#include "ppu.hpp"
#include "renderer.hpp"
#include "exception.hpp"
#include "processor.hpp"
#include "exception.hpp"

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

#define CHANGE_BIT(b, n, x) b ^= (-x ^ b) & (1UL << n)
#define CLEAR_BIT(b, n) b &= ~(1UL << n)
#define SET_BIT(b, n) b |= (1UL << n)

/* GBC colour correction factors */
#define GBC_CC_R  0.87
#define GBC_CC_G  0.66
#define GBC_CC_B  0.79
#define GBC_CC_RG 0.115
#define GBC_CC_RB 0.14
#define GBC_CC_GR 0.18
#define GBC_CC_GB 0.07
#define GBC_CC_BR -0.05
#define GBC_CC_BG 0.225

using namespace qtboy;

Ppu::Ppu(Memory &m,
         Processor &p,
         Renderer *r)
    : memory_ {m},
      cpu_ {p},
      renderer_ {r}
{}

void Ppu::reset()
{
    clock_ = 0;
    window_line_ = 0;
    lcdc_ = 0x90;
    stat_ = 0x00;
    scy_ = 0;
    scx_ = 0;
    ly_ = 0;
    lyc_ = 0;
    bgp_ = 0xfc;
    obp0_ = 0xff;
    obp1_ = 0xff;
    wy_ = 0;
    wx_ = 0;
    sprites_ = {};
    stat_signal_ = false;
    // CGB registers
    cgb_mode_ = false;
    bgpd_ = {};
    obpd_ = {};
    bgpi_ = 0;
    obpi_ = 0;
}

void Ppu::enable_cgb(bool is_cgb)
{
    cgb_mode_ = is_cgb;
}

void Ppu::step(size_t cycles)
{
    // PPU operates on 4.194 MHz clock, 1 clock = 1/4 cycle
    clock_ += cycles;
    if (!(lcdc_ & 0x80)) // bit 7
    {
        clock_ = 0;
        ly_ = 0;
        stat_ &= 0xfc; // mode 0
        return; // don't execute if master bit is off
    }
    check_stat();
    switch (stat_ & 3) // bit 0-1
    {
        // mode 2: scan for OAM sprites
        case 2:
            oam_scan();
            break;
        // OAM/VRAM read
        // end of mode 3 = end of scan line
        case 3:
            vram_read();
            break;
        // HBLANK
        case 0:
            hblank();
            break;
        // VBLANK
        case 1:
            vblank();
            break;
    }
}

int Ppu::mode() const
{
    return stat_ & 3;
}

int Ppu::clock() const
{
    return clock_;
}

bool Ppu::enabled() const
{
    return lcdc_ & 0x80;
}

uint8_t Ppu::read_reg(uint16_t adr)
{
    uint8_t b = 0xff;
    switch (adr)
    {
        case 0xff40: b = lcdc_; break;
        case 0xff41: b = stat_; break;
        case 0xff42: b = scy_; break;
        case 0xff43: b = scx_; break;
        case 0xff44: b = ly_; break;
        case 0xff45: b = lyc_; break;
        // ff46: DMA transfer
        case 0xff47: b = bgp_; break;
        case 0xff48: b = obp0_; break;
        case 0xff49: b = obp1_; break;
        case 0xff4a: b = wy_; break;
        case 0xff4b: b = wx_; break;

        // CGB registers
        case 0xff68: b = bgpi_; break;
        case 0xff69: b = bgpd_[bgpi_ & 0x3f]; break;
        case 0xff6a: b = obpi_; break;
        case 0xff6b: b = obpd_[obpi_ & 0x3f]; break;

        default:
            throw qtboy::Exception("Attempted to read invalid PPU register.",
                                     __FILE__, __LINE__);
    }
    return b;
}

void Ppu::write_reg(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff40: lcdc_ = b; break;
        case 0xff41:
        {
            // lower 3 bits of STAT are read only,
            // only write to upper 4 bits
            // clear upper 4 bits
            stat_ &= ~0xf8;
            // set upper 4 bits
            stat_ |= b & 0xf8;
        } break;
        case 0xff42: scy_ = b; break;
        case 0xff43: scx_ = b; break;
        case 0xff44: ly_ = b; break;
        case 0xff45: lyc_ = b; break;
        // ff46: DMA transfer
        case 0xff47: bgp_ = b; break;
        case 0xff48: obp0_ = b; break;
        case 0xff49: obp1_ = b; break;
        case 0xff4a: wy_ = b; break;
        case 0xff4b: wx_ = b; break;

        // CGB registers
        case 0xff68: bgpi_ = b; break; // background palette index
        case 0xff69:
        {
            // bits 0-5 in bgpi used to index in background palette memory
            // bit 7 enables auto increment after writting
            bgpd_[bgpi_ & 0x3f] = b;
            if (bgpi_ & 0x80)
                ++bgpi_;
        } break;
        case 0xff6a: obpi_ = b; break;
        case 0xff6b:
        {
            // bits 0-5 in obpi used to index in object palette memory
            // bit 7 enables auto increment after writting
            obpd_[obpi_ & 0x3f] = b;
            if (obpi_ & 0x80)
                ++obpi_;
        } break;

        default:
            throw qtboy::Exception("Attempted to write invalid PPU register.",
                                     __FILE__, __LINE__);
    }
}


void Ppu::set_renderer(Renderer *r)
{
    renderer_ = r;
}

Texture Ppu::get_framebuffer(bool with_bg, bool with_win,
                             bool with_sprites) const
{
    Texture frame = with_bg ? get_layer(Layer::Background) : Texture(256, 256);
    if (with_win)
    {
        Texture window(get_layer(Layer::Window));
        // copy window layer onto frame buffer layer in the correct position
        for (uint16_t y = 0; y < 256; ++y)
        {
            for (uint16_t x = 0; x < 256; ++x)
            {
                uint16_t wx = wx_-7 - scx_ + x;
                uint16_t wy = wy_ - scy_ + y;
                if (wx > 0xff || wy > 0xff
                    || wx >= scx_+160 || wy >= scy_+144)
                    continue;
                unsigned wi = wx+wy*256;
                frame.set_pixel(wi, window.pixel(x+y*256));
            }
        }
    }
    return frame;
}

Texture Ppu::get_tile(uint8_t bank, uint16_t i) const
{
    // DMG only has 1 VRAM bank, only allow reading multiple banks if CGB
    if (bank > 0 && !cgb_mode_)
        return {};
    Texture tex(8,8);
    uint16_t tile_base = 0x8000;
    for (uint8_t byte = 0; byte < 16; byte += 2)
    {
        uint8_t lo_byte = read_vram(bank, byte+tile_base+i*16);
        uint8_t hi_byte = read_vram(bank, byte+tile_base+i*16+1);
        for (uint8_t px = 0; px < 8; ++px)
        {
            bool hi_bit = hi_byte & 1 << (7-px);
            bool lo_bit = lo_byte & 1 << (7-px);
            uint8_t color_i = static_cast<uint8_t>(hi_bit << 1 | lo_bit);
            Color c = dmg_palette[color_i];
            uint8_t p = (byte/2)*8+px;
            tex.set_pixel(p, c);
            tex.set_pixel_priority(p, 0); // irrelevant
        }
    }
    return tex;
}

Texture Ppu::get_layer(Layer l) const
{
    Texture tex(256, 256);
    for (unsigned y = 0; y < 256; ++y)
    {
        for (unsigned x = 0; x < 256; ++x)
        {
            render_layer_pixel(tex, l, x, y, x, y);
        }
    }
    return tex;
}

std::array<uint8_t, 32*32> Ppu::get_raw_background()
{
    uint16_t bg_map = ((lcdc_ & (1 << 3)) ? 0x9c00 : 0x9800);
    std::array<uint8_t, 32*32> raw {};
    for (int i = 0; i < 32*32; ++i)
        raw[i] = read(bg_map+i);
    return raw;
}

Ppu::Dump Ppu::dump_values() const
{
    return Dump
    {
        lcdc_, stat_,
        scy_, scx_,
        ly_, lyc_,
        bgp_, obp0_, obp1_,
        wy_, wx_,
        bgpi_, bgpd_, obpi_, obpd_
    };
}

uint8_t Ppu::read(uint16_t adr) const
{
    return memory_.read(adr);
}

void Ppu::write(uint8_t b, uint16_t adr)
{
    memory_.write(b, adr);
}

uint8_t Ppu::read_vram(uint8_t bank, uint16_t adr) const
{
    return memory_.vram_read(bank, adr);
}

void Ppu::write_vram(uint8_t b, uint8_t bank, uint16_t adr)
{
    memory_.vram_write(b, bank, adr);
}

void Ppu::render_scanline()
{
    Texture tex {160, 1};
    // STOP mode: if LCD is on, set to all white, if off, all black
    if (false && cpu_.stopped())
    {
        Color c = (lcdc_ & 0x80) ? 0xffff : 0;
        tex.fill(c);
    }
    else
    {
        if (lcdc_ & 1 || cgb_mode_) // bg/window enable
        {
            render_layer_line(tex, Ppu::Layer::Background);
            if (lcdc_ & 1 << 5) // window display enable
                render_layer_line(tex, Ppu::Layer::Window);
        }
        // background and window appear white if lcdc bit 0 is cleared
        else
        {
            Color c(0xffff);
            tex.fill(c);
        }
        if (lcdc_ & 1 << 1) // OBJ display enable
            render_sprite_line(tex);
    }
    renderer_->draw_texture(tex, 0, ly_);
}

void Ppu::render_layer_line(Texture &tex, Ppu::Layer layer)
{
    // don't draw windo if the current line isn't a window line
    if (layer == Ppu::Layer::Window && ly_ < wy_)
        return;
    // current y-coordinate of the 256x256 layer
    uint8_t y = 0;
    if (layer == Ppu::Layer::Background)
        y = ly_ + scy_;
    else if (layer == Ppu::Layer::Window)
        y = window_line_;
    // draw each pixel in the scanline
    uint8_t window_pxs_drawn = 0;
    for (uint8_t x_px = 0; x_px < 160; ++x_px)
    {
        // don't draw window if it doesn't appear yet
        if (layer == Ppu::Layer::Window)
        {
            // x: window could appear later in the line
            if (x_px < wx_-7)
                continue;
            // y: window does not appear at all in this line
            else if (wx_ < 7 || wx_ > 166 || wy_ > 143)
                return;
        }
        // current x-coordinate of the 256x256 layer
        uint8_t x = (layer == Ppu::Layer::Background)
                ? x_px + scx_
                : x_px - (wx_-7);
        render_layer_pixel(tex, layer, x, y, x_px, 0);
        ++window_pxs_drawn;
    }
    if (layer == Ppu::Layer::Window && window_pxs_drawn != 0)
        ++window_line_;
}

// render the layer pixel at (tex_x,tex_y) with the color at (x, y) in VRAM
void Ppu::render_layer_pixel(Texture &tex, Ppu::Layer layer,
                             uint8_t x, uint8_t y,
                             uint8_t tex_x, uint8_t tex_y) const
{
    uint16_t tile_map = 0x9800;
    if (layer == Ppu::Layer::Background)
    {
        tile_map = ((lcdc_ & (1 << 3)) ? 0x9c00 : 0x9800);

    }
    else if (layer == Ppu::Layer::Window)
    {
        tile_map = ((lcdc_ & (1 << 6)) ? 0x9c00 : 0x9800);
    }
    // tile data offset in VRAM
    uint16_t tile_base = (lcdc_ & 1 << 4) ? 0x8000 : 0x9000;
    // x-index into the tile BG map (tile_map) of the tile to draw
    uint8_t tile_x = x >> 3; // tiles are 8 px wide
    // y tile index into the tile map
    uint8_t tile_y = y >> 3; // tiles are 8 px tall
    // index into tile BG map of the tile to draw
    uint16_t tile_i = (tile_y*32) + tile_x + tile_map;
    // CGB only: corresponding tile attributes held in parallel location
    // in VRAM bank 1
    uint8_t tile_attr = cgb_mode_ ? read_vram(1, tile_i) : 0;
    // index of the tile in tile data of the tile to draw
    uint8_t tile_data_i = read_vram(0, tile_i);
    // bank containing the tile data
    uint8_t bank = (tile_attr & 1 << 3) ? 1 : 0;
    // location to read tile data from
    uint16_t adr = 0;
    // 0x9000 base uses signed addressing
    if (tile_base == 0x9000)
    {
       // starting address of the data for the tile (1 tile is 16 bytes)
       adr = tile_base+static_cast<int8_t>(tile_data_i)*16;
    }
    // 0x8000 uses unsigned addressing
    else
    {
       // starting address of the data for the tile (1 tile is 16 bytes)
       adr = tile_base+tile_data_i*16;
    }
    // point address to the right byte in the tile data based on y pos
    // take into account vertical flip if specified in tile attribute (CGB)
    if (tile_attr & 1 << 6) // vertical flip
        adr += (7 - (y % 8)) * 2;
    else
        adr += (y % 8) * 2;
    // get two bytes (one row of pixels) from the tile data
    uint8_t lo_byte = read_vram(bank, adr);
    uint8_t hi_byte = read_vram(bank, adr+1);
    // get the pixel offset of the tile (0-7) (tiles are 8x8)
    // take into account horizontal flip
    uint8_t px_offset = (x % 8);
    if (tile_attr & 1 << 5)
        px_offset = 7 - (x % 8);
    // get 2 corresponding bits, 1 from each byte
    // most significant bits of both bytes represent color of first
    // (or last) pixel
    bool hi_bit = (hi_byte & 1 << (7-px_offset));
    bool lo_bit = (lo_byte & 1 << (7-px_offset));
    Palette pal(get_bg_palette(tile_attr & 7));
    // get pixel color index
    // bit of hi byte is the hi bit of the 2-bit color index in the palette
    // bit of lo byte is the lo bit of the 2-bit color index in the palette
    uint8_t px_i = static_cast<uint8_t>(hi_bit << 1 | lo_bit);
    // 0 = highest priority (appears above everything else)
    // px_index == 0 has lowest priority
    uint8_t priority = (px_i == 0) ? 2 : 1;
    if (tile_attr & 1 << 7) // ensure highest priority
        priority = 0;
    // CGB: when LCDC bit 0 is cleared, sprites always appear above bg/window
    if (cgb_mode_ && !(lcdc_ & 1))
        priority = 3;
    // index into texture
    unsigned tex_i = tex_x + tex_y * tex.width();
    tex.set_pixel(tex_i, pal[px_i]);
    tex.set_pixel_priority(tex_i, priority);
}

void Ppu::render_sprite_line(Texture &tex)
{
    const uint16_t tile_data = 0x8000;
    // 0=8x8, 1=8x16
    const uint8_t sprite_h = lcdc_ & 1 << 2 ? 16 : 8;
    uint8_t sprites_drawn = 0;
    // sprite priority list: get first 10 sprites, then reorder so the lowest
    // priority is drawn first
    std::array<Sprite, 10> ordered_sprites {};
    for (uint8_t i = 0; i < 40; ++i)
    {
        Sprite &s = sprites_[i];
        // skip if sprite is offscreen
        if (s.y == 0 || s.y >= 160)
            continue;
        // stop drawing sprites if more than 10 are already on this scanline
        if (sprites_drawn == 10)
            break;
        // line number relative to start of sprite
        uint8_t ln = ly_ - (s.y-16);
        // ignore sprite if no part of it falls on this line
        if (ln > sprite_h-1 || ly_ < s.y-16)
            continue;
        ordered_sprites[sprites_drawn] = s;
        ++sprites_drawn;
    }
    // first object in OAM has highest priority, so draw first
    order_sprites(ordered_sprites);
    for (Sprite s : ordered_sprites)
    {
        uint8_t tile_i;
        // line number relative to start of sprite
        uint8_t ln = ly_ - (s.y-16);
        bool y_flip = s.attr & 1 << 6;
        if (ln > 7) // lower half of an 8x16 sprite
        {
            tile_i = (y_flip)
                    ? s.tile & 0xfe
                    : s.tile | 0x01;
        }
        else
        {
            if (sprite_h == 16) // upper half of an 8x16 sprite
            {
                tile_i = (y_flip)
                        ? s.tile | 0x01
                        : s.tile & 0xfe;
            }
            else // 8x8 sprite
            {
                tile_i = s.tile;
            }
        }
        uint16_t adr = tile_data+tile_i*16;
        if (y_flip)
            adr += (7 - (ln % 8)) * 2;
        else
            adr += (ln % 8) * 2;
        // CGB Only: attribute bit 3 specifies VRAM bank, otherwise 0
        uint8_t bank = (cgb_mode_ && s.attr & 1 << 3) ? 1 : 0;
        uint8_t low_byte = read_vram(bank, adr);
        uint8_t high_byte = read_vram(bank, adr+1);
        // if CGB: palette is in attribute bits 0-2, otherwise bit 4
        uint8_t pal_idx = (cgb_mode_) ? (s.attr & 7) : (s.attr & 1 << 4);
        Palette pal = get_sprite_palette(pal_idx);
        bool ob_priority = s.attr & 1 << 7;
        for (uint8_t px = 0; px < 8; ++px)
        {
            uint8_t x = s.x-8 + px;
            bool on_screen = (x < 160 && x > 0);
            uint8_t bg_priority = tex.pixel_priority(x);
            // only draw pixel if on screen and if priority is 0 and bg pixel priority
            // IS NOT 0, or if priority is 1
            // and bg pixel is >=2 (transparent)
            if (on_screen
                    && ( (ob_priority == 0 && bg_priority > 0)
                        || (ob_priority < bg_priority)))
            {
                bool x_flip = s.attr & 1 << 5;
                bool hi_bit = (high_byte & 1 << (x_flip ? px : 7-px));
                bool lo_bit = (low_byte & 1 << (x_flip ? px : 7-px));
                uint8_t p = static_cast<uint8_t>(hi_bit << 1 | lo_bit);
                if (p != 0) // only draw if not transparent
                    tex.set_pixel(x, pal[p]);
            }
        }
    }
}

std::array<Texture, 40> Ppu::get_sprites() const
{
    std::array<Texture, 40> out {};
    int i = 0;
    for (const Sprite &s : sprites_)
    {
        uint8_t bank = s.attr & 1 << 3 ? 1 : 0;
        out[i++] = get_tile(bank, s.tile);
    }
    return out;
}

void Ppu::order_sprites(std::array<Sprite, 10> &s) const
{
    // CGB mode: lower OAM # = higher priority
    if (cgb_mode_)
    {
        std::reverse(s.begin(), s.end());
    }
    // DMG mode: lower X priority = higher priority, tie breaker: lower OAM
    else
    {
        std::sort(s.begin(), s.end(), [](const Sprite &a, const Sprite &b)
        {
            return (a.x == b.x) ? (a.id > b.id) : (a.x > b.x);
        });
    }
}

Palette Ppu::get_bg_palette(uint8_t idx) const
{
    Palette pal {};
    // CGB stores palettes in background palette memoery (bgpm)
    if (cgb_mode_)
    {
        uint8_t pal_idx = idx * 8;
        // each palette is 8 bytes long (2 per color)
        for (uint8_t i = 0; i < 4; ++i)
        {
            // RGB = 2 bytes (lo byte first)
            uint16_t rgb = static_cast<uint16_t>(
                        bgpd_[pal_idx+i*2] | bgpd_[pal_idx+i*2+1] << 8);
            // xbbb bbgg gggr rrrr
            pal[i] = color_correct(rgb);
        }
    }
    // DMG only has one grayscale palette
    else
    {
        for (uint8_t i {0}; i < 4; ++i)
            pal[i] = dmg_palette[((bgp_ >> i*2) & 3)];
    }
    return pal;
}

Palette Ppu::get_sprite_palette(uint8_t idx) const
{
    Palette pal {};
    if (cgb_mode_)
    {
        uint8_t pal_idx = idx * 8;
        // each palette is 8 bytes long (2 per color)
        for (uint8_t i = 0; i < 4; ++i)
        {
            // RGB = 2 bytes (lo byte first)
            uint16_t rgb = static_cast<uint16_t>(
                        obpd_[pal_idx+i*2] | obpd_[pal_idx+i*2+1] << 8);
            // xbbb bbgg gggr rrrr
            pal[i] = color_correct(rgb);
        }
    }
    else
    {
        uint8_t obp = idx ? obp1_ : obp0_;
        for (uint8_t i {1}; i < 4; ++i)
            pal[i] = dmg_palette[((obp >> i*2) & 3)];
        pal[0] = dmg_palette[0]; // 00 in sprite palette is always transparent
    }
    return pal;
}

Color Ppu::color_correct(const Color &c) const
{
    const unsigned r = c & 0x1f,
            g = c >> 5 & 0x1f,
            b = c >> 10 & 0x1f;
    unsigned rFinal = 0, gFinal = 0, bFinal = 0;

    static const float rgbMax = 31.0;
    static const float rgbMaxInv = 1.0 / rgbMax;
    const float colorCorrectionBrightness = brightness;

    if (color_correction == Color_correction::Fast)
    {
        // Gambatte method. Fast, but inaccurate
        rFinal = ((r * 13) + (g * 2) + b) >> 4;
        gFinal = ((g * 3) + b) >> 2;
        bFinal = ((r * 3) + (g * 2) + (b * 11)) >> 4;

    }
    else if (color_correction == Color_correction::Proper)
    {
        // Use Pokefan531's "gold standard" GBC colour correction
        // (https://forums.libretro.com/t/real-gba-and-ds-phat-colors/1540/174)
        // NB: The results produced by this implementation are ever so slightly
        // different from the output of the gbc-colour shader. This is due to the
        // fact that we have to tolerate rounding errors here that are simply not
        // an issue when tweaking the final image with a post-processing shader.
        // *However:* the difference is so tiny small that 99.9% of users will
        // never notice, and the result is still 100x better than the 'fast'
        // colour correction method.
        //
        // Constants
        static const float targetGamma = 2.2;
        static const float displayGammaInv = 1.0 / targetGamma;
        // Perform gamma expansion
        float adjustedGamma = targetGamma - colorCorrectionBrightness;
        float rFloat = std::pow(static_cast<float>(r) * rgbMaxInv, adjustedGamma);
        float gFloat = std::pow(static_cast<float>(g) * rgbMaxInv, adjustedGamma);
        float bFloat = std::pow(static_cast<float>(b) * rgbMaxInv, adjustedGamma);
        // Perform colour mangling
        float rCorrect = (GBC_CC_R  * rFloat) + (GBC_CC_GR * gFloat) + (GBC_CC_BR * bFloat);
        float gCorrect = (GBC_CC_RG * rFloat) + (GBC_CC_G  * gFloat) + (GBC_CC_BG * bFloat);
        float bCorrect = (GBC_CC_RB * rFloat) + (GBC_CC_GB * gFloat) + (GBC_CC_B  * bFloat);
        // Range check...
        rCorrect = rCorrect > 0.0f ? rCorrect : 0.0f;
        gCorrect = gCorrect > 0.0f ? gCorrect : 0.0f;
        bCorrect = bCorrect > 0.0f ? bCorrect : 0.0f;
        // Perform gamma compression
        rCorrect = std::pow(rCorrect, displayGammaInv);
        gCorrect = std::pow(gCorrect, displayGammaInv);
        bCorrect = std::pow(bCorrect, displayGammaInv);
        // Range check...
        rCorrect = rCorrect > 1.0f ? 1.0f : rCorrect;
        gCorrect = gCorrect > 1.0f ? 1.0f : gCorrect;
        bCorrect = bCorrect > 1.0f ? 1.0f : bCorrect;
        // Perform image darkening, if required
        // Convert back to 8bit unsigned
        rFinal = static_cast<unsigned>((rCorrect * rgbMax) + 0.5) & 0x1F;
        gFinal = static_cast<unsigned>((gCorrect * rgbMax) + 0.5) & 0x1F;
        bFinal = static_cast<unsigned>((bCorrect * rgbMax) + 0.5) & 0x1F;
    }
    else
    {
        rFinal = r;
        gFinal = g;
        bFinal = b;
    }

    return rFinal << 10 | gFinal << 5 | bFinal;
}

void Ppu::load_sprites()
{
    uint8_t i = 0;
    for (uint16_t adr = 0xfe00; adr < 0xfea0; adr += 4) // sprites are 4 bytes
    {
        sprites_[i].y = read(adr);
        sprites_[i].x = read(adr+1);
        sprites_[i].tile = read(adr+2);
        sprites_[i].attr = read(adr+3);
        sprites_[i].id = i;
        ++i;
    }
}

// OAM_SCAN mode 2
void Ppu::oam_scan()
{
    if (clock_ >= 80)
    {
        clock_ -= 80;
        load_sprites();
        SET_BIT(stat_, 1);
        SET_BIT(stat_, 0); // mode 3
    }
}

// VRAM_READ mode 3
void Ppu::vram_read()
{
    if (clock_ >= 172)
    {
        clock_ -= 172;
        if (!renderer_)
            return;
        render_scanline();
        // enter hblank
        CLEAR_BIT(stat_, 1);
        CLEAR_BIT(stat_, 0); // mode 0
        if (cgb_mode_)
            memory_.hblank_dma();
    }
}

// HBLANK mode 0
void Ppu::hblank()
{
    if (clock_ >= 204)
    {
        clock_ -= 204;
        ++ly_;
        if (ly_ == 144)
        {
            // enter vblank
            window_line_ = 0;
            CLEAR_BIT(stat_, 1); // mode 1
            SET_BIT(stat_, 0);
            cpu_.request_interrupt(Processor::Interrupt::VBLANK);
            if (!renderer_)
                return;
            renderer_->present_screen();
        }
        else
        {
            // enter oam_scan
            SET_BIT(stat_, 1); // mode 2
            CLEAR_BIT(stat_, 0);
        }
    }
}

// VBLANK mode 1
void Ppu::vblank()
{
    if (clock_ >= 456)
    {
        clock_ -= 456;
        ++ly_;
        if (ly_ > 153)
        {
            // restart scanning modes
            SET_BIT(stat_, 1); // mode 2
            CLEAR_BIT(stat_, 0);
            ly_ = 0;
        }
    }
}

void Ppu::check_stat()
{
    int mod = mode();
    bool lyc_comp = (ly_ == lyc_ && stat_ & (1 << 6));
    bool vbl_chk = (mod == 0 && stat_ & (1 << 3));
    bool oam_chk = (mod == 2 && stat_ & (1 << 5));
    bool hbl_chk = (mod == 1
                    && ((stat_ & 1 << 4) || (stat_ & 1 << 5)));
    if (ly_ == lyc_)
        SET_BIT(stat_, 2);
    else
        CLEAR_BIT(stat_, 2);
    if (lyc_comp || vbl_chk || oam_chk || hbl_chk)
    {
        // STAT interrupt is only request when signal goes from 0->1
        if (!stat_signal_)
            cpu_.request_interrupt(Processor::Interrupt::LCD_STAT);
        stat_signal_ = true;
    }
    else
    {
        stat_signal_ = false;
    }
}

Palette Ppu::dmg_palette =
{{
    0xffff, 0x56b5, 0x294a, 0
}};
