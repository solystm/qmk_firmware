/* Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

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
enum layer_names {
    _BASE,
    _FN1,
    _FN2,
    _VIM1,
    _VIM2,
    _VIS1
};

/* Macro section
 * Defining macros here, to be used below in the keymap...
 */
enum{
	MC_TEST = SAFE_RANGE, // Test macro!
	MC_DLNE, // Delete line
	// In VIM mode:
	MC_WORD, // Move to the next word (Currently not really this behavior, it's more like "end" below)
	MC_WEND, // Move to the end of the word
	MC_BACK, // Move back a word
	MC_DLIN, // Delete, then insert
	MC_VMES, // Escape key macro, esc on tap or VIM mode when held
};
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	// Note: this layout is slightly less space efficient (or so it seems) than putting the "if( record-> event.pressed )" up front.
	// However, when handling macros processes that need to interact with both on key press and on key release, it's easier to understand like this.
	static uint16_t vim_esc_keypress;
	switch( keycode ){
		case MC_DLNE: // Delete an entire line
			if( record->event.pressed ){
				SEND_STRING( SS_TAP( X_HOME )SS_DOWN( X_LSFT )SS_TAP( X_END )SS_UP( X_LSFT )SS_TAP( X_DEL ));
			}
			return false;
		case MC_WORD: // Forward a word (not matching VIM behavior atm)
			if( record->event.pressed ){
				SEND_STRING( SS_DOWN( X_LCTL )SS_TAP( X_RGHT )SS_UP( X_LCTL ));
			}
			return false;
		case MC_WEND: // Forward a word
			if( record->event.pressed ){
				SEND_STRING( SS_DOWN( X_LCTL )SS_TAP( X_RGHT )SS_UP( X_LCTL ));
			}
			return false;
		case MC_BACK: // Back a word
			if( record->event.pressed ){
				SEND_STRING( SS_DOWN( X_LCTL )SS_TAP( X_LEFT )SS_UP( X_LCTL ));
			}
			return false;
		case MC_DLIN: // Delete and return to insert mode
			if( record->event.pressed ){
				SEND_STRING( SS_TAP( X_DEL ));
				layer_off( _VIM1 );layer_off( _VIM2 );layer_off( _VIS1 );
				layer_on( _BASE );
			}
			return false;
		case MC_VMES: // Escape when tapped, enter VIM mode when held
			if( record->event.pressed ){ // Press
				vim_esc_keypress = timer_read();
				register_code( KC_ESC );
			}else{ // Release
				unregister_code( KC_ESC );
				if( timer_elapsed( vim_esc_keypress )> TAPPING_TERM ){
					layer_off( _BASE );layer_off( _FN1 );layer_off( _FN2 );
					layer_on( _VIM1 );
				}
			}
			return false; // We handled this keypress
		default:
			return true; // Process all other keycodes normally
	}
};

/* LAYER NOTES
 * Probably need an additional mod layer for VIM mode, control-mod. Don't think we need control-shift-mod?
 * Still need to figure out how much memory is in this thing.
 * 
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base layer, controls normal functions
 * Notes: Basically a standard layout, but with some additional mode swaps: escape sends to "VIM mode", left control sends to FN1, and caps is control.
 * LT( _VIM1, KC_ESC) -- tap for escape, but hold to go into VIM mode. Should make that easier to get into.
 */
  [_BASE] = LAYOUT_eighty_m80h(
    MC_VMES, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,   KC_F11,     KC_F12,     KC_BSPC,    KC_MUTE, KC_VOLD, KC_VOLU,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,     KC_MINS,    KC_EQL,                 KC_INS,  KC_HOME, KC_PGUP,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,     KC_LBRC,    KC_RBRC,    KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,  KC_QUOT,    KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,              KC_RSFT,                         KC_UP,
    MO(_FN1),KC_LGUI, KC_LALT,                   KC_SPC,                                      KC_RALT,  KC_RGUI,    MO(_FN1),   KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT),

/* Function layer one, basic functions
 * We have our Escape key in here instead of reset.
 */
  [_FN1] = LAYOUT_eighty_m80h(
    KC_ESC,  _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            KC_PSCR, KC_SLCK, KC_PAUS,
    _______, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                     _______, _______, _______,
    _______, _______, _______,   _______, _______, _______, _______, MC_DLNE, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    KC_CAPS, _______, LSA(KC_Y), _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______, _______,
    MO(_FN2),_______, _______,   _______, _______, _______, KC_HOME, KC_END,  _______, _______, _______, _______,                                       _______,
    _______, _______, _______,                     _______,                   _______, _______, _______, _______,                              _______, _______, _______),

/* Function layer two, rarer functions
 * Reset key is here instead of escape/mode swap.
 */
  [_FN2] = LAYOUT_eighty_m80h(
    RESET,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                     _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                                       _______,
    _______, _______, _______,                   _______,                   _______, _______, _______, _______,                              _______, _______, _______),

/* VIM layer 1, base VIM keys
 * Base level VIM mode, all the usual goodness
 */
  [_VIM1] = LAYOUT_eighty_m80h(
    TO(_BASE),XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, KC_COPY, XXXXXXX, XXXXXXX, XXXXXXX, KC_PSTE, XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX, MC_WEND,  MC_WEND, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX, MC_DLIN, XXXXXXX, XXXXXXX,  XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX, XXXXXXX,
    MO(_VIM2),XXXXXXX, KC_DEL,  XXXXXXX, TO(_VIS1),MC_BACK, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                       XXXXXXX,
    XXXXXXX,  XXXXXXX, XXXXXXX,                    XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                              XXXXXXX, XXXXXXX, XXXXXXX),

/* VIM layer 2, shift-mod
 * VIM mode, but for the capital letter inputs
 */
  [_VIM2] = LAYOUT_eighty_m80h(
    TO(_BASE),_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                     _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    MO(_VIM2),_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                                       _______,
    _______,  _______, _______,                   _______,                   _______, _______, _______, _______,                              _______, _______, _______),

/* Visual layer 1, visual mode
 * VIM visual mode, select mode.
 */
  [_VIS1] = LAYOUT_eighty_m80h(
    TO(_VIM1),_______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______,  _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______,                     _______, _______, _______,
    _______,  _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    _______,  _______, _______, KC_CUT,  _______,  _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, KC_CUT,  _______, TO(_VIM1),_______, _______, _______, _______, _______, _______, _______,                                       _______,
    _______,  _______, _______,                    _______,                   _______, _______, _______, _______,                              _______, _______, _______),

};
