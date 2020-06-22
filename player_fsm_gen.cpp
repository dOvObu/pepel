#include "player_fsm_gen.h"


void PlayerFsm::OnAnim_end() {
	switch(_state) {
	case PlayerFsm::Player__State::stating_up: _state = PlayerFsm::Player__State::stand; break;
	default: break;
	}
}
void PlayerFsm::OnLetKeys() {
	switch(_state) {
	case PlayerFsm::Player__State::sitting: _state = PlayerFsm::Player__State::standing_up; break;
	default: break;
	}
}
void PlayerFsm::OnPush_down() {
	switch(_state) {
	case PlayerFsm::Player__State::sitting: _state = PlayerFsm::Player__State::sitting; break;
	case PlayerFsm::Player__State::stand: _state = PlayerFsm::Player__State::sitting; break;
	default: break;
	}
}
void PlayerFsm::OnPush_left() {
	switch(_state) {
	case PlayerFsm::Player__State::stand: _state = PlayerFsm::Player__State::start_goto_left; break;
	case PlayerFsm::Player__State::stating_up: _state = PlayerFsm::Player__State::start_goto_left; break;
	default: break;
	}
}
void PlayerFsm::OnPush_right() {
	switch(_state) {
	case PlayerFsm::Player__State::stand: _state = PlayerFsm::Player__State::start_goto_right; break;
	case PlayerFsm::Player__State::stating_up: _state = PlayerFsm::Player__State::start_goto_right; break;
	default: break;
	}
}
void PlayerFsm::OnPush_up() {
	switch(_state) {
	case PlayerFsm::Player__State::stand: _state = PlayerFsm::Player__State::start_jump; break;
	default: break;
	}
}
