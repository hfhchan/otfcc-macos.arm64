#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../caryll-sfnt.h"
#include "../caryll-font.h"

#include "kit.h"

int main(int argc, char *argv[]) {
	printf("Testing Payload %s\n", argv[1]);
	caryll_sfnt *sfnt = caryll_sfnt_open(argv[1]);
	caryll_font *font = caryll_font_open(sfnt, 0);

	int nChecks = 0;
	
	{ // Simple fields
		assert_equal("head.version", font->head->version, 0x10000);
		assert_equal("OS/2.version", font->OS_2->version, 0x0004);
		assert_equal("OS/2.ulUnicodeRange2", font->OS_2->ulUnicodeRange2, 0x2adf3c10);
	}

	{ // glyf
		assert_equal("Glyph count", font->glyf->numberGlyphs, 15);
		assert_equal("glyf[14] contour count", font->glyf->glyphs[14]->numberOfContours, 2);
		assert_equal("glyf[14] instr length", font->glyf->glyphs[14]->instructionsLength, 281);
		assert_equal("glyf[14] contour[0] pts", font->glyf->glyphs[14]->contours[0].pointsCount, (11 - 0 + 1));
		assert_equal("glyf[14] contour[1] pts", font->glyf->glyphs[14]->contours[1].pointsCount, (56 - 12 + 1));
	}

	{ // Glyph order and naming
		glyph_order_entry *s;
		int testindex = 0;
		HASH_FIND_INT(*(font->glyph_order), &testindex, s);
		assert_exists("Glyph 0 has a name", s);
		if (s != NULL) assert_equal("Glyph 0 is named as .notdef", strcmp(s->name, ".notdef"), 0);

		testindex = 5;
		HASH_FIND_INT(*(font->glyph_order), &testindex, s);
		assert_exists("Glyph 5 has a name", s);
		if (s != NULL) assert_equal("Glyph 5 is named as NameMe.3", strcmp(s->name, "NameMe.3"), 0);

		bool allGlyphNamed = true;
		for (int index = 0; index < font->glyf->numberGlyphs; index++) {
			HASH_FIND_INT(*(font->glyph_order), &index, s);
			if (s == NULL) allGlyphNamed = false;
			if (font->glyf->glyphs[index]->name == NULL) {
				printf("%d\n", index);
				allGlyphNamed = false;
			}
		}
		assert_equal("All glyphs are named", allGlyphNamed, true);
	}

	{ // cmap (flatten)
		cmap_entry *s;
		int testindex = 0x888B;
		HASH_FIND_INT(*(font->cmap), &testindex, s);
		assert_exists("Found cmap entry for U+888B", s);
		if (s != NULL) {
			assert_equal("U+888B Mapping correct", s->glyph.gid, 13);
			assert_equal("U+888B is named as NameMe.11", strcmp(s->glyph.name, "NameMe.11"), 0);
		}

		testindex = 0x9df9;
		HASH_FIND_INT(*(font->cmap), &testindex, s);
		assert_exists("Found cmap entry for U+9DF9", s);
		if (s != NULL) assert_equal("U+9DF9 Mapping correct", s->glyph.gid, 9);
	}

	caryll_font_close(font);
	caryll_sfnt_close(sfnt);
	return 0;
}