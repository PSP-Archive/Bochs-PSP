/////////////////////////////////////////////////////////////////////////
// $Id: gui.cc,v 1.80 2004/08/15 19:27:14 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#include <signal.h>
#include "bochs.h"
#include "iodev.h"
#include "gui/bitmaps/floppya.h"
#include "gui/bitmaps/floppyb.h"
#include "gui/bitmaps/mouse.h"
#include "gui/bitmaps/reset.h"
#include "gui/bitmaps/power.h"
#include "gui/bitmaps/snapshot.h"
#include "gui/bitmaps/copy.h"
#include "gui/bitmaps/paste.h"
#include "gui/bitmaps/configbutton.h"
#include "gui/bitmaps/cdromd.h"
#include "gui/bitmaps/userbutton.h"
#if BX_WITH_MACOS
#  include <Disks.h>
#endif

bx_gui_c *bx_gui = NULL;

#define BX_GUI_THIS bx_gui->
#define LOG_THIS BX_GUI_THIS

bx_gui_c::bx_gui_c(void)
{
  put("GUI"); // Init in specific_init
  settype(GUILOG);
  statusitem_count = 0;
  framebuffer = NULL;
}

bx_gui_c::~bx_gui_c()
{
  if (framebuffer != NULL) {
    delete [] framebuffer;
  }
}

  void
bx_gui_c::init(int argc, char **argv, unsigned tilewidth, unsigned tileheight)
{
  BX_GUI_THIS new_gfx_api = 0;
  BX_GUI_THIS host_xres = 640;
  BX_GUI_THIS host_yres = 480;
  BX_GUI_THIS host_bpp = 8;

  specific_init(argc, argv, tilewidth, tileheight, BX_HEADER_BAR_Y);

  // Define some bitmaps to use in the headerbar
  BX_GUI_THIS floppyA_bmap_id = create_bitmap(bx_floppya_bmap,
                          BX_FLOPPYA_BMAP_X, BX_FLOPPYA_BMAP_Y);
  BX_GUI_THIS floppyA_eject_bmap_id = create_bitmap(bx_floppya_eject_bmap,
                          BX_FLOPPYA_BMAP_X, BX_FLOPPYA_BMAP_Y);
  BX_GUI_THIS floppyB_bmap_id = create_bitmap(bx_floppyb_bmap,
                          BX_FLOPPYB_BMAP_X, BX_FLOPPYB_BMAP_Y);
  BX_GUI_THIS floppyB_eject_bmap_id = create_bitmap(bx_floppyb_eject_bmap,
                          BX_FLOPPYB_BMAP_X, BX_FLOPPYB_BMAP_Y);
  BX_GUI_THIS cdromD_bmap_id = create_bitmap(bx_cdromd_bmap,
                          BX_CDROMD_BMAP_X, BX_CDROMD_BMAP_Y);
  BX_GUI_THIS cdromD_eject_bmap_id = create_bitmap(bx_cdromd_eject_bmap,
                          BX_CDROMD_BMAP_X, BX_CDROMD_BMAP_Y);
  BX_GUI_THIS mouse_bmap_id = create_bitmap(bx_mouse_bmap,
                          BX_MOUSE_BMAP_X, BX_MOUSE_BMAP_Y);
  BX_GUI_THIS nomouse_bmap_id = create_bitmap(bx_nomouse_bmap,
                          BX_MOUSE_BMAP_X, BX_MOUSE_BMAP_Y);


  BX_GUI_THIS power_bmap_id = create_bitmap(bx_power_bmap, BX_POWER_BMAP_X, BX_POWER_BMAP_Y);
  BX_GUI_THIS reset_bmap_id = create_bitmap(bx_reset_bmap, BX_RESET_BMAP_X, BX_RESET_BMAP_Y);
  BX_GUI_THIS snapshot_bmap_id = create_bitmap(bx_snapshot_bmap, BX_SNAPSHOT_BMAP_X, BX_SNAPSHOT_BMAP_Y);
  BX_GUI_THIS copy_bmap_id = create_bitmap(bx_copy_bmap, BX_COPY_BMAP_X, BX_COPY_BMAP_Y);
  BX_GUI_THIS paste_bmap_id = create_bitmap(bx_paste_bmap, BX_PASTE_BMAP_X, BX_PASTE_BMAP_Y);
  BX_GUI_THIS config_bmap_id = create_bitmap(bx_config_bmap, BX_CONFIG_BMAP_X, BX_CONFIG_BMAP_Y);
  BX_GUI_THIS user_bmap_id = create_bitmap(bx_user_bmap, BX_USER_BMAP_X, BX_USER_BMAP_Y);


  // Add the initial bitmaps to the headerbar, and enable callback routine, for use
  // when that bitmap is clicked on

  // Floppy A:
  BX_GUI_THIS floppyA_status = DEV_floppy_get_media_status(0);
  if (BX_GUI_THIS floppyA_status)
    BX_GUI_THIS floppyA_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyA_bmap_id,
                          BX_GRAVITY_LEFT, floppyA_handler);
  else
    BX_GUI_THIS floppyA_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyA_eject_bmap_id,
                          BX_GRAVITY_LEFT, floppyA_handler);

  // Floppy B:
  BX_GUI_THIS floppyB_status = DEV_floppy_get_media_status(1);
  if (BX_GUI_THIS floppyB_status)
    BX_GUI_THIS floppyB_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyB_bmap_id,
                          BX_GRAVITY_LEFT, floppyB_handler);
  else
    BX_GUI_THIS floppyB_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyB_eject_bmap_id,
                          BX_GRAVITY_LEFT, floppyB_handler);

  // CDROM, 
  // valgrinds says that the harddrive object is not be initialised yet, 
  // so we just set the bitmap to ejected for now
