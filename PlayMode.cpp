#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

#include "read_assets.hpp"
#include "palettes.cpp"
#include "tilemap.cpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them!
	// or, at least, if you do hardcode them,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	// copying static palette array into ppu's palette table
	std::copy_n(std::make_move_iterator(game1_palettes), 8, ppu.palette_table.begin());

	// reading & copying tile images into ppu's tile table
	ppu.tile_table[0]  = read_tile("transparent");
	ppu.tile_table[1]  = read_tile("ul_outer_corner");
	ppu.tile_table[2]  = read_tile("ur_outer_corner");
	ppu.tile_table[3]  = read_tile("ll_outer_corner");
	ppu.tile_table[4]  = read_tile("lr_outer_corner");
	ppu.tile_table[5]  = read_tile("u_side");
	ppu.tile_table[6]  = read_tile("d_side");
	ppu.tile_table[7]  = read_tile("l_side");
	ppu.tile_table[8]  = read_tile("r_side");
	ppu.tile_table[9]  = read_tile("ul_inner_corner");
	ppu.tile_table[10] = read_tile("ur_inner_corner");
	ppu.tile_table[11] = read_tile("ll_inner_corner");
	ppu.tile_table[12] = read_tile("lr_inner_corner");
	ppu.tile_table[13] = read_tile("vert");
	ppu.tile_table[14] = read_tile("horiz");
	ppu.tile_table[15] = read_tile("not_up");
	ppu.tile_table[16] = read_tile("not_down");
	ppu.tile_table[17] = read_tile("not_left");
	ppu.tile_table[18] = read_tile("not_right");
	ppu.tile_table[19] = read_tile("ul_in_out");
	ppu.tile_table[20] = read_tile("ur_in_out");
	ppu.tile_table[21] = read_tile("ll_in_out");
	ppu.tile_table[22] = read_tile("lr_in_out");
	ppu.tile_table[23] = read_tile("ll_lr_out");
	ppu.tile_table[24] = read_tile("ll_ul_out");
	ppu.tile_table[25] = read_tile("l_side_lr_corn");
	ppu.tile_table[26] = read_tile("l_side_corns");
	ppu.tile_table[27] = read_tile("u_side_lr_corn");
	ppu.tile_table[28] = read_tile("d_side_ul_corn");
	ppu.tile_table[29] = read_tile("d_side_corns");
	ppu.tile_table[30] = read_tile("l_side_ur_corn");
	ppu.tile_table[31] = read_tile("r_side_ll_corn");
	ppu.tile_table[32] = read_tile("d_side_ur_corn");
	ppu.tile_table[33] = read_tile("ul_lr_out");
	ppu.tile_table[34] = read_tile("r_side_corns");

	for (uint16_t row = 0; row < 30; row++) {
		for (uint16_t col = 0; col < 32; col++) {
			uint16_t idx = row * ppu.BackgroundWidth + col;
			// tile_idxs stores rows in backwards order for human readability
			ppu.background[idx] = tile_idxs[29 - row][col];
			ppu.background[idx] += (palette_idxs[row] << 8);
		}
	}

	// use sprite 35 as a "player":
	ppu.tile_table[35] = read_tile("player");
	player_at = glm::uvec2(8., 8.);

	// initialize hat tiles!
	ppu.tile_table[36] = read_tile("wizard_hat");
	ppu.tile_table[37] = read_tile("top_hat");

	// placeholders for out-of-bounds and not-yet-created tiles
	for (uint i = 38; i < 256; i++) {
		ppu.tile_table[i] = read_tile("transparent");
	}

	// hat sprites :3
	for (uint8_t h = 1; h < 1 + num_hat_sprites; h++) {
		ppu.sprites[h].index = 36 + h % 2;
		ppu.sprites[h].attributes = 0;
	}
}

PlayMode::~PlayMode() {
}

