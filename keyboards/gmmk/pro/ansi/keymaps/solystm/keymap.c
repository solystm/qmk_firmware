/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

enum layer_names{
    _BASE,
    _FN1,
    _FN2,
    _VIM1,
};

enum{  // Macro names...
	MC_VMES = SAFE_RANGE, // Escape key macro, esc on tap or VIM mode when held
	MC_VMESC, // Escape key to leave VIM mode
	MC_DLNE,
	MC_DWRD,
	MC_LSFT, // Left shift handling
	MC_LCTL, // Left control handling
	/* VIM keybindings
	 * All the rest of the key bindings deal with VIM mode.
	 * (Ignoring those for now)
	 */
};

bool process_record_user( uint16_t keycode, keyrecord_t *record ){
	// static bool nasty_hacks = true; // Enable optimistic behavior
	// static bool custom_behavior = true; // Enable non-standard VIM behavior
	static bool shift_pressed; // Left shift tracking
	static bool control_pressed; // Left control tracking
	switch( keycode ){
		case MC_LSFT:
			if( record->event.pressed ){
				shift_pressed = true;
				if( control_pressed ){
					layer_off( _FN1 );
					layer_on( _FN2 );
				}else{
					register_code( KC_LSFT );
				}
			}else{
				shift_pressed = false;
				unregister_code( KC_LSFT );
				layer_off( _FN2 );
				if( control_pressed ){
					layer_on( _FN1 );
				}
			}
			return false; // We handled this input, do not process further
		case MC_LCTL:
			if( record->event.pressed ){
				control_pressed = true;
				if( shift_pressed ){
					unregister_code( KC_LSFT );
					layer_on( _FN2 );
					layer_off( _FN1 );
				}else{
					layer_on( _FN1 );
				}
			}else{
				if( shift_pressed ){
					register_code( KC_LSFT );
				}
				control_pressed = false;
				layer_off( _FN1 );
				layer_off( _FN2 );
			}
			return false;
		default:
			return true; // We did not handle this input, continue processing normally
	}
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// Default keymap:
//      ESC      F1       F2       F3       F4       F5       F6       F7       F8       F9       F10      F11      F12	     Prt           Rotary(Mute)
//      ~        1        2        3        4        5        6        7        8        9        0         -       (=)	     BackSpc           Del
//      Tab      Q        W        E        R        T        Y        U        I        O        P        [        ]        \                 PgUp
//      Caps     A        S        D        F        G        H        J        K        L        ;        "                 Enter             PgDn
//      Sh_L              Z        X        C        V        B        N        M        ,        .        ?                 Sh_R     Up       End
//      Ct_L     Win_L    Alt_L                               SPACE                               Alt_R    FN       Ct_R     Left     Down     Right


    // The FN key by default maps to a momentary toggle to layer 1 to provide access to the RESET key (to put the board into bootloader mode). Without
    // this mapping, you have to open the case to hit the button on the bottom of the PCB (near the USB cable attachment) while plugging in the USB
    // cable to get the board into bootloader mode - definitely not fun when you're working on your QMK builds. Remove this and put it back to KC_RGUI
    // if that's your preference.
    //
    // To put the keyboard in bootloader mode, use FN+backslash. If you accidentally put it into bootloader, you can just unplug the USB cable and
    // it'll be back to normal when you plug it back in.
    [_BASE] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,           KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        MC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        MC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_FN1] = LAYOUT(
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR,          _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RESET,            _______,
        _______, _______, LSA(KC_Y),_______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                             _______,                            _______, _______, _______, _______, _______, _______
    ),
    [_FN2] = LAYOUT(
        RESET,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RESET,            _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),


};


bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
      tap_code(KC_VOLU);
    } else {
      tap_code(KC_VOLD);
    }
    return true;
}