#if 0
  if (DEV_hd_present()) {
    Bit32u handle = DEV_hd_get_first_cd_handle();
    BX_GUI_THIS cdromD_status = DEV_hd_get_cd_media_status(handle);
  }

  if (BX_GUI_THIS cdromD_status)
    BX_GUI_THIS cdromD_hbar_id = headerbar_bitmap(BX_GUI_THIS cdromD_bmap_id,
                          BX_GRAVITY_LEFT, cdromD_handler);
  else
#endif
    BX_GUI_THIS cdromD_hbar_id = headerbar_bitmap(BX_GUI_THIS cdromD_eject_bmap_id,
                          BX_GRAVITY_LEFT, cdromD_handler);

  // Mouse button
  if (bx_options.Omouse_enabled->get ())
    BX_GUI_THIS mouse_hbar_id = headerbar_bitmap(BX_GUI_THIS mouse_bmap_id,
                          BX_GRAVITY_LEFT, toggle_mouse_enable);
  else
    BX_GUI_THIS mouse_hbar_id = headerbar_bitmap(BX_GUI_THIS nomouse_bmap_id,
                          BX_GRAVITY_LEFT, toggle_mouse_enable);

  // These are the buttons on the right side.  They are created in order
  // of right to left.

  // Power button
  BX_GUI_THIS power_hbar_id = headerbar_bitmap(BX_GUI_THIS power_bmap_id,
                          BX_GRAVITY_RIGHT, power_handler);
  // Reset button
  BX_GUI_THIS reset_hbar_id = headerbar_bitmap(BX_GUI_THIS reset_bmap_id,
                          BX_GRAVITY_RIGHT, reset_handler);
  // Configure button
  BX_GUI_THIS config_hbar_id = headerbar_bitmap(BX_GUI_THIS config_bmap_id,
                          BX_GRAVITY_RIGHT, config_handler);
  // Snapshot button
  BX_GUI_THIS snapshot_hbar_id = headerbar_bitmap(BX_GUI_THIS snapshot_bmap_id,
                          BX_GRAVITY_RIGHT, snapshot_handler);
  // Paste button
  BX_GUI_THIS paste_hbar_id = headerbar_bitmap(BX_GUI_THIS paste_bmap_id,
                          BX_GRAVITY_RIGHT, paste_handler);
  // Copy button
  BX_GUI_THIS copy_hbar_id = headerbar_bitmap(BX_GUI_THIS copy_bmap_id,
                          BX_GRAVITY_RIGHT, copy_handler);
  // User button
  BX_GUI_THIS user_hbar_id = headerbar_bitmap(BX_GUI_THIS user_bmap_id,
                          BX_GRAVITY_RIGHT, userbutton_handler);

  if(bx_options.Otext_snapshot_check->get()) {
    bx_pc_system.register_timer(this, bx_gui_c::snapshot_checker, (unsigned) 1000000, 1, 1, "snap_chk");
  }

  BX_GUI_THIS charmap_updated = 0;

  if (!BX_GUI_THIS new_gfx_api) {
    BX_GUI_THIS framebuffer = new Bit8u[BX_MAX_XRES * BX_MAX_YRES * 4];
  }
  show_headerbar();
}

