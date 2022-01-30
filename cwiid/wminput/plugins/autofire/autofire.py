# -*- coding: utf-8 -*-

# Copyright (C) 2013 Frank Löffler <knarf@cct.lsu.edu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

# This plugin enables one button of a wiimote to use autofire: repeated on/off
# events as long as this button is pressed.
# The only parameter is "Button", specifying the autofire button, unfortunately
#   I don't know how to specify that differently than the internal enum.

# Example: to enable autofire on Space for the "A" button, add this to your
# config:
#   Plugin.autofire.Button = 8
#   Plugin.autofire.Key    = KEY_SPACE

import wmplugin
import cwiid

# This disables effects from the module by default, use e.g. 8 for BTN_A
Button = 0

# remember state of Button
pressed     = False
# current state of autofire (flip-flop)
fire_toggle = False


# register one Key of our own, and one parameter Button
def wmplugin_info():
  return ["Key"], [], [("Button", wmplugin.PARAM_INT, Button)]


# Register as BTN plugin since we set one button of our own, and
# as IR plugin because we use whatever IR events we get (many) to
# trigger autofire
def wmplugin_init(id, wiimote):
  wmplugin.set_rpt_mode(id, cwiid.RPT_IR|cwiid.RPT_BTN)
  return


# This is called whenever something changes in the inputs of this plugin
def wmplugin_exec(mesg):
  global pressed, fire_toggle, Button
  # early escape for disabled plugin
  if Button == 0:
    return [False], []
  # go through different message types (e.g. IR/Buttons/...)
  for m in mesg:
    # For button events: look for the button we trigger on
    if m[0] == cwiid.MESG_BTN:
      pressed = m[1] & Button
      # if released, release key
      if (not pressed):
        fire_toggle = False
    # For IR events: trigger autofire if trigger button is currently pressed
    if m[0] == cwiid.MESG_IR:
      if pressed:
        fire_toggle = not fire_toggle

  # set "Key" to whatever autofire status we have
  return [fire_toggle], []
