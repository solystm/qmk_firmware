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

/* https://youtu.be/x6pWqjC39_I
 * This is the video I'm working off
 * See that for how things should be running
 */

#include QMK_KEYBOARD_H
#include "colors.h"

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

/*
const rgblight_segment_t PROGMEM rgb_layer_base[] = RGBLIGHT_LAYER_SEGMENTS({ 9, 20, HSV_CYAN });
const rgblight_segment_t PROGMEM rgb_layer_fn1[] = RGBLIGHT_LAYER_SEGMENTS({ 9, 20, HSV_PURPLE });
const rgblight_segment_t PROGMEM rgb_layer_fn2[] = RGBLIGHT_LAYER_SEGMENTS({ 9, 20, HSV_GREEN });

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
	rgb_layer_base,
	rgb_layer_fn1,
	rgb_layer_fn2
);

void keyboard_post_init_user( void ){
	rgb_matrix_enable();
	rgblight_layers = my_rgb_layers;
}
*/

// Light LEDs 6 to 9 and 12 to 15 red when caps lock is active. Hard to ignore!
const rgblight_segment_t PROGMEM my_capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {6, 4, HSV_RED},       // Light 4 LEDs, starting with LED 6
    {12, 4, HSV_RED}       // Light 4 LEDs, starting with LED 12
);
// Light LEDs 9 & 10 in cyan when keyboard layer 1 is active
const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {9, 2, HSV_CYAN}
);
// Light LEDs 11 & 12 in purple when keyboard layer 2 is active
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {11, 2, HSV_PURPLE}
);
// Light LEDs 13 & 14 in green when keyboard layer 3 is active
const rgblight_segment_t PROGMEM my_layer3_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {13, 2, HSV_GREEN}
);

// Now define the array of layers. Later layers take precedence
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_capslock_layer,
    my_layer1_layer,    // Overrides caps lock layer
    my_layer2_layer,    // Overrides other layers
    my_layer3_layer     // Overrides other layers
);

void keyboard_post_init_user(void) {
    // Enable the LED layers
    rgblight_layers = my_rgb_layers;
}

const uint8_t PROGMEM ledmap[][DRIVER_LED_TOTAL][3] = {
	// RGB layout stub
};
void set_layer_color( int layer ){
	// What to do when animations are disabled
}

void matrix_scan_user( void ){
	// Runs constantly...
}
/*
void rgb_matrix_indicators_user( void ){
	// Set colors for layer
	if( g_suspend_state || keyboard_config.disable_layer_led ){
		return;
	}
	switch( biton32( layer_state )){
		case _BASE:
			set_layer_color( color_base );
			break;
		case _FN1:
			set_layer_color( color_fn1 );
			break;
		case _FN2:
			set_layer_color( color_fn2 );
			break;
		default:
			if( rgb_matrix_get_flags() == LED_FLAG_NONE ){
				rgb_matrix_set_color_all( 0,0,0 );
			}
			break;
	}
}
*/
