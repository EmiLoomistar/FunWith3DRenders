#include "display.h"

SDL_Window* window = NULL; // extern means we declare here but initialize in .c
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
	// Initialize SDL...
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the fullscreen max. width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode); // index (0 is main, struct)

	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create a SDL Window...
	window = SDL_CreateWindow(
		NULL, // title
		SDL_WINDOWPOS_CENTERED, // x
		SDL_WINDOWPOS_CENTERED, // y
		window_width, // w
		window_height, // h
		SDL_WINDOW_BORDERLESS // flags
	);
	if (!window) // If error (window pointer is null) 
	{
		fprintf(stderr, "Error creating SDL window");
		return false;
	}

	//Create a SDL renderer ...
	renderer = SDL_CreateRenderer(
		window, // window
		-1, // default display (monitor)
		0 // flags
	);
	if (!renderer) { // if error (renderer pointer is null)
		fprintf(stderr, "Error creating SDL renderer");
		return false;
	}

	// Fullscreen mode
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	//If everything is okay return true
	return true;
}

void draw_grid(int tile_size)
{
	for (int y = 0; y < window_height; y += tile_size)
		for (int x = 0; x < window_width; x += tile_size)
			color_buffer[window_width * y + x] = 0xFF777777;
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x >= 0 && x < window_width && y >= 0 && y < window_height)
		color_buffer[window_width * y + x] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color)
{
	for (int i = y; i < y + height; i++)
		for (int j = x; j < x + width; j++)
			draw_pixel(j, i, color);
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
		for (int x = 0; x < window_width; x++)
			color_buffer[window_width * y + x] = color;
}

void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}