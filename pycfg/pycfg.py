#!/usr/bin/env python

#
#  Copyright (C) 2014-2018 Roman Pauer
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy of
#  this software and associated documentation files (the "Software"), to deal in
#  the Software without restriction, including without limitation the rights to
#  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
#  of the Software, and to permit persons to whom the Software is furnished to do
#  so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#

import os
import datetime
import sys

import pygtk
pygtk.require('2.0')
import gtk

class ConfigEntry:
    def __init__(self, entry_name, data_format, default_value):
        self.EntryName = entry_name
        self.DataFormat = data_format
        self.DefaultValue = default_value
        self.FileValue = default_value
        self.CurrentValue = default_value

        self.DataType = "Unknown"
        if "/" in self.DataFormat:
            self.DataType = "ListOfValues"
            self.ListOfValuesLow = self.DataFormat.lower().split("/")
        elif "-" in self.DataFormat:
            self.DataType = "IntegerRange"
            partition = self.DataFormat.partition("-")
            self.MinIntValue = int(partition[0])
            self.MaxIntValue = int(partition[2])
        elif self.DataFormat == "*":
            self.DataType = "*"
        else:
            raise BaseException, "ConfigEntry: unknown data format"


    def ResetValue(self):
        self.FileValue = self.DefaultValue
        self.CurrentValue = self.DefaultValue

    def GetFormat(self):
        return self.DataFormat

    def GetName(self):
        return self.EntryName

    def GetValue(self):
        return self.CurrentValue

    def IsValueChanged(self):
        return self.CurrentValue != self.FileValue

    def CorrectValue(self, value):
        if self.DataType == "ListOfValues":
            if value.lower() in self.ListOfValuesLow:
                return value
        elif self.DataType == "IntegerRange":
            try:
                ivalue = int(value)
            except ValueError:
                return self.DefaultValue

            if ivalue < self.MinIntValue:
                return str(self.MinIntValue)
            elif ivalue > self.MaxIntValue:
                return str(self.MaxIntValue)
            else:
                return value
        else:
            return value

        return self.DefaultValue

    def LoadFileValue(self, file_value):
        self.FileValue = file_value

        self.CurrentValue = self.CorrectValue(file_value)

    def RestoreDefaultValue(self):
        self.CurrentValue = self.DefaultValue

    def RestoreFileValue(self):
        self.CurrentValue = self.FileValue

    def SaveFileValue(self):
        self.FileValue = self.CurrentValue

        return self.CurrentValue

    def SetValue(self, new_value):
        if new_value == self.CorrectValue(new_value):
            self.CurrentValue = new_value
        else:
            raise BaseException, "ConfigEntry: wrong value"


