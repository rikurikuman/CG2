#include "Color.h"

Color Color::convertFromHSVA(int h, int s, int v, int a)
{
	Color c;

	while (h < 0) { h += 360; }
	while (h > 360) { h -= 360; }
	if (s < 0) { s = 0; }
	if (s > 100) { s = 100; }
	if (v < 0) { v = 0; }
	if (v > 100) { v = 100; }

	s = (int)(255 * s / 100.0f);
	v = (int)(255 * v / 100.0f);

	int MAX = v;
	int MIN = (int)(MAX - ((s / 255.0f) * MAX));

	if (h >= 0 && h <= 60) {
		return Color(MAX / 255.0f, ((h / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MIN / 255.0f, a / 255.0f);
	}
	else if (h >= 61 && h <= 120) {
		return Color((((120 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MAX / 255.0f, MIN / 255.0f, a / 255.0f);
	}
	else if (h >= 121 && h <= 180) {
		return Color(MIN / 255.0f, MAX / 255.0f, (((h - 120) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, a / 255.0f);
	}
	else if (h >= 181 && h <= 240) {
		return Color(MIN / 255.0f, (((240 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MAX / 255.0f, a / 255.0f);
	}
	else if (h >= 241 && h <= 300) {
		return Color((((h - 240) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MIN / 255.0f, MAX / 255.0f, a / 255.0f);
	}
	else if (h >= 301 && h <= 360) {
		return Color(MAX / 255.0f, MIN / 255.0f, (((360 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, a / 255.0f);
	}

    return Color();
}
