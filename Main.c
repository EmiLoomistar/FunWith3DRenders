#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"


#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS]; // 9*9*9 cube
vec2_t projected_points[N_POINTS]; // we have original and projected (scalated and moved)

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 }; // vector showing rotation direction

float fov_factor = 1024;

bool is_running = false;
int previous_frame_time = 0;

void setup(void)
{
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height); // allocates buffer memory and points

	color_buffer_texture = SDL_CreateTexture( // creates SDL texture and points
		renderer,
		SDL_PIXELFORMAT_ARGB8888, // format ARGB
		SDL_TEXTUREACCESS_STREAMING, // cause we will update continously
		window_width,
		window_height
	);

	// load array of vectors
	int point_count = 0;
	for (float x = -1; x <= 1; x += 0.25)
		for (float y = -1; y <= 1; y += 0.25)
			for (float z = -1; z <= 1; z += 0.25)
			{
				vec3_t new_point = { .x = x, .y = y, .z = z };
				cube_points[point_count++] = new_point;
			}
}

void process_input()
{
	SDL_Event event; // creates event (a structure)
	SDL_PollEvent(&event); // sets actual event

	switch (event.type)
	{
	case SDL_QUIT: // this events happens when X is clicked
		is_running = false;
		break;

	case SDL_KEYDOWN: // when key pressed
		if (event.key.keysym.sym == SDLK_ESCAPE) // if key is Esc
			is_running = false;
		break;
	}
}

vec2_t project(vec3_t point)
{
	vec2_t projected_point = {
		.x = point.x * fov_factor / point.z, // fov magnifies (zoom)
		.y = point.y * fov_factor / point.z
	};
	return projected_point;
}

void update(void)
{
	// DEPRECTED SLOW while has not surpassed FRAME_TARGET_TIME stay here
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		SDL_Delay(time_to_wait);

	previous_frame_time = SDL_GetTicks(); // ms since SDL_Init

	cube_rotation.y += 0.02; // defines vector rotation
	cube_rotation.z += 0.02;
	cube_rotation.x += 0.02;

	for (int i = 0; i < N_POINTS; i++)
	{
		vec3_t point = cube_points[i];

		vec3_t transformed_point; // rotates point i
		transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

		// translate point i away from camera
		transformed_point.z -= camera_position.z;

		vec2_t projected_point = project(transformed_point); // scales by fov and applies perspective
	
		projected_points[i] = projected_point; // saves
	}
}

void render(void)
{
	draw_grid(10); // sets pixels in colorbuffer

	for (int i = 0; i < N_POINTS; i++) // for every projected point paints rectangle in colorbuffer
	{
		vec2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x + (window_width / 2), // mid of screen
			projected_point.y + (window_height / 2),
			4, 
			4, 
			0xFFFFFF00);
	}

	render_color_buffer(); // copies color_buffer to color_buffer_texture
	clear_color_buffer(0xFF000000); // clears color_buffer with 0xcolor (black)
	SDL_RenderPresent(renderer); // finally updates screen with color_buffer_texture
}

//////////////////////
// M	A	I	N	//
//////////////////////

int main(void) 
{	
	is_running = initialize_window(); // creates window & renderer

	setup(); // allocates bufferers memory & links buffer_texture to renderer

	while (is_running) // game loop
	{
		process_input(); // inputs
		update(); // change state & wastes time
		render();
	}

	destroy_window();

	return 0;
}