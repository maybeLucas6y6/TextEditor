#include "raylib.h"

#include "multitextarea.h"

int main() {
	constexpr int window_width = 800, window_height = 600;
	InitWindow(window_width, window_height, "Text Editor");

	MultiLineTextArea area(10.0f, 10.0f, 8, 32, "resources/IBMPlexMono-Regular.ttf", 32.0f, 5.0f, LIGHTGRAY);

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		area.Edit();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(600, 5);

		area.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}