void
bx_gui_c::update_drive_status_buttons (void) {
  BX_GUI_THIS floppyA_status = 
    DEV_floppy_get_media_status(0)
    && bx_options.floppya.Ostatus->get ();
  BX_GUI_THIS floppyB_status = 
      DEV_floppy_get_media_status(1)
      && bx_options.floppyb.Ostatus->get ();
  Bit32u handle = DEV_hd_get_first_cd_handle();
  BX_GUI_THIS cdromD_status = DEV_hd_get_cd_media_status(handle);
  if (BX_GUI_THIS floppyA_status)
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(bx_options.floppya.Opath->getptr (), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_eject_bmap_id);
    }
  if (BX_GUI_THIS floppyB_status)
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(bx_options.floppyb.Opath->getptr (), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_eject_bmap_id);
    }
  if (BX_GUI_THIS cdromD_status)
    replace_bitmap(BX_GUI_THIS cdromD_hbar_id, BX_GUI_THIS cdromD_bmap_id);
  else {
    replace_bitmap(BX_GUI_THIS cdromD_hbar_id, BX_GUI_THIS cdromD_eject_bmap_id);
    }
}

  void
bx_gui_c::floppyA_handler(void)
{
  if (bx_options.floppya.Odevtype->get() == BX_FLOPPY_NONE)
    return; // no primary floppy device present
#ifdef WIN32
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
    // instead of just toggling the status, call win32dialog to bring up
    // a dialog asking what disk image you want to switch to.
    int ret = SIM->ask_param (BXP_FLOPPYA_PATH);
    if (ret > 0) {
      BX_GUI_THIS update_drive_status_buttons ();
    }
    return;
  }
#endif
  BX_GUI_THIS floppyA_status = !BX_GUI_THIS floppyA_status;
  DEV_floppy_set_media_status(0, BX_GUI_THIS floppyA_status);
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::floppyB_handler(void)
{
  if (bx_options.floppyb.Odevtype->get() == BX_FLOPPY_NONE)
    return; // no secondary floppy device present
#ifdef WIN32
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
    // instead of just toggling the status, call win32dialog to bring up
    // a dialog asking what disk image you want to switch to.
    int ret = SIM->ask_param (BXP_FLOPPYB_PATH);
    if (ret > 0) {
      BX_GUI_THIS update_drive_status_buttons ();
    }
    return;
  }
#endif
  BX_GUI_THIS floppyB_status = !BX_GUI_THIS floppyB_status;
  DEV_floppy_set_media_status(1, BX_GUI_THIS floppyB_status);
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::cdromD_handler(void)
{
  Bit32u handle = DEV_hd_get_first_cd_handle();
#ifdef WIN32
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
    // instead of just toggling the status, call win32dialog to bring up 
    // a dialog asking what disk image you want to switch to.
    // This code handles the first cdrom only. The cdrom drives #2, #3 and
    // #4 are handled in the win32 runtime dialog.
    bx_param_c *cdrom = SIM->get_first_cdrom ();
    if (cdrom == NULL)
      return;  // no cdrom found
    int ret = SIM->ask_param (cdrom->get_id ());
    if (ret > 0) {
      BX_GUI_THIS update_drive_status_buttons ();
    }
    return;
  }
#endif
  BX_GUI_THIS cdromD_status =
    DEV_hd_set_cd_media_status(handle, !BX_GUI_THIS cdromD_status);
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::reset_handler(void)
{
  BX_INFO(( "system RESET callback." ));
  bx_pc_system.Reset( BX_RESET_HARDWARE );
}

  void
bx_gui_c::power_handler(void)
{
  // the user pressed power button, so there's no doubt they want bochs
  // to quit.  Change panics to fatal for the GUI and then do a panic.
  bx_user_quit = 1;
  LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
  BX_PANIC (("POWER button turned off."));
  // shouldn't reach this point, but if you do, QUIT!!!
  fprintf (stderr, "Bochs is exiting because you pressed the power button.\n");
  BX_EXIT (1);
}