// event handler kept identical to base game code :)
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	constexpr float speed = 30.0f;
	const float delta = speed * elapsed;

	// collision shenanigans

	// helper to check whether a certain index in the background points to a nonzero tile
	std::function<bool(uint16_t)> is_collidable = [this](uint16_t idx){
		return (bool) (ppu.background[idx] & ((1<<8) - 1));
	};

	// object_pos is either the player's or a hat's position
	std::function<glm::vec2(glm::vec2)> full_collision_check = [this, is_collidable, delta](glm::vec2 object_pos) {
		glm::vec2 new_pos = object_pos;
		glm::vec2 res;
		if (left.pressed) new_pos.x -= delta;
		if (right.pressed) new_pos.x += delta;
		if (down.pressed) new_pos.y -= delta;
		if (up.pressed) new_pos.y += delta;

		glm::u8vec2 grid_indices = (glm::u8vec2) floor(new_pos / 8.f);
		glm::u8vec2 old_grid_indices = (glm::u8vec2) floor(object_pos / 8.f);

		uint16_t ll_grid_idx = grid_indices.y * ppu.BackgroundWidth + grid_indices.x;
		uint16_t lr_grid_idx = grid_indices.y * ppu.BackgroundWidth + grid_indices.x + 1;
		uint16_t ul_grid_idx = (grid_indices.y + 1) * ppu.BackgroundWidth + grid_indices.x;
		uint16_t ur_grid_idx = (grid_indices.y + 1) * ppu.BackgroundWidth + grid_indices.x + 1;

		if (left.pressed  
			&& (is_collidable(ll_grid_idx)
			|| (is_collidable(ul_grid_idx) && ((int) new_pos.y % 8 != 0)))
		){
			res.x = 8.f * old_grid_indices.x - object_pos.x;
			res.y = new_pos.y - object_pos.y;
		} else if (right.pressed  
			&& (is_collidable(lr_grid_idx)
			|| (is_collidable(ur_grid_idx) && ((int) new_pos.y % 8 != 0)))
		){
			res.x = 8.f * grid_indices.x - object_pos.x;
			res.y = new_pos.y - object_pos.y;
		} else if (down.pressed  
			&& (is_collidable(ll_grid_idx)
			|| (is_collidable(lr_grid_idx) && ((int) new_pos.x % 8 != 0)))
		){
			res.y = (8.f * old_grid_indices.y - object_pos.y);
			res.x = new_pos.x - object_pos.x;
		} else if (up.pressed 
			&& (is_collidable(ul_grid_idx)
			|| (is_collidable(ur_grid_idx) && ((int) new_pos.x % 8 != 0)))
		){
			res.y = 8.f * grid_indices.y - object_pos.y;
			res.x = new_pos.x - object_pos.x;
		} else res = new_pos - object_pos;

		if (new_pos.x < 0) res.x = 0;
		if (new_pos.y < 0) res.y = 0; 
		return res;
	};

	std::function<glm::vec2(glm::vec2, glm::vec2)> find_min_delta = [](glm::vec2 a, glm::vec2 b) {
		float res_x = std::abs(a.x) <= std::abs(b.x) ? a.x : b.x;
		float res_y = std::abs(a.y) <= std::abs(b.y) ? a.y : b.y;
		return glm::vec2(res_x, res_y);
	};

	glm::vec2 min_delta = full_collision_check(player_at);
	for (uint8_t h = 0; h < num_hat_sprites; h++) {
		PPU466::Sprite hat = ppu.sprites[h+1];
		if (hat.attributes == 2) {
			glm::vec2 hat_delta = full_collision_check(hat_positions[h]);
			min_delta = find_min_delta(min_delta, hat_delta);
		}
	}
	for (uint8_t h = 0; h < num_hat_sprites; h++) {
		PPU466::Sprite hat = ppu.sprites[h+1];
		if (hat.attributes == 2) {
			hat_positions[h] += min_delta;
		}
	} player_at += min_delta;

	// stick hats to player if they're Exactly 1 tile away horizontally or vertically
	for (uint8_t h = 1; h < 1 + num_hat_sprites; h++) {
		PPU466::Sprite hat = ppu.sprites[h];
		if ((hat.y == player_at.y && (hat.x + 8 == player_at.x || hat.x - 8 == player_at.x))
			|| (hat.x == player_at.x && (hat.y + 8 == player_at.y || hat.y - 8 == player_at.y))) {
			// turns blue when stuck to player
			ppu.sprites[h].attributes = 2;
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will NOT be some hsv-like fade:
	ppu.background_color = glm::u8vec4(0x00);

	//player sprite (adapted from base game code):
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = 35;
	ppu.sprites[0].attributes = 7;

	// move hat sprites
	for (uint8_t h = 0; h < num_hat_sprites; h++) {
		ppu.sprites[h+1].x = (int8_t)hat_positions[h].x;
		ppu.sprites[h+1].y = (int8_t)hat_positions[h].y;
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
