#ifndef INCLUDED_player_fsm_GEN_H
#define INCLUDED_player_fsm_GEN_H


struct PlayerFsm
{
	enum Player__State {
		sitting,
		stand,
		standing_up,
		start_goto_left,
		start_goto_right,
		start_jump,
		stating_up,
	};

	unsigned State() { return _state; }

	void OnAnim_end();
	void OnLetKeys();
	void OnPush_down();
	void OnPush_left();
	void OnPush_right();
	void OnPush_up();
private:
	unsigned _state{ 0 };
};


#endif // INCLUDED_player_fsm_GEN_H