Bit32s
bx_gui_c::make_text_snapshot (char **snapshot, Bit32u *length)
{
  Bit8u* raw_snap = NULL;
  char *clean_snap;
  unsigned line_addr, txt_addr, txHeight, txWidth;

  DEV_vga_get_text_snapshot(&raw_snap, &txHeight, &txWidth);
  if (txHeight <= 0) return -1;
  clean_snap = (char*) malloc(txHeight*(txWidth+2)+1);
  txt_addr = 0;
  for (unsigned i=0; i<txHeight; i++) {
    line_addr = i * txWidth * 2;
    for (unsigned j=0; j<(txWidth*2); j+=2) {
      clean_snap[txt_addr++] = raw_snap[line_addr+j];
    }
    while ((txt_addr > 0) && (clean_snap[txt_addr-1] == ' ')) txt_addr--;
#ifdef WIN32
    if(!(bx_options.Otext_snapshot_check->get())) {
      clean_snap[txt_addr++] = 13;
    }
#endif
    clean_snap[txt_addr++] = 10;
  }
  clean_snap[txt_addr] = 0;
  *snapshot = clean_snap;
  *length = txt_addr;
  return 0;
}

// create a text snapshot and copy to the system clipboard.  On guis that
// we haven't figured out how to support yet, dump to a file instead.
  void
bx_gui_c::copy_handler(void)
{
  Bit32u len;
  char *text_snapshot;
  if (make_text_snapshot (&text_snapshot, &len) < 0) {
    BX_INFO(( "copy button failed, mode not implemented"));
    return;
  }
  if (!BX_GUI_THIS set_clipboard_text(text_snapshot, len)) {
    // platform specific code failed, use portable code instead
    FILE *fp = fopen("copy.txt", "w");
    fwrite(text_snapshot, 1, len, fp);
    fclose(fp);
  }
  free(text_snapshot);
}

// Check the current text snapshot against file snapchk.txt.
  void
bx_gui_c::snapshot_checker(void * this_ptr)
{
  char filename[BX_PATHNAME_LEN];
  strcpy(filename,"snapchk.txt");
  FILE *fp = fopen(filename, "rb");
  if(fp) {
    char *text_snapshot;
    Bit32u len;
    if (make_text_snapshot (&text_snapshot, &len) < 0) {
      return;
    }
    char *compare_snapshot = (char *) malloc((len+1) * sizeof(char));
    fread(compare_snapshot, 1, len, fp);
    fclose(fp);
    strcpy(filename,"snapmask.txt");
    fp=fopen(filename, "rb");
    if(fp) {
      char *mask_snapshot = (char *) malloc((len+1) * sizeof(char));
      unsigned i;
      bx_bool flag = 1;
      fread(mask_snapshot, 1, len, fp);
      fclose(fp);
      for(i=0;i<len;i++) {
	if((text_snapshot[i] != compare_snapshot[i]) &&
	   (compare_snapshot[i] == mask_snapshot[i])) {
	  flag = 0;
	  break;
	}
      }
      if(flag) {
	if(!memcmp(text_snapshot,compare_snapshot,len)) {
	  BX_PASS(("Test Passed."));
	} else {
	  BX_PASS(("Test Passed with Mask."));
	}
      }
    } else {
      if(!memcmp(text_snapshot,compare_snapshot,len)) {
	BX_PASS(("Test Passed."));
      }
    }
    free(compare_snapshot);
    free(text_snapshot);
  }
}

// create a text snapshot and dump it to a file
  void
bx_gui_c::snapshot_handler(void)
{
  char *text_snapshot;
  Bit32u len;
  if (make_text_snapshot (&text_snapshot, &len) < 0) {
    BX_ERROR(( "snapshot button failed, mode not implemented"));
    return;
  }
  //FIXME
  char filename[BX_PATHNAME_LEN];
#ifdef WIN32
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
#else
  if (!strcmp(bx_options.Osel_config->get_choice(bx_options.Osel_config->get()),
              "wx")) {
#endif
    int ret = SIM->ask_filename (filename, sizeof(filename),
                                 "Save snapshot as...", "snapshot.txt",
                                 bx_param_string_c::SAVE_FILE_DIALOG);
    if (ret < 0) { // cancelled
      free(text_snapshot);
      return;
    }
  } else {
    strcpy (filename, "snapshot.txt");
  }
  FILE *fp = fopen(filename, "wb");
  fwrite(text_snapshot, 1, len, fp);
  fclose(fp);
  free(text_snapshot);
}

// Read ASCII chars from the system clipboard and paste them into bochs.
// Note that paste cannot work with the key mapping tables loaded.
  void
bx_gui_c::paste_handler(void)
{
  Bit32s nbytes;
  Bit8u *bytes;
  if (!bx_keymap.isKeymapLoaded ()) {
    BX_ERROR (("keyboard_mapping disabled, so paste cannot work"));
    return;
  }
  if (!BX_GUI_THIS get_clipboard_text(&bytes, &nbytes)) {
    BX_ERROR (("paste not implemented on this platform"));
    return;
  }
  BX_INFO (("pasting %d bytes", nbytes));
  DEV_kbd_paste_bytes (bytes, nbytes);
}


  void
