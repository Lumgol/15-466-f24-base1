#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	const static uint8_t num_hat_sprites = 15;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	// hat positions:
	glm::vec2 hat_positions[num_hat_sprites] = {
		glm::vec2(1.f,  4.f ) * 8.f, glm::vec2(8.f,  2.f ) * 8.f, glm::vec2(17.f, 1.f ) * 8.f,
		glm::vec2(29.f, 4.f ) * 8.f, glm::vec2(29.f, 12.f) * 8.f, glm::vec2(23.f, 11.f) * 8.f,
		glm::vec2(1.f,  9.f ) * 8.f, glm::vec2(4.f,  11.f) * 8.f, glm::vec2(9.f,  12.f) * 8.f,
		glm::vec2(15.f, 13.f) * 8.f, glm::vec2(30.f, 17.f) * 8.f, glm::vec2(24.f, 18.f) * 8.f,
		glm::vec2(15.f, 17.f) * 8.f, glm::vec2(8.f,  20.f) * 8.f, glm::vec2(1.f,  15.f) * 8.f
	};

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
