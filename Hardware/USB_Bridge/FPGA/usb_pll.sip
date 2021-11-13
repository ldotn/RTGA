set_global_assignment -entity "usb_pll" -library "lib_usb_pll" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "usb_pll" -library "lib_usb_pll" -name IP_TOOL_VERSION "19.1"
set_global_assignment -entity "usb_pll" -library "lib_usb_pll" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_usb_pll" -name SPD_FILE [file join $::quartus(sip_path) "usb_pll.spd"]

set_global_assignment -library "lib_usb_pll" -name MISC_FILE [file join $::quartus(sip_path) "usb_pll_sim/usb_pll.vo"]