class ConfigFile:
    def __init__(self, game, platform):
        self.Entries = dict()
        self.Lines = []
        self.EOL = ""

        # misc entries
        self.AddEntry("Sound", "on/off", "on")
        self.AddEntry("Music", "on/off", "on")

        if game == "albion" or game == "xcom1":
            self.AddEntry("Intro", "on/off", "on")

        if game == "albion":
            self.AddEntry("Screenshot_Format", "Original/LBM/LBM_pad16/TGA/BMP", "Original")

        if platform == "pandora":
            self.AddEntry("Use_Alternative_SDL", "yes/no", "yes")


        # display entries
        if platform == "pandora":
            self.AddEntry("Display_Mode", "double_pixels/fullscreen/correct_aspect_ratio", ("double_pixels" if game == "albion" else "fullscreen"))

        if platform == "gp2x":
            if game == "albion":
                self.AddEntry("Tvout_Scaling", "on/off", "on")
            else:
                self.AddEntry("Display", "stretched/original", "stretched")

        if platform == "pc":
            if game == "albion":
                self.AddEntry("Display_ScaledWidth", "640-3840", "720")
                self.AddEntry("Display_ScaledHeight", "480-2160", "480")
            else:
                self.AddEntry("Display_ScaledWidth", "640-3840", "640")
                self.AddEntry("Display_ScaledHeight", "400-2160", "400")

            self.AddEntry("Display_Fullscreen", "yes/no", "no")
            self.AddEntry("Display_MouseCursor", "normal/minimal/none", "normal")

        if game == "albion":
            if platform == "pandora" or platform == "pc":
                self.AddEntry("Display_Enhanced_3D_Rendering", "on/off", "on")

        # audio entries
        self.AddEntry("Audio_Channels", "stereo/mono", ("mono" if platform == "gp2x" else "stereo"))
        self.AddEntry("Audio_Resolution", "16/8", "16")
        self.AddEntry("Audio_Sample_Rate", "11025-48000", ("44100" if platform == "pc" else "22050"))
        self.AddEntry("Audio_Interpolation", "on/off", ("off" if platform == "gp2x" else "on"))

        if platform == "gp2x":
            self.AddEntry("Audio_MIDI_Subsystem", "wildmidi/adlmidi/sdl_mixer", "sdl_mixer")
        else:
            midi_values = "alsa/wildmidi"
            if game != "warcraft":
                midi_values += "/bassmidi"
            if game == "albion" or game == "warcraft":
                midi_values += "/adlmidi"
            midi_values += "/sdl_mixer"
            if game == "xcom1" or game == "xcom2":
                midi_values += "/adlib-dosbox_opl"
                if platform == "pc":
                    midi_values += "/mt32-munt"
            self.AddEntry("Audio_MIDI_Subsystem", midi_values, ("adlmidi" if game == "albion" or game == "warcraft" else "wildmidi"))

            if "alsa" in midi_values:
                self.AddEntry("Audio_MIDI_Device", "*", "")

            if "bassmidi" in midi_values:
                self.AddEntry("Audio_SoundFont_Path", "*", "")

            if "mt32-munt" in midi_values:
                self.AddEntry("Audio_MT32_Roms_Path", "*", "")

        if game == "albion":
            self.AddEntry("Audio_Swap_Channels", "yes/no", "yes")

        if game == "warcraft":
            self.AddEntry("Audio_Music_Volume", "0-127", ("50" if platform == "gp2x" else "80"))

        if game == "xcom1" or game == "xcom2":
            self.AddEntry("Audio_Music_Volume", "0-128", ("128" if platform == "gp2x" else "128"))
            self.AddEntry("Audio_Sample_Volume", "0-128", ("64" if platform == "gp2x" else "128"))

            self.AddEntry("Audio_Buffer_Size", "256-8192", ("2048" if platform == "pc" else "1024"))
        else:
            self.AddEntry("Audio_Buffer_Size", "256-8192", ("256" if platform == "gp2x" else ("4096" if platform == "pc" else "2048")))


        # input entries
        if platform == "pandora":
            self.AddEntry("Input_Mode", "touchscreen_dpad/touchscreen_abxy/keyboard_dpad", "touchscreen_dpad")

        if platform == "pandora" or platform == "pc":
            if game == "warcraft":
                self.AddEntry("Input_MouseHelper", "on/off", "off")
                self.AddEntry("Input_SelectGroupTreshold", "0-20", "6")

        if platform == "pandora":
            if game == "xcom1" or game == "xcom2":
                self.AddEntry("Input_Old_Touchscreen_Mode", "on/off", "off")

            # todo: Button_

        if platform == "gp2x":
            if game == "albion" or game == "xcom1" or game == "xcom2":
                self.AddEntry("Touchscreen", "on/off", "off")

            if game == "albion":
                self.AddEntry("Stylus_Clicks", "on/off", "off")
                self.AddEntry("Cursor_Buttons", "dpad/abxy", "dpad")

            if game == "xcom1" or game == "xcom2":
                self.AddEntry("Scrollbuttons", "F100/F200_rh/F200_lh", "F100")

            # todo: Button_

        #   pandora
        # Action_key_esc
        # Action_mouse_left_button
        # Action_mouse_right_button
        # Action_virtual_keyboard
        # Action_toggle_scaling
        # Action_pause
        # Action_none

        #   albion pandora
        # Action_key_alt
        # Action_key_pagedown
        # Action_key_pageup
        # Action_key_tab

        #   warcraft pandora
        # Action_key_fN - N = 1-10
        # Action_key_X - X = 32-127
        # Action_macro_key_X_mouse_left_button - X = 32-127
        # Action_combo_mouse_left_button_select_group

        #   xcom1, xcom2 pandora
        # Action_key_enter
        # Action_levelup
        # Action_leveldown
        # Action_selectnextsoldier
        # Action_deselectcurrentsoldier

        #   gp2x
        # Action_key_backspace
        # Action_key_X - X = 32-127
        # Action_key_
        # Action_mouse_left_button
        # Action_mouse_right_button
        # Action_virtual_keyboard
        # Action_volume_increase
        # Action_volume_decrease
        # Action_none

        #   albion gp2x
        # Action_key_alt
        # Action_key_ctrl
        # Action_key_esc
        # Action_key_pagedown
        # Action_key_pageup
        # Action_key_tab
        # Action_key_fN - N = 1-6
        # Action_toggle_scaling
        # Action_pause

        #   warcraft gp2x
        # Action_key_ctrl
        # Action_key_enter
        # Action_key_esc
        # Action_key_shift
        # Action_key_fN - N = 1-10
        # Action_macro_key_X_mouse_left_button - X = 32-127
        # Action_combo_mouse_left_button_select_group

        #   xcom1, xcom2 gp2x
        # Action_key_enter
        # Action_virtual_keyboard
        # Action_rotateup
        # Action_rotatedown
        # Action_rotateleft
        # Action_rotateright
        # Action_levelup
        # Action_leveldown
        # Action_selectnextsoldier
        # Action_deselectcurrentsoldier
        # Action_pause

    def AddEntry(self, entry_name, data_format, default_value):
        self.Entries[entry_name.lower()] = ConfigEntry(entry_name, data_format, default_value)

    def AreValuesChanged(self):
        for entry_lname in self.Entries.iterkeys():
            if self.Entries[entry_lname].IsValueChanged():
                return True

        return False

    def ResetValues(self):
        for entry_lname in self.Entries.iterkeys():
            self.Entries[entry_lname].ResetValue()

    def HasEntry(self, entry_name):
        return entry_name.lower() in self.Entries

    def GetEntryFormat(self, entry_name):
        return self.Entries[entry_name.lower()].GetFormat()

    def GetEntryName(self, entry_name):
        return self.Entries[entry_name.lower()].GetName()

    def GetEntryValue(self, entry_name):
        return self.Entries[entry_name.lower()].GetValue()

    def SetEntryValue(self, entry_name, new_value):
        self.Entries[entry_name.lower()].SetValue(new_value)

    def RestoreEntryDefaultValue(self, entry_name):
        self.Entries[entry_name.lower()].RestoreDefaultValue()

    def RestoreEntryFileValue(self, entry_name):
        self.Entries[entry_name.lower()].RestoreFileValue()

    def ReadConfigFile(self, file_path):
        self.ResetValues()
        self.Lines = []
        fCfg = open(file_path, "rU")

        for OrigLine in fCfg:
            Line = OrigLine.strip(" \r\n")
            IsEntry = False

            EqIndex = Line.find("=")

            if (not Line.startswith("#")) and (EqIndex != -1):
                EntryName = Line[:EqIndex].strip(" ")
                EntryValue = Line[EqIndex + 1:].strip(" ")

                if self.HasEntry(EntryName):
                    IsEntry = True

            if IsEntry:
                self.Entries[EntryName.lower()].LoadFileValue(EntryValue)

                self.Lines.append((OrigLine.strip("\r\n"), EntryName.lower()))
            else:
                self.Lines.append((OrigLine.strip("\r\n"), ""))

        if fCfg.newlines is None:
            self.EOL = ""
        elif type(fCfg.newlines) is tuple:
            self.EOL = ""
        else:
            self.EOL = fCfg.newlines

        fCfg.close()

    def WriteConfigFile(self, file_path):
        if (self.EOL == ""):
            fCfg = open(file_path, "wt")
            EOL = "\n"
        else:
            fCfg = open(file_path, "wb")
            EOL = self.EOL

        UsedEntryNamesLow = set([])
        for (OrigLine, EntryNameLow) in self.Lines:
            if EntryNameLow == "":
                fCfg.write(OrigLine + EOL)
            else:
                UsedEntryNamesLow.add(EntryNameLow)
                fCfg.write(self.Entries[EntryNameLow].GetName() + "=" + self.Entries[EntryNameLow].SaveFileValue() + EOL)

        FirstNewEntry = True
        for EntryNameLow in self.Entries.iterkeys():
            if not EntryNameLow in UsedEntryNamesLow:
                if FirstNewEntry:
                    FirstNewEntry = False
                    fCfg.write("# added automatically by configurator: " + datetime.datetime.now().isoformat(" ") + EOL)

                fCfg.write(self.Entries[EntryNameLow].GetName() + "=" + self.Entries[EntryNameLow].SaveFileValue() + EOL)

        fCfg.close()


