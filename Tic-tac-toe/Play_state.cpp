/*
 * Tic-tac-toe
 * MIT Licence, Copyright 2017 Chris Kempson (chriskempson.com)
 */

#include "Play_state.h"
#include "Game.h"
#include "Setting.h"
#include "State_manager.h"
#include "Resource_manager.h"
#include "Title_state.h"
#include "Theme.h"
#include <iostream>

void Play_state::init()
{
	//Checks the theme and will load the corresponding set of sprites to the resource
	// manager
	if(GetTheme() == 1)
	{
		Resource_manager::load_image("cell_sprite");;
	}
	else if(GetTheme() == 2)
	{
		Resource_manager::load_image("cell_spriteH");;
	}
	else
	{
		Resource_manager::load_image("cell_spriteA");
	}

	Resource_manager::load_image("red_wins");
	Resource_manager::load_image("yellow_wins");
	Resource_manager::load_image("draw");

	Resource_manager::load_sound("valid_move");
	Resource_manager::load_sound("invalid_move");
	Resource_manager::load_sound("win");
	Resource_manager::load_sound("draw");

	grid.init();

}

void Play_state::handle_events(SDL_Event& event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		advance_game();
	}
	
	// Using the SDL library to detect a keypress and the type of key pressed.
	// When the key "M" is pressed, there is a check to see if music is playing
	// if music is playing, unload sound, if music is not playing, load sound.
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_m)
		if(bgm_mute) {
			Resource_manager::load_sound("Spy");
			Resource_manager::get_sound("Spy")->play();
			bgm_mute = 0;
		}
		else {
			Resource_manager::unload_sound("Spy");
			bgm_mute = 1;
		}
	}
}

void Play_state::update()
{
	// Not much updating to do in this kind of game :P
}

void Play_state::render()
{
	grid.render();

	// Depending on the win or draw conditions, display a win or draw message
	// overlay image
	if (win_type == grid.player_red) {
		Resource_manager::get_image("red_wins")->render();
	}
	else if (win_type == grid.player_yellow) {
		Resource_manager::get_image("yellow_wins")->render();
	}
	else if (win_type == grid.player_red + grid.player_yellow) {
		Resource_manager::get_image("draw")->render();
	}
}

void Play_state::clean_up()
{
	//Pulls the theme and will clean up the set of sprites that are loaded for the current theme
	if(GetTheme() == 1)
	{
		Resource_manager::unload_image("cell_sprite");;
	}
	else if(GetTheme() == 2)
	{
		Resource_manager::unload_image("cell_spriteH");;
	}
	else
	{
		Resource_manager::unload_image("cell_spriteA");
	}
	Resource_manager::unload_image("red_wins");
	Resource_manager::unload_image("yellow_wins");
	Resource_manager::unload_image("draw");

	Resource_manager::unload_sound("valid_move");
	Resource_manager::unload_sound("invalid_move");
	Resource_manager::unload_sound("win");
	Resource_manager::unload_sound("draw");
}

void Play_state::advance_game() {

	// If we have no winner
	if (!win_type) {

		// Get mouse coordinates and normalise to row and columns so we know in
		// which column and row we have clicked
		int x, y;
		SDL_GetMouseState(&x, &y);
		int col = x / ((Setting::actual_window_width + Setting::scale_factor) / Setting::grid_columns);
		int row = y / ((Setting::actual_window_height + Setting::scale_factor) / Setting::grid_rows);

		// Try to play a move...
		if (play_a_move(col, row)) {

			// See if there was a win or a draw
			check_for_win_or_draw(col, row);
		}
		// Otherwise play a sound for an invalid move
		else {
			Resource_manager::get_sound("invalid_move")->play();
		}

	}

	// Else if there was a win or draw, reset the winner, make next sprite
	// red and clear the grid
	else {
		win_type = false;
		sprite_to_play = grid.sprite_red;
		grid.clear();
		State_manager::set_state(new Title_state());
	}
}

bool Play_state::play_a_move(int col, int row) {

	// If a grid area is empty
	if (grid.cell[col][row].current_sprite == grid.sprite_blank) {

		// Play the sprite
		grid.cell[col][row].current_sprite = sprite_to_play;

		// Toggle sprite
		if (sprite_to_play == grid.sprite_red) {
			// Set the colour of the next sprite to be played
			sprite_to_play = grid.sprite_yellow;
		}
		else {
			// Set the colour of the next sprite to be played
			sprite_to_play = grid.sprite_red;
		}

		// Play turn sound effect
		Resource_manager::get_sound("valid_move")->play();

		return true;
	}

	return false;
}

bool Play_state::check_for_win_or_draw(int col, int row) {

	// Check to see if the last move caused a win for the red player
	if (grid.check_for_win(col, row, grid.sprite_red)) {
		win_type = grid.player_red;
		Resource_manager::get_sound("win")->play();
		return true;
	}

	// Check to see if the last move caused a win for the yellow player
	else if (grid.check_for_win(col, row, grid.sprite_yellow)) {
		win_type = grid.player_yellow;
		Resource_manager::get_sound("win")->play();
		return true;
	}

	// Check to see if the last move caused a draw
	else if (grid.check_for_draw()) {
		win_type = grid.player_red + grid.player_yellow;
		Resource_manager::get_sound("draw")->play();
		return true;
	}

	return false;
}