bx_gui_c::config_handler(void)
{
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
    SIM->configuration_interface (NULL, CI_RUNTIME_CONFIG);
  }
}

  void
bx_gui_c::toggle_mouse_enable(void)
{
  int old = bx_options.Omouse_enabled->get ();
  BX_DEBUG (("toggle mouse_enabled, now %d", !old));
  bx_options.Omouse_enabled->set (!old);
}

  void
bx_gui_c::userbutton_handler(void)
{
  unsigned shortcut[4];
  unsigned p;
  char *user_shortcut;
  int i, len, ret = 1;

  len = 0;
#ifdef WIN32
  if (strcmp(bx_options.Osel_displaylib->get_choice(bx_options.Osel_displaylib->get()),
              "rfb")) {
#else
  if (!strcmp(bx_options.Osel_config->get_choice(bx_options.Osel_config->get()),
              "wx")) {
#endif
    ret = SIM->ask_param (BXP_USER_SHORTCUT);
  }
  user_shortcut = bx_options.Ouser_shortcut->getptr();
  if ((ret > 0) && user_shortcut[0] && (strcmp(user_shortcut, "none"))) {
    len = 0;
    p = 0;
    while ((p < strlen(user_shortcut)) && (len < 3)) {
      if (!strncmp(user_shortcut+p, "alt", 3)) {
        shortcut[len++] = BX_KEY_ALT_L;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "ctrl", 4)) {
        shortcut[len++] = BX_KEY_CTRL_L;
        p += 4;
      } else if (!strncmp(user_shortcut+p, "del", 3)) {
        shortcut[len++] = BX_KEY_DELETE;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "esc", 3)) {
        shortcut[len++] = BX_KEY_ESC;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "f1", 2)) {
        shortcut[len++] = BX_KEY_F1;
        p += 2;
      } else if (!strncmp(user_shortcut+p, "f4", 2)) {
        shortcut[len++] = BX_KEY_F4;
        p += 2;
      } else if (!strncmp(user_shortcut+p, "tab", 3)) {
        shortcut[len++] = BX_KEY_TAB;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "win", 3)) {
        shortcut[len++] = BX_KEY_WIN_L;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "bksp", 4)) {
        shortcut[len++] = BX_KEY_BACKSPACE;
        p += 4;
      } else {
        BX_ERROR(("Unknown shortcut %s ignored", user_shortcut));
        return;
      }
    }
    i = 0;
    while (i < len) {
      DEV_kbd_gen_scancode(shortcut[i++]);
    }
    i--;
    while (i >= 0) {
      DEV_kbd_gen_scancode(shortcut[i--] | BX_KEY_RELEASED);
    }
  }
}

  void
bx_gui_c::mouse_enabled_changed (bx_bool val)
{
  // This is only called when SIM->get_init_done is 1.  Note that VAL
  // is the new value of mouse_enabled, which may not match the old
  // value which is still in bx_options.Omouse_enabled->get ().
  BX_DEBUG (("replacing the mouse bitmaps"));
  if (val)
    BX_GUI_THIS replace_bitmap(BX_GUI_THIS mouse_hbar_id, BX_GUI_THIS mouse_bmap_id);
  else
    BX_GUI_THIS replace_bitmap(BX_GUI_THIS mouse_hbar_id, BX_GUI_THIS nomouse_bmap_id);
  // give the GUI a chance to respond to the event.  Most guis will hide
  // the native mouse cursor and do something to trap the mouse inside the
  // bochs VGA display window.
  BX_GUI_THIS mouse_enabled_changed_specific (val);
}

void
bx_gui_c::init_signal_handlers ()
{
#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler) 
  {
    Bit32u mask = bx_gui->get_sighandler_mask ();
    for (Bit32u sig=0; sig<32; sig++)
    {
      if (mask & (1<<sig))
        signal (sig, bx_signal_handler);
    }
  }
#endif
}

  void
bx_gui_c::set_text_charmap(Bit8u *fbuffer)
{
  memcpy(& BX_GUI_THIS vga_charmap, fbuffer, 0x2000);
  for (unsigned i=0; i<256; i++) BX_GUI_THIS char_changed[i] = 1;
  BX_GUI_THIS charmap_updated = 1;
}

  void
