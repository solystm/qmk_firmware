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
    _BAS,
    _FN1,
    _FN2,
    _VIM1,
};

/* Macro section
 * Defining macros here, to be used below in the keymap...
 */
enum{
	MC_VMES = SAFE_RANGE, // Escape key macro, esc on tap or VIM mode when held
	MC_VMESC, // Escape key to leave VIM mode
	MC_DLNE,
	MC_DWRD,
	/* VIM keybindings
	 * All the rest of the key bindings deal with VIM mode.
	 */
	V_SHFT, // VIM shift mode
	V_CTRL, // VIM control mode
	V_SPACE, // I think this is just right-arrow?
	VIM_A, // a: Enter insert after current character
	       // A (capital): Insert at end of line
	VIM_B, // b: Back a word
	       // C-b: Page up
	VIM_C, // c: Change command
	       // C: Change to end of line
	VIM_D, // d: Delete command
	       // D: Delete to end of line
	       // C-d: Page down
	VIM_E, // e: End of word
	       // C-e: Scroll text down... not implemented, not really supportable through key commands
	VIM_F, // f: Find character after cursor... may not implement this
	       // F: Find backwards... may not implement this
	       // C-f: Page down
	VIM_G, // g: unbound
	       // G: Go to line number... may not implement this, but theoretically i should be able to?
	       // gg: move to start of document
	       // GG: move to end of document
	       // C-g: Show status, cannot implement
	VIM_H, // h: Move left
	       // H: Move to top line of screen (maybe page up)
	       // C-h: Backspace
	VIM_I, // i: Insert before current character
	       // I: Insert at start of row
	       // C-i: Tab... not implemented as it only works in insert mode
	VIM_J, // j: Move down
	       // J: Join current and next lines
	       // C-j: Move down
	VIM_K, // k: Move up
	       // K: Unbound
	       // C-k: Unbound... may make it move up?
	VIM_L, // l: Move right
	       // L: Move to bottom line of screen (maybe page down)
	       // C-l: Refresh screen... no need to implement
	VIM_M, // m: Mark curent line... not sure if we're using this...
	       // M: Move to middle of screen... may not implement this
	       // C-m: Move to first non-whitespace of next line, CR in insert mode
	VIM_N, // n: Repeat last search
	       // N: Repeat last search but in opposite direction... may not implement
	       // C-n: Move down one line
	VIM_O, // o: Open line below and enter insert
	       // O: Open line above and enter insert
	VIM_P, // p: Paste
	       // P: Paste before cursor, not sure how to really implement this...
	       // C-p: Move up one line
	VIM_Q, // q: unbound
	       // Q: Leave visual mode and enter ex mode... may not implement
	       // C-q: XON
	VIM_R, // r: Replace single character
	       // R: Replace to end of line, then insert
	       // C-r: Redo
	VIM_S, // s: Substitute single character
	       // S: Substitute entire line
	       // C-s: XOFF
	// VIM_T, // t: Same as F but cursor moves to just before the found character... may not implement this
	       // T: Backwards t... may not implement this
	       // C-t: move back a tag jump... probably won't implement
	VIM_U, // u: Undo
	       // U: Restores line to state when cursor was moved into it... may not implement this
	       // C-u: Page up
	VIM_V, // v: Enter visual mode
	       // V: Enter visual mode and highlight whole line (Need to be careful so it doesn't get glitchy)
	       // C-v: Visual block mode... probably won't implement
	VIM_W, // w: Move forward one word
	VIM_X, // x: Delete a single character
	       // X: Delete single character backwards
	VIM_Y, // y: Yank command
	       // Y, yy: Yank entire line
	       // C-y: Scroll text up, not implemented, not really supportable through key commands
	// VIM_Z, // z: Position current line... may not implement
	       // ZZ: Quick save and exit... may not implement
	       // C-z: Suspend program... may not implement
	// VIM_1, VIM_2, VIM_3, VIM_4, VIM_5, VIM_6, VIM_7, VIM_8, VIM_9, VIM_0, // Precursor/multiplier commands... may not implement

};
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	static bool nasty_hacks = true; // Enable some more optimistic behavior
	static uint16_t vim_esc_keypress;
	static bool change_mode; // Used with the "c" key
	static bool del_mode; // Used with the "d" key
	static bool visual_mode; // While in visual mode
	static bool yank_mode; // Used with "y" key
	static bool vim_shift; // Shift key while in VIM mode
	static bool vim_control; // Control key while in VIM mode
	switch( keycode ){
		case MC_DLNE: // Delete an entire line
			if( record->event.pressed ){
				SEND_STRING( SS_TAP( X_HOME )SS_DOWN( X_LSFT )SS_TAP( X_END )SS_UP( X_LSFT )SS_TAP( X_DEL ));
			}
			return false;
		case MC_VMES: // Escape when tapped, enter VIM mode when held
			if( record->event.pressed ){ // Press
				register_code( KC_ESC );
				vim_esc_keypress = timer_read();
			}else{ // Release
				unregister_code( KC_ESC );
				if( timer_elapsed( vim_esc_keypress )> TAPPING_TERM ){
					clear_keyboard(); // Clear all keys from being pressed
					layer_clear();
					layer_on( _VIM1 );
					if( nasty_hacks ){ // Try to cancel selections
						SEND_STRING( SS_TAP( X_LEFT )SS_TAP( X_RGHT ));
					}
				}
			}
			return false; // We handled this keypress
		case MC_VMESC: // Escape from inside VIM mode
			if( record->event.pressed ){
				if( visual_mode ){
					visual_mode = false;
					if( nasty_hacks ){ // Try to cancel the selection
						SEND_STRING( SS_TAP( X_LEFT )SS_TAP( X_RGHT ));
					}
				}else if( change_mode || del_mode || yank_mode ){
					// Just get rid of all these.
					change_mode = false;
					del_mode = false;
					yank_mode = false;
				}else{
					// Nothing else going on, go back to normal mode.
					layer_clear();
					layer_on( _BAS );
				}
				clear_keyboard();
			}
			return false;
		case MC_DWRD: // Delete word (emulating VIM C-w)
			if( record->event.pressed ){
				SEND_STRING( SS_LSFT( SS_TAP( X_RGHT ))SS_TAP( X_DEL ));
			}
			return false;
			/* VIM MODE
			 * Below this is the VIM input handling code.
			 * It's kinda taking over some of the keyboard input/layer handling, so it's a little complicated.
			 * I'd like to refactor this to use functions and stuff but uh... not sure how best to do that in the QMK framework.
			 */
		case V_SHFT: // Shift key pressed...
			if( record->event.pressed ){
				vim_shift = true;
			}else{
				vim_shift = false;
			}
			return false;
		case V_CTRL: // Shift key pressed...
			if( record->event.pressed ){
				vim_control = true;
			}else{
				vim_control = false;
			}
			return false;
		case VIM_A:
			if( record->event.pressed ){
				if( vim_shift ){
					// A: Insert at end of line
					SEND_STRING( SS_TAP( X_END ));
					layer_clear();
					layer_on( _BAS );
				}else{
					// a: Insert after character
					SEND_STRING( SS_TAP( X_RGHT ));
					layer_clear();
					layer_on( _BAS );
				}
			}
			return false;
		case VIM_B:
			if( record->event.pressed ){
				if( vim_control ){
					// Page up
					register_code( KC_PGUP );
				}else{
					if( del_mode ){
						SEND_STRING( SS_LSFT( SS_TAP( X_LEFT ))SS_TAP( X_DEL ));
						del_mode = false;
					}else if( change_mode ){
						SEND_STRING( SS_LSFT( SS_TAP( X_LEFT ))SS_TAP( X_DEL ));
						layer_clear();
						layer_on( _BAS );
						change_mode = false;
					}else if( visual_mode ){
						register_code( KC_LCTL );
						register_code( KC_LSFT );
						register_code( KC_LEFT );
					}else{
						// b: Back a word
						register_code( KC_LCTL );
						register_code( KC_LEFT );
					}
				}
			}else{
				clear_keyboard();
			}
			return false;
		case VIM_C:
			if( record->event.pressed ){
				if( vim_shift ){
					// C: Change to end of line
					SEND_STRING( SS_LSFT( SS_TAP( X_END ))SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
				}else if( visual_mode ){
					// V-c: Delete and enter insert
					SEND_STRING( SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
					visual_mode = false;
				}else{
					// c: Enter change command
					change_mode = true;
				}
			}
			return false;
		case VIM_D:
			if( record->event.pressed ){
				if( vim_control ){
					// C-d: Page down
					register_code( KC_PGDN );
				}else if( vim_shift ){
					// D: Delete backwards (backspace)
					register_code( KC_BSPC );
				}else if( visual_mode) {
					SEND_STRING( SS_TAP( X_DEL ));
					visual_mode = false;
				}else{
					// d: Enter delete mode
					if( del_mode ){
						// Blow up this line
						SEND_STRING( SS_TAP( X_HOME )SS_LSFT( SS_TAP( X_END ))SS_TAP( X_DEL ));
						del_mode = false;
					}else{
						del_mode = true;
					}
				}
			}else{
				clear_keyboard();
				// unregister_code( KC_PGDN );
				// unregister_code( KC_BSPC );
			}
		case VIM_E:
			if( record->event.pressed ){
				if( vim_control ){
					// C-e: Scroll down... not implemented
				}else{
					if( del_mode ){
						SEND_STRING( SS_LSFT( SS_TAP( X_RGHT ))SS_TAP( X_DEL ));
						del_mode = false;
					}else if( change_mode ){
						SEND_STRING( SS_LSFT( SS_TAP( X_RGHT ))SS_TAP( X_DEL ));
						layer_clear();
						layer_on( _BAS );
						change_mode = false;
					}else{
						if( visual_mode ){
							register_code( KC_LSFT );
						}
						// e: Forward a word
						register_code( KC_LCTL );
						register_code( KC_RGHT );
					}
				}
			}else{
				clear_keyboard();
			}
			return false;
		case VIM_F:
			// f: Find character after cursor... may not implement this
			// F: Find backwards... may not implement this
			if( record->event.pressed ){
				if( vim_control ){
					// C-f: Page down
					register_code( KC_PGDN );
				}
			}else{
				clear_keyboard();
			}

			return false;
		case VIM_G:
			// g: unbound
			// G: Go to line number... may not implement this, but theoretically i should be able to?
			// move to start of document
			// GG: move to end of document
			// C-g: Show status, cannot implement

			return false;
		case VIM_H:
			if( record->event.pressed ){
				if( change_mode ){
					// Delete one character to the left, then insert 
					SEND_STRING( SS_TAP( X_BSPC ));
					layer_clear();
					layer_on( _BAS );
					change_mode = false;
				}else if( del_mode ){
					// Delete one character to the left
					SEND_STRING( SS_TAP( X_BSPC ));
					del_mode = false;
				}else{
					if( visual_mode ){
						register_code( KC_LSFT );
					}
					register_code( KC_LEFT );
				}
			}else{
				clear_keyboard();
				//unregister_code( KC_LEFT );
			}
			return false;
		case VIM_I:
			if( record->event.pressed ){
				if( vim_shift ){
					// I: Insert at start of line
					SEND_STRING( SS_TAP( X_HOME ));
					layer_clear();
					layer_on( _BAS );
				}else{
					// i: Insert at character
					layer_clear();
					layer_on( _BAS );
				}
			}
			return false;
		case VIM_J:
			if( record->event.pressed ){
				if( change_mode ){
					// Delete current line and the next, then insert 
					SEND_STRING( SS_TAP( X_HOME )SS_LSFT( SS_TAP( X_DOWN )SS_TAP( X_END ))SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
					change_mode = false;
				}else if( del_mode ){
					// Delete current line and the next
					SEND_STRING( SS_TAP( X_HOME )SS_LSFT( SS_TAP( X_DOWN )SS_TAP( X_END ))SS_TAP( X_DEL ));
					del_mode = false;
				}else{
					if( visual_mode ){
						register_code( KC_LSFT );
					}
					register_code( KC_DOWN );
				}
			}else{
				clear_keyboard();
				//unregister_code( KC_DOWN );
			}
			return false;
		case VIM_K:
			if( record->event.pressed ){
				if( change_mode ){
					// Delete current line and the previous, then insert
					SEND_STRING( SS_TAP( X_END )SS_LSFT( SS_TAP( X_UP )SS_TAP( X_HOME ))SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
					change_mode = false;
				}else if( del_mode ){
					// Delete current line and the previous
					SEND_STRING( SS_TAP( X_END )SS_LSFT( SS_TAP( X_UP )SS_TAP( X_HOME ))SS_TAP( X_DEL ));
					del_mode = false;
				}else{
					if( visual_mode ){
						register_code( KC_LSFT );
					}
					register_code( KC_UP );
				}
			}else{
				clear_keyboard();
				//unregister_code( KC_UP );
			}
			return false;
		case VIM_L:
			if( record->event.pressed ){
				if( change_mode ){
					// Delete one character to the right, then insert 
					SEND_STRING( SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
					change_mode = false;
				}else if( del_mode ){
					// Delete one character to the right
					SEND_STRING( SS_TAP( X_DEL ));
					del_mode = false;
				}else{
					if( visual_mode ){
						register_code( KC_LSFT );
					}
					register_code( KC_RGHT );
				}
			}else{
				clear_keyboard();
				//unregister_code( KC_RGHT );
			}
			return false;
		case VIM_M:
			if( record->event.pressed ){
				// m: Mark curent line... Unimplemented
				// M: Move to middle of screen... Unimplemented
				if( vim_control ){
					// C-m: Move to first non-whitespace of next line, CR in insert mode
					SEND_STRING( SS_TAP( X_DOWN )SS_TAP( X_HOME ));
				}
			}
			return false;
		case VIM_N:
			return false;
		case VIM_O:
			return false;
		case VIM_P:
			return false;
		case VIM_Q:
			return false;
		case VIM_R:
			if( record->event.pressed ){
				if( vim_control ){
					// C-r: Redo... or at least make an attempt at it
					SEND_STRING( SS_LCTL( SS_TAP( X_Y )));
				}else if( vim_shift ){
					// R: Replace mode... not sure this is getting implemented.
				}else{
					// r: Replace single character... not sure this is getting implemented, though.
				}
			}
			return false;
		case VIM_S:
			if( record->event.pressed ){
				if( vim_control ){
					// C-s: XOFF... whatever this is, probably not getting implemented
				}else if( vim_shift ){
					// S: Substitute entire line
					SEND_STRING( SS_TAP( X_HOME )SS_DOWN( X_LSFT )SS_TAP( X_END )SS_UP( X_LSFT )SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
				}else{
					// s: Substitute single character
					SEND_STRING( SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
				}
			}
			return false;
		/* Unimplemented
		case VIM_T:
			return false;
		*/
		case VIM_U:
			if( record->event.pressed ){
				if( vim_control ){
					// C-u: page up
					SEND_STRING( SS_TAP( X_PGUP ));
				}else{
				// u: Undo
				SEND_STRING( SS_LCTL( SS_TAP( X_Z )));
				}
			}
			return false;
		case VIM_V:
			if( record->event.pressed ){
				// Toggle visual mode
				// The behavior is a little different (particularly the V behavior) because we can't really copy VIM 1:1 here
				if( visual_mode ){
					if( vim_shift ){
						// This behavior 
					}else{
						// Would be great to deselect things in here somehow... gonna try a nasty hack
						if( nasty_hacks ){
							SEND_STRING( SS_TAP( X_LEFT )SS_TAP( X_RGHT ));
						}
						visual_mode = false;
					}
				}else{
					if( vim_shift ){
						// Select whole line
						SEND_STRING( SS_TAP( X_HOME )SS_LSFT( SS_TAP( X_END )));
					}
					visual_mode = true;
				}
			}
			return false;
		case VIM_W:
			if( record->event.pressed ){
				if( del_mode ){
					// D-w: delete word
					SEND_STRING( SS_LSFT( SS_TAP( X_RGHT ))SS_TAP( X_DEL ));
					del_mode = false;
				}else if( change_mode ){
					// Ch-W: Delete word and insers
					SEND_STRING( SS_LSFT( SS_TAP( X_RGHT ))SS_TAP( X_DEL ));
					layer_clear();
					layer_on( _BAS );
					change_mode = false;
				}else{
					// w: Forward a word
					register_code( KC_LCTL );
					register_code( KC_RGHT );
				}
			}else{
				clear_keyboard();
			}
			return false;
		case VIM_X:
			if( record->event.pressed ){
				if( vim_shift ){
					// X: Delete previous character
					register_code( KC_BSPC );
				}else{
					// x: Delete single character
					// Same for visual mode
					register_code( KC_DEL );
				}
			}else{
				clear_keyboard();
			}
			return false;
		case VIM_Y:
			if( record->event.pressed ){
				if( yank_mode || vim_shift ){
					// Y, yy: Yank entire line
					yank_mode = false;
				}else{
					yank_mode = true;
					// y: Yank command
				}
			}else{
			}
				// C-y: Scroll text up, not implemented, not really supportable through key commands
			return false;
		/* Unimplemented
		case VIM_Z:
			return false;
		*/
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
  [_BAS] = LAYOUT_eighty_m80h(
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
    _______, _______, MC_DWRD,   _______, _______, _______, _______, MC_DLNE, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
    KC_CAPS, _______, LSA(KC_Y), _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______, _______,
    MO(_FN2),_______, _______,   _______, _______, _______, _______, KC_ENT,  KC_HOME, KC_END,  _______, _______,                                       _______,
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
    MC_VMESC,XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            KC_PSCR, KC_SLCK, KC_PAUS,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     KC_INS,  KC_HOME, KC_PGUP,
    XXXXXXX, VIM_Q,   VIM_W,   VIM_E,   VIM_R,   XXXXXXX, VIM_Y,   VIM_U,   VIM_I,   VIM_O,   VIM_P,   XXXXXXX, XXXXXXX, XXXXXXX,            KC_DEL,  KC_END,  KC_PGDN,
    V_CTRL,  VIM_A,   VIM_S,   VIM_D,   VIM_F,   VIM_G,   VIM_H,   VIM_J,   VIM_K,   VIM_L,   XXXXXXX, XXXXXXX, XXXXXXX,
    V_SHFT,  XXXXXXX, VIM_X,   VIM_C,   VIM_V,   VIM_B,   VIM_N,   VIM_M,   XXXXXXX, XXXXXXX, XXXXXXX,          V_SHFT,                               KC_UP,
    XXXXXXX, XXXXXXX, XXXXXXX,                   V_SPACE,                                     XXXXXXX, XXXXXXX, XXXXXXX, V_CTRL,             KC_LEFT, KC_DOWN, KC_RGHT),
};
