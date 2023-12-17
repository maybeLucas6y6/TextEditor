#include "raylib.h"

#include "multitextarea.h"

int main() {
	constexpr int window_width = 800, window_height = 600;
	InitWindow(window_width, window_height, "Text Editor");

	Font font = LoadFont("resources/IBMPlexMono-Medium.ttf");
	MultiLineTextArea area1(10.0f, 10.0f, 8, 44, font, 20.0f, 5.0f, DARKGRAY, LIGHTGRAY);
	MultiLineTextArea area2(10.0f, 300.0f, 8, 44, font, 20.0f, 5.0f, DARKGRAY, LIGHTGRAY);

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		area1.Edit();
		area2.Edit();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(600, 500);

		area1.Draw();
		area2.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}