class ConfigGUI:
    def delete(self, widget, event=None):
        gtk.main_quit()
        return False

    def __init__(self, game, platform, file_path):
        self.file_path = file_path
        self.widgets = []

        self.CfgFile = ConfigFile(game, platform)
        self.CfgFile.ReadConfigFile(file_path)

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("delete_event", self.delete)

        self.window.set_size_request(790, 420)
        self.window.set_title(os.path.basename(file_path))

        mainvbox = gtk.VBox(False, 0)
        self.window.add(mainvbox)

        # Create a new notebook, place the position of the tabs
        notebook = gtk.Notebook()
        notebook.set_tab_pos(gtk.POS_LEFT)
        mainvbox.pack_start(notebook, True, True, 0)
        notebook.show()
        self.show_tabs = True
        self.show_border = True

        if self.CfgFile.HasEntry("Intro"):
            vbox = self.AddPageFrameVBox(notebook, "Intro", "Intro")

            self.CreateRadioSet(vbox, "Intro:", "Intro", "Select whether intro is played when the game is started.")

        if self.CfgFile.HasEntry("Sound") or self.CfgFile.HasEntry("Music"):
            vbox = self.AddPageFrameVBox(notebook, "Music/Sound", "Music / Sound")

            if self.CfgFile.HasEntry("Music"):
                self.CreateRadioSet(vbox, "Music:", "Music", "Select whether music is played during the game.")

            if self.CfgFile.HasEntry("Music"):
                if self.CfgFile.HasEntry("Sound"):
                    self.CreateSeparator(vbox)

                self.CreateRadioSet(vbox, "Sound:", "Sound", "Select whether sound effects are played during the game.")

        if self.CfgFile.HasEntry("Audio_Channels"):
            vbox = self.AddPageFrameVBox(notebook, "Audio", "Audio parameters")

            self.CreateRadioSet(vbox, "Channels:", "Audio_Channels", "Number of audio channels.")
            self.CreateSeparator(vbox)
            self.CreateRadioSet(vbox, "Resolution:", "Audio_Resolution", "Audio resolution - 8 or 16 bits.")
            self.CreateSeparator(vbox)
            self.CreateRadioSet2(vbox, "Sample Rate:", "Audio_Sample_Rate", "11025/22050/44100", "Audio frequency in Hz.")
            self.CreateSeparator(vbox)
            self.CreateRadioSet2(vbox, "Buffer Size:", "Audio_Buffer_Size", "128/256/512/1024/2048/4096/8192", "Audio buffer size in bytes.")

        if self.CfgFile.HasEntry("Audio_Interpolation"):
            vbox = self.AddPageFrameVBox(notebook, "Audio 2", "Audio parameters")

            self.CreateRadioSet(vbox, "Interpolation ?", "Audio_Interpolation", "Select whether to use audio interpolation or not.")

            if self.CfgFile.HasEntry("Audio_Swap_Channels"):
                self.CreateSeparator(vbox)

                description = "Select whether to swap left and right sound channel or not."
                if game == "albion":
                    description += "\nAlbion has the channels swapped, so swapping them corrects it."
                self.CreateRadioSet(vbox, "Swap Channels ?", "Audio_Swap_Channels", description)

            if self.CfgFile.HasEntry("Audio_Music_Volume"):
                self.CreateSeparator(vbox)
                self.CreateScale(vbox, "Music Volume:", "Audio_Music_Volume", "Music volume loudness relative to overall volume")

            if self.CfgFile.HasEntry("Audio_Sample_Volume"):
                self.CreateSeparator(vbox)
                self.CreateScale(vbox, "Sample Volume:", "Audio_Sample_Volume", "Sound sample volume loudness relative to overall volume")

        if self.CfgFile.HasEntry("Audio_MIDI_Subsystem"):
            vbox = self.AddPageFrameVBox(notebook, "MIDI", "MIDI")

            description = "Select library for music playback."

            if "adlib-dosbox_opl" in self.CfgFile.GetEntryFormat("Audio_MIDI_Subsystem"):
                description += "\nGeneral MIDI music (ALSA sequencer or WildMIDI, BASSMIDI, SDL_mixer library)"
                if self.CfgFile.HasEntry("Audio_MT32_Roms_Path"):
                    description += ", Adlib music or MT-32 music."
                else:
                    description += " or Adlib music."

            if "alsa" in self.CfgFile.GetEntryFormat("Audio_MIDI_Subsystem"):
                description += "\nALSA sequencer can use hardware or software synth (like Fluidsynth or TiMidity++)."

            description += "\nWildMIDI sounds better than SDL_mixer."

            if game == "albion":
                description += "\nSDL_mixer can only play one MIDI stream simultaneously."

            description += "\nWildMIDI and SDL_mixer use GUS patch files for playback."
            if self.CfgFile.HasEntry("Audio_SoundFont_Path"):
                description += "\nBASSMIDI uses soundfont for playback."

            if "adlmidi" in self.CfgFile.GetEntryFormat("Audio_MIDI_Subsystem"):
                description += "\nADLMIDI uses OPL3 emulator for playback."

            if "adlib-dosbox_opl" in self.CfgFile.GetEntryFormat("Audio_MIDI_Subsystem"):
                description += "\nAdlib music is played using 'compat' OPL emulator from DOSBox.\n(Works only for the DOS game version.)"

            if self.CfgFile.HasEntry("Audio_MT32_Roms_Path"):
                description += "\nMT-32 music is played using MUNT emulator. (Works only for the DOS game version.)"
                if game == "xcom1":
                    description += "\n\tIn the intro the playback falls back to adlib music when MT-32 music for intro is not present."

            self.CreateRadioSet(vbox, "MIDI Subsystem:", "Audio_MIDI_Subsystem", description)

            num_extra_options = 0

            if self.CfgFile.HasEntry("Audio_MIDI_Device"):
                num_extra_options += 1

            if self.CfgFile.HasEntry("Audio_SoundFont_Path"):
                num_extra_options += 1

            if self.CfgFile.HasEntry("Audio_MT32_Roms_Path"):
                num_extra_options += 1

            if num_extra_options == 1:
                self.CreateSeparator(vbox)
            elif num_extra_options > 1:
                vbox = self.AddPageFrameVBox(notebook, "MIDI 2", "MIDI")

            if self.CfgFile.HasEntry("Audio_MIDI_Device"):
                self.CreateEntry(vbox, "MIDI Device:", "Audio_MIDI_Device", "Client name or port (e.g. 128:0). No value = autodetection.\nThis is necessary when MIDI playback using ALSA sequencer is selected.")
                num_extra_options -= 1
                if num_extra_options != 0:
                    self.CreateSeparator(vbox)

            if self.CfgFile.HasEntry("Audio_SoundFont_Path"):
                self.CreateSoundfontSelector(vbox, "SoundFont Path:", "Audio_SoundFont_Path", "Set path to soundfont file. No value = autodetection in game's directory.\nThis is necessary when MIDI playback using BASSMIDI is selected.")
                num_extra_options -= 1
                if num_extra_options != 0:
                    self.CreateSeparator(vbox)

            if self.CfgFile.HasEntry("Audio_MT32_Roms_Path"):
                self.CreateROMsDirectorySelector(vbox, "MT-32 Roms Path:", "Audio_MT32_Roms_Path", "Set path to directory containing Control and PCM roms from MT-32 or CM-32L (LAPC-I).\nThis is necessary when MIDI playback using MUNT emulator is selected.\nCM-32L (LAPC-I) roms are preferred.\nThe CM-32L (LAPC-I) roms filenames must be CM32L_CONTROL.ROM and CM32L_PCM.ROM.\nThe MT-32 roms filenames must be MT32_CONTROL.ROM and MT32_PCM.ROM.")
                num_extra_options -= 1
                if num_extra_options != 0:
                    self.CreateSeparator(vbox)

        if self.CfgFile.HasEntry("Display_Mode") or self.CfgFile.HasEntry("Tvout_Scaling") or self.CfgFile.HasEntry("Display") or self.CfgFile.HasEntry("Display_Fullscreen") or self.CfgFile.HasEntry("Display_MouseCursor") or self.CfgFile.HasEntry("Display_Enhanced_3D_Rendering"):
            vbox = self.AddPageFrameVBox(notebook, "Display", "Display")

            IsFirst = True
            if self.CfgFile.HasEntry("Display_Mode"):
                IsFirst = False
                description = "Select initial scaling mode."
                if game == "albion":
                    description += "\nOriginal game resolution is 360x240 displayed with aspect ratio 4:3.\ndouble_pixels = 720x480\nfullscreen = 800x480\ncorrect_aspect_ratio = 640x480"
                else:
                    description += "\nOriginal game resolution is 320x200 displayed with aspect ratio 4:3.\ndouble_pixels = 640x400\nfullscreen = 800x480\ncorrect_aspect_ratio = 640x480"

                self.CreateRadioSet(vbox, "Display Mode:", "Display_Mode", description)
            elif self.CfgFile.HasEntry("Tvout_Scaling"):
                IsFirst = False
                self.CreateRadioSet(vbox, "Tvout Scaling:", "Tvout_Scaling", "If the TV-output mode of the GP2X is being used, this setting allows to select\nwhether the image will be scaled down from a width of 360 to 320 or just left at 360.\nMost users will want to leave this on.")
            elif self.CfgFile.HasEntry("Display"):
                IsFirst = False
                description = "Select whether game is displayed in original resolution or stretched to fullscreen."
                if self.CfgFile.HasEntry("Touchscreen"):
                    description += "\nNote: Display is forced into stretched mode on F200s when using touchscreen."

                self.CreateRadioSet(vbox, "Display:", "Display", description)
            elif self.CfgFile.HasEntry("Display_Fullscreen"):
                IsFirst = False
                self.CreateRadioSet2(vbox, "Display Scaled Width:", "Display_ScaledWidth", ("640/720" if game == "albion" else "640") + "/960/1280/1600/1920/2240/2560/2880/3840", "Default value is " + ("720" if game == "albion" else "640") + ". Correct display aspect ratio is 4:3.")
                self.CreateSeparator(vbox)
                self.CreateRadioSet2(vbox, "Display Scaled Height:", "Display_ScaledHeight", ("480" if game == "albion" else "400/480") + "/720/960/1080/1200/1440/1680/1920/2160", "Default value is " + ("480" if game == "albion" else "400") + ". Correct display aspect ratio is 4:3.")
                self.CreateSeparator(vbox)
                self.CreateRadioSet(vbox, "Display Fullscreen:", "Display_Fullscreen", "Select whether game is displayed fullscreen or in a window.")

                if self.CfgFile.HasEntry("Display_MouseCursor"):
                    self.CreateSeparator(vbox)
                    self.CreateRadioSet(vbox, "Display MouseCursor:", "Display_MouseCursor", "Select mouse cursor type when the game is displayed in a window.")
                    if self.CfgFile.HasEntry("Display_Enhanced_3D_Rendering"):
                        vbox = self.AddPageFrameVBox(notebook, "Display 2", "Display")

                        IsFirst = True

            if self.CfgFile.HasEntry("Display_Enhanced_3D_Rendering"):
                if not IsFirst:
                    self.CreateSeparator(vbox)
                self.CreateRadioSet(vbox, "Enhanced 3D Rendering:", "Display_Enhanced_3D_Rendering", "Enhanced 3D rendering renders the 3d part of the game in the native resolution,\ninstead of rendering it in the original resolution and then scaling it.\n\nThere are some minor issues, read the readme for more information.")

        if self.CfgFile.HasEntry("Use_Alternative_SDL"):
            vbox = self.AddPageFrameVBox(notebook, "SDL", "SDL")

            self.CreateRadioSet(vbox, "Use Alternative SDL ?", "Use_Alternative_SDL", "Select whether to use alternative SDL library by notaz or the firmware SDL library.\nAlternative SDL library by notaz uses OMAP specific driver and\ncontains vsync code, but some input methods (nubs, mouse) might not work.")

        if self.CfgFile.HasEntry("Screenshot_Format"):
            vbox = self.AddPageFrameVBox(notebook, "Screenshots", "Screenshot settings")

            self.CreateRadioSet(vbox, "Screenshot Format", "Screenshot_Format", "Original = original resolution and image format (LBM)\nLBM = image format is LBM\nLBM_pad16 = image format is LBM with lines in file padded to 16 bytes\nTGA = image format is TGA\nBMP = image format is BMP")

        if self.CfgFile.HasEntry("Input_Mode") or self.CfgFile.HasEntry("Input_MouseHelper"):
            vbox = self.AddPageFrameVBox(notebook, "Input", "Input")

            dpad_input_mode = True
            if self.CfgFile.HasEntry("Input_Mode"):
                dpad_input_mode = self.CfgFile.GetEntryValue("Input_Mode").lower() == "keyboard_dpad"

                description = "Select input mode."
                if game == "albion":
                    description += "\ntouchscreen_dpad = right-handed touchscreen input\n     (dpad = cursor keys, L+touchscreen = right mouse button)\ntouchscreen_abxy = left-handed touchscreen input\n     (abxy = cursor keys, R+touchscreen = right mouse button)\nkeyboard_dpad = input without touchscreen\n     (dpad = mouse movement / cursor keys,\n       L = switch between mouse movement and cursor keys)"
                else:
                    description += "\ntouchscreen_dpad = right-handed touchscreen input\n     (dpad = cursor keys, L = shift)\ntouchscreen_abxy = left-handed touchscreen input\n     (abxy = cursor keys, R = shift)\nkeyboard_dpad = input without touchscreen\n     (dpad = mouse movement, L+dpad = cursor keys)"

                self.CreateRadioSet(vbox, "Input Mode:", "Input_Mode", description, True)

            mouse_helper = False
            if dpad_input_mode and self.CfgFile.HasEntry("Input_MouseHelper"):
                mouse_helper = True
                if self.CfgFile.HasEntry("Input_Mode"):
                    vbox = self.AddPageFrameVBox(notebook, "Input2", "Input2")

                description = "Select whether mouse helper is enabled."
                description += "\nFeatures:"
                description += "\n* Left click and drag to draw selection boxes around multiple units\n  without having to hold down the CTRL key."
                description += "\n* Right click to order units to move, attack or harvest/transport goods."
                description += "\n  (Can also be used to build walls when the town center is selected)"
                description += "\n* Middle click to order units to repair or stop."
                description += "\n  (Can also be used to build roads when the town center is selected)"
                self.CreateRadioSet(vbox, "Mouse Helper:", "Input_MouseHelper", description)

            if self.CfgFile.HasEntry("Input_SelectGroupTreshold"):
                self.CreateSeparator(vbox)

                self.CreateScale(vbox, "Select Group Treshold:", "Input_SelectGroupTreshold", "Select treshold (distance in pixels) in " + ("mouse" if mouse_helper else "touchscreen") + " input before\nleft mouse click changes to ctrl + left mouse click (select group).", dpad_input_mode and not mouse_helper)

            if self.CfgFile.HasEntry("Input_Old_Touchscreen_Mode"):
                self.CreateSeparator(vbox)

                self.CreateRadioSet(vbox, "Old Touchscreen Mode:", "Input_Old_Touchscreen_Mode", "Select whether to use old touchscreen mode.\nOld touchscreen mode emulates left mouse click when pressing the touchscreen\ninstead of when releasing the touchscreen.", dpad_input_mode)

        if self.CfgFile.HasEntry("Touchscreen"):
            vbox = self.AddPageFrameVBox(notebook, "Input", "Input")

            description = "Select whether touchscreen is used.\nHas no effect on F100 units."
            if self.CfgFile.HasEntry("Display"):
                description += "\nWhen Touchscreen=on, display is forced into stretched mode."

            self.CreateRadioSet(vbox, "Touchscreen:", "Touchscreen", description, True)

            if self.CfgFile.HasEntry("Stylus_Clicks"):
                self.CreateSeparator(vbox)

                self.CreateRadioSet(vbox, "Stylus Clicks:", "Stylus_Clicks", "When this is on, when the stylus is tapped and held, it will register that the left mouse button is pressed\nand when the button assigned to right mouse button is held while tapping and holding the stylus, it will\nregister that the right mouse button is pressed.\nWhen this is off, the stylus will only move the cursor around.", self.CfgFile.GetEntryValue("Touchscreen").lower() == "off")

            if self.CfgFile.HasEntry("Cursor_Buttons"):
                self.CreateSeparator(vbox)

                self.CreateRadioSet(vbox, "Cursor Buttons:", "Cursor_Buttons", "Select which buttons are used as cursor buttons.\n", True)

            if self.CfgFile.HasEntry("Scrollbuttons"):
                self.CreateSeparator(vbox)

                self.CreateRadioSet(vbox, "Scrollbuttons:", "Scrollbuttons", "Select method to scroll battlescape (and presumably also rotate geoscape).\nF100 uses L+Stick/DPAD (it's also for F200 users wishing to not use touchscreen to play game)\nF200_RH is for right-handed touchscreen users: DPAD\nF200_LH is for left-handed touchscreen users: A/B/X/Y as pseudo-dpad", True)

        if platform == "pandora" or platform == "gp2x":
            vbox = self.AddPageFrameVBox(notebook, "Buttons", "Buttons")
            self.CreateEntryLabel(vbox, "Button mappings:", 5)
            self.CreateSeparator(vbox)
            self.CreateEntryLabel(vbox, "Button mappings are currently not editable using this configuration tool.\nThey can be changed by editing the configuration file directly.", 20)

        # Create a bunch of buttons
        hbox = gtk.HBox(True, 0)
        mainvbox.pack_start(hbox, False, False, 0)
        hbox.show()

        button = gtk.Button("Save")
        button.connect("clicked", self.Save)
        hbox.pack_start(button, False, True, 0)
        button.show()

        button = gtk.Button("Restore values")
        button.connect("clicked", self.RestoreValues)
        hbox.pack_start(button, False, True, 0)
        button.show()

        button = gtk.Button("Load default values")
        button.connect("clicked", self.LoadDefaultValues)
        hbox.pack_start(button, False, True, 0)
        button.show()

        button = gtk.Button("Exit")
        button.connect("clicked", self.Exit)
        hbox.pack_start(button, False, True, 0)
        button.show()

        mainvbox.show()
        self.window.show()

    def Save(self, widget, data=None):
        message = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL, gtk.MESSAGE_QUESTION, gtk.BUTTONS_YES_NO, "Save configuration ?")
        message.format_secondary_text("Existing configuration will be overwritten.")
        message.set_title("Question")

        response = message.run()
        message.destroy()

        if response != gtk.RESPONSE_YES:
            return

        self.CfgFile.WriteConfigFile(self.file_path)

    def RestoreValues(self, widget, data=None):
        message = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL, gtk.MESSAGE_QUESTION, gtk.BUTTONS_YES_NO, "Restore values ?")
        message.format_secondary_text("Configuration entries will be restored to the last saved values.")
        message.set_title("Question")

        response = message.run()
        message.destroy()

        if response != gtk.RESPONSE_YES:
            return

        for entry_type, entry_name, widget in self.widgets:
            self.CfgFile.RestoreEntryFileValue(entry_name)

        self.DisplayValues()

    def LoadDefaultValues(self, widget, data=None):
        message = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL, gtk.MESSAGE_QUESTION, gtk.BUTTONS_YES_NO, "Load default values ?")
        message.format_secondary_text("Configuration entries will be set to their default values.")
        message.set_title("Question")

        response = message.run()
        message.destroy()

        if response != gtk.RESPONSE_YES:
            return

        for entry_type, entry_name, widget in self.widgets:
            self.CfgFile.RestoreEntryDefaultValue(entry_name)

        self.DisplayValues()

    def Exit(self, widget, data=None):
        if self.CfgFile.AreValuesChanged():
            message = gtk.MessageDialog(self.window, gtk.DIALOG_MODAL, gtk.MESSAGE_QUESTION, gtk.BUTTONS_YES_NO, "Exit without saving ?")
            message.format_secondary_text("Some configuration entries were changed.\nIf you exit now, the changed values won't be saved.")
            message.set_title("Question")

            response = message.run()
            message.destroy()

            if response != gtk.RESPONSE_YES:
                return

        self.delete(self.window)

    def AddPageFrameVBox(self, notebook, page_label, frame_label):
        frame = gtk.Frame(frame_label)
        frame.set_border_width(5)
        frame.show()

        label = gtk.Label(page_label)
        notebook.append_page(frame, label)

        vbox = gtk.VBox(False, 0)
        frame.add(vbox)
        vbox.show()

        return vbox

    def CreateEntryLabel(self, parentvbox, entry_label, padding):
        label = gtk.Label(entry_label)
        label.set_alignment(0, 0)
        label.set_padding(padding, 0)
        parentvbox.pack_start(label, False, False)
        label.show()

    def CreateSeparator(self, parentvbox):
        separator = gtk.HSeparator()
        parentvbox.pack_start(separator, False, False)
        separator.show()

    def CreateEntry(self, parentvbox, entry_label, entry_name, entry_description = None):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        entry = gtk.Entry(256)
        entry.set_text(self.CfgFile.GetEntryValue(entry_name))
        entry.connect("changed", self.EntryChanged, entry_name)
        vbox.pack_start(entry, False, False, 5)
        entry.show()

        self.widgets.append(("entry", entry_name, entry))

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def CreateSoundfontSelector(self, parentvbox, entry_label, entry_name, entry_description = None):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        hbox = gtk.HBox(False, 0)
        vbox.pack_start(hbox, False, False, 5)
        hbox.show()

        entry = gtk.Entry(256)
        entry.set_text(self.CfgFile.GetEntryValue(entry_name))
        entry.connect("changed", self.EntryChanged, entry_name)
        hbox.pack_start(entry, True, True, 5)
        entry.show()

        self.widgets.append(("entry", entry_name, entry))

        button = gtk.Button("Select SoundFont")
        button.connect("clicked", self.SelectSoundfont, entry)
        hbox.pack_start(button, False, False, 5)
        button.show()

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def CreateROMsDirectorySelector(self, parentvbox, entry_label, entry_name, entry_description = None):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        hbox = gtk.HBox(False, 0)
        vbox.pack_start(hbox, False, False, 5)
        hbox.show()

        entry = gtk.Entry(256)
        entry.set_text(self.CfgFile.GetEntryValue(entry_name))
        entry.connect("changed", self.EntryChanged, entry_name)
        hbox.pack_start(entry, True, True, 5)
        entry.show()

        self.widgets.append(("entry", entry_name, entry))

        button = gtk.Button("Select ROMs directory")
        button.connect("clicked", self.SelectROMsDirectory, entry)
        hbox.pack_start(button, False, False, 5)
        button.show()

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def CreateRadioSet(self, parentvbox, entry_label, entry_name, entry_description = None, ReadOnly = False):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        entry_format = self.CfgFile.GetEntryFormat(entry_name).split("/")
        current_value = self.CfgFile.GetEntryValue(entry_name).lower()
        group = None

        buttons = []

        for entry_value in entry_format:
            button = gtk.RadioButton(group, entry_value)
            button.set_use_underline(False)
            if current_value == entry_value.lower():
                button.set_active(True)
            if ReadOnly:
                button.set_sensitive(False)
            button.connect("toggled", self.RadioSetToggle, entry_name + ":" + entry_value)
            vbox.pack_start(button, False, False, 0)
            button.show()
            buttons.append(button)

            if group is None:
                group = button

        if not ReadOnly:
            self.widgets.append(("radioset", entry_name, buttons))

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def CreateRadioSet2(self, parentvbox, entry_label, entry_name, entry_values, entry_description = None):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        entry_format = entry_values.split("/")
        current_value = self.CfgFile.GetEntryValue(entry_name).lower()
        if not current_value in entry_format:
            entry_format.append(current_value)
        group = None

        hbox = gtk.HBox(False, 0)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        buttons = []

        for entry_value in entry_format:
            button = gtk.RadioButton(group, entry_value)
            button.set_use_underline(False)
            if current_value == entry_value.lower():
                button.set_active(True)
            button.connect("toggled", self.RadioSetToggle, entry_name + ":" + entry_value)
            hbox.pack_start(button, False, False, 0)
            button.show()
            buttons.append(button)

            if group is None:
                group = button

        self.widgets.append(("radioset", entry_name, buttons))

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def CreateScale(self, parentvbox, entry_label, entry_name, entry_description = None, ReadOnly = False):
        vbox = gtk.VBox(False, 0)
        self.CreateEntryLabel(vbox, entry_label, 5)

        hbox = gtk.HBox(False, 0)
        vbox.pack_start(hbox, False, False, 0)
        hbox.show()

        entry_format = self.CfgFile.GetEntryFormat(entry_name).partition("-")
        current_value = int(self.CfgFile.GetEntryValue(entry_name))
        adjustment = gtk.Adjustment(current_value, int(entry_format[0]), int(entry_format[2]), 1, 1)
        hscale = gtk.HScale(adjustment)
        hscale.set_digits(0)
        if ReadOnly:
            hscale.set_sensitive(False)
        hscale.connect("value-changed", self.ScaleChanged, entry_name)
        hbox.pack_start(hscale, True, True, 5)
        hscale.show()

        if not ReadOnly:
            self.widgets.append(("scale", entry_name, hscale))

        if not entry_description is None:
            self.CreateEntryLabel(vbox, entry_description, 20)

        parentvbox.pack_start(vbox, False, False, 5)
        vbox.show()

    def DisplayValues(self):
        for entry_type, entry_name, widget in self.widgets:
            current_value = self.CfgFile.GetEntryValue(entry_name)
            if entry_type == "entry":
                widget.set_text(current_value)
            elif entry_type == "scale":
                widget.set_value(int(current_value))
            elif entry_type == "radioset":
                for button in widget:
                    button.set_active(button.get_label() == current_value)

    def EntryChanged(self, widget, data=None):
        self.CfgFile.SetEntryValue(data, widget.get_text())

    def RadioSetToggle(self, widget, data=None):
        if widget.get_active():
            partition = data.partition(":")
            self.CfgFile.SetEntryValue(partition[0], partition[2])

    def ScaleChanged(self, widget, data=None):
        self.CfgFile.SetEntryValue(data, str(int(widget.get_value())))

    def SelectSoundfont(self, widget, entry=None):
        dialog = gtk.FileChooserDialog("Select SoundFont", self.window, gtk.FILE_CHOOSER_ACTION_OPEN, (gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL, gtk.STOCK_OK,gtk.RESPONSE_OK))

        dialog.set_local_only(True)

        filename = entry.get_text()
        if filename is None or filename == "":
            dialog.set_current_folder(os.getcwd())
        else:
            dialog.set_filename(filename)

        dialog_filter = gtk.FileFilter()
        dialog_filter.set_name("Soundfonts (*.sf2)")
        dialog_filter.add_pattern("*.sf2")
        dialog.add_filter(dialog_filter)

        dialog_filter = gtk.FileFilter()
        dialog_filter.set_name("All files (*)")
        dialog_filter.add_pattern("*")
        dialog.add_filter(dialog_filter)

        response = dialog.run()

        if response == gtk.RESPONSE_OK:
            entry.set_text(dialog.get_filename())

        dialog.destroy()

    def SelectROMsDirectory(self, widget, entry=None):
        dialog = gtk.FileChooserDialog("Select ROMs directory", self.window, gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER, (gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL, gtk.STOCK_OK,gtk.RESPONSE_OK))

        dialog.set_local_only(True)

        dirname = entry.get_text()
        if dirname is None or dirname == "":
            dialog.set_current_folder(os.getcwd())
        else:
            dialog.set_current_folder(dirname)

        dialog_filter = gtk.FileFilter()
        dialog_filter.set_name("ROMs (*.rom)")
        dialog_filter.add_pattern("*.ROM")
        dialog_filter.add_pattern("*.rom")
        dialog.add_filter(dialog_filter)

        dialog_filter = gtk.FileFilter()
        dialog_filter.set_name("All files (*)")
        dialog_filter.add_pattern("*")
        dialog.add_filter(dialog_filter)

        response = dialog.run()

        if response == gtk.RESPONSE_OK:
            entry.set_text(dialog.get_filename())

        dialog.destroy()

if len(sys.argv) >= 4:
    ConfigGUI(sys.argv[1], sys.argv[2], sys.argv[3])
    gtk.main()
else:
    print "Not enough parameters: game platform file_path"
    print "\tgame = albion / xcom1 / xcom2 / warcraft"
    print "\tplatform = pc / pandora / gp2x"