bx_gui_c::set_text_charbyte(Bit16u address, Bit8u data)
{
  BX_GUI_THIS vga_charmap[address] = data;
  BX_GUI_THIS char_changed[address >> 5] = 1;
  BX_GUI_THIS charmap_updated = 1;
}

  
  void
bx_gui_c::beep_on(float frequency)
{
  BX_INFO(( "GUI Beep ON (frequency=%.2f)",frequency));
}

  void
bx_gui_c::beep_off()
{
  BX_INFO(( "GUI Beep OFF"));
}

  int
bx_gui_c::register_statusitem(const char *text)
{
  if (statusitem_count < BX_MAX_STATUSITEMS) {
    strncpy(statusitem_text[statusitem_count], text, 8);
    statusitem_text[statusitem_count][7] = 0;
    return statusitem_count++;
  } else {
   return -1;
  }
}

  void
bx_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  *xres = 1024;
  *yres = 768;
  *bpp = 32;
}

  bx_svga_tileinfo_t *
bx_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  if (!info) {
    info = (bx_svga_tileinfo_t *)malloc(sizeof(bx_svga_tileinfo_t));
    if (!info) {
      return NULL;
    }
  }

  BX_GUI_THIS host_pitch = BX_GUI_THIS host_xres * ((BX_GUI_THIS host_bpp + 1) >> 3);

  info->bpp = BX_GUI_THIS host_bpp;
  info->pitch = BX_GUI_THIS host_pitch;
  switch (info->bpp) {
    case 15:
      info->red_shift = 15;
      info->green_shift = 10;
      info->blue_shift = 5;
      info->red_mask = 0x7c00;
      info->green_mask = 0x03e0;
      info->blue_mask = 0x001f;
      break;
    case 16:
      info->red_shift = 16;
      info->green_shift = 11;
      info->blue_shift = 5;
      info->red_mask = 0xf800;
      info->green_mask = 0x07e0;
      info->blue_mask = 0x001f;
      break;
    case 24:
    case 32:
      info->red_shift = 24;
      info->green_shift = 16;
      info->blue_shift = 8;
      info->red_mask = 0xff0000;
      info->green_mask = 0x00ff00;
      info->blue_mask = 0x0000ff;
      break;
  }
  info->is_indexed = (BX_GUI_THIS host_bpp == 8);
#ifdef BX_LITTLE_ENDIAN
  info->is_little_endian = 1;
#else
  info->is_little_endian = 0;
#endif

  return info;
}

  Bit8u *
bx_gui_c::graphics_tile_get(unsigned x0, unsigned y0,
                            unsigned *w, unsigned *h)
{
  if (x0+X_TILESIZE > BX_GUI_THIS host_xres) {
    *w = BX_GUI_THIS host_xres - x0;
  }
  else {
    *w = X_TILESIZE;
  }

  if (y0+Y_TILESIZE > BX_GUI_THIS host_yres) {
    *h = BX_GUI_THIS host_yres - y0;
  }
  else {
    *h = Y_TILESIZE;
  }

  return (Bit8u *)framebuffer + y0 * BX_GUI_THIS host_pitch +
                  x0 * ((BX_GUI_THIS host_bpp + 1) >> 3);
}

  void
bx_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                        unsigned w, unsigned h)
{
  Bit8u tile[X_TILESIZE * Y_TILESIZE * 4];
  Bit8u *tile_ptr, *fb_ptr;
  Bit16u xc, yc, fb_pitch, tile_pitch;
  Bit8u r, diffx, diffy;

  diffx = (x0 % X_TILESIZE);
  diffy = (y0 % Y_TILESIZE);
  if (diffx > 0) {
    x0 -= diffx;
    w += diffx;
  }
  if (diffy > 0) {
    y0 -= diffy;
    h += diffy;
  }
  fb_pitch = BX_GUI_THIS host_pitch;
  tile_pitch = X_TILESIZE * ((BX_GUI_THIS host_bpp + 1) >> 3);
  for (yc=y0; yc<(y0+h); yc+=Y_TILESIZE) {
    for (xc=x0; xc<(x0+w); xc+=X_TILESIZE) {
      fb_ptr = BX_GUI_THIS framebuffer + (yc * fb_pitch + xc * ((BX_GUI_THIS host_bpp + 1) >> 3));
      tile_ptr = &tile[0];
      for (r=0; r<h; r++) {
        memcpy(tile_ptr, fb_ptr, tile_pitch);
        fb_ptr += fb_pitch;
        tile_ptr += tile_pitch;
      }
      BX_GUI_THIS graphics_tile_update(tile, xc, yc);
    }
  }
}
