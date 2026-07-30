#include "v8datamodel/BrickColor.h"

namespace RBX
{
	void BrickColor::BrickMap::insert(Number number, unsigned char r, unsigned char g, unsigned char b, std::string name)
	{
		G3D::Color4uint8 c;
		c.r = r;
		c.g = g;
		c.b = b;

		allColors.push_back(BrickColor(number));

		map1.insert(std::pair<Number, G3D::Color4uint8>(number, c));

		G3D::Color4 color4 = G3D::Color4(c);
		map2.insert(std::pair<Number, G3D::Color4>(number, color4));

		map3.insert(std::pair<Number, std::string>(number, name));
	}

	void BrickColor::BrickMap::insertPaletteItem(Number number)
	{
		colorPalette.push_back(BrickColor(number));
	}

	void BrickColor::BrickMap::generatePaletteMap(Number number)
	{
		float closestDistance = 10000.0f;
		int match = 0;
		G3D::Color4 color = map2[number];

		std::vector<BrickColor>::iterator iter = colorPalette.begin();
		std::vector<BrickColor>::iterator end = colorPalette.end();

		for (; iter != end; iter++)
		{
			if (iter->number == number)
			{
				match = std::distance(colorPalette.begin(), iter);
				break;
			}

			G3D::Color3 paletteColor = map2[iter->number];

			float distance = fabs(color.r - paletteColor.r);
			distance += fabs(color.g - paletteColor.g);
			distance += fabs(color.b - paletteColor.b);

			if (distance < closestDistance)
			{
				match = std::distance(colorPalette.begin(), iter);
				if (distance == 0.0f)
					break;
				closestDistance = distance;
			}
		}

		paletteMap[number] = match;
	}

	void BrickColor::BrickMap::generatePaletteMap()
	{
		std::vector<BrickColor>::iterator iter = allColors.begin();
		std::vector<BrickColor>::iterator end = allColors.end();

		for (; iter != end; iter++)
		{
			generatePaletteMap(iter->number);
		}
	}

	BrickColor::BrickMap::BrickMap()
	{
		insert(lego_1, 242, 243, 242, "White");
		insert(lego_2, 161, 165, 162, "Grey");
		insert(lego_3, 249, 233, 153, "Light yellow");
		insert(lego_5, 215, 197, 153, "Brick yellow");
		insert(lego_6, 194, 218, 184, "Light green (Mint)");
		insert(lego_9, 232, 186, 199, "Light reddish violet");
		insert(lego_11, 128, 187, 219, "Pastel Blue");
		insert(lego_12, 203, 132, 66, "Light orange brown");
		insert(lego_18, 204, 142, 104, "Nougat");
		insert(lego_21, 196, 40, 27, "Bright red");
		insert(lego_22, 196, 112, 160, "Med. reddish violet");
		insert(lego_23, 13, 105, 171, "Bright blue");
		insert(lego_24, 245, 205, 47, "Bright yellow");
		insert(lego_25, 98, 71, 50, "Earth orange");
		insert(lego_26, 27, 42, 52, "Black");
		insert(lego_27, 109, 110, 108, "Dark grey");
		insert(lego_28, 40, 127, 70, "Dark green");
		insert(lego_29, 161, 196, 139, "Medium green");
		insert(lego_36, 243, 207, 155, "Lig. Yellowich orange");
		insert(lego_37, 75, 151, 74, "Bright green");
		insert(lego_38, 160, 95, 52, "Dark orange");
		insert(lego_39, 193, 202, 222, "Light bluish violet");
		insert(lego_40, 236, 236, 236, "Transparent");
		insert(lego_41, 205, 84, 75, "Tr. Red");
		insert(lego_42, 193, 223, 240, "Tr. Lg blue");
		insert(lego_43, 123, 182, 232, "Tr. Blue");
		insert(lego_44, 247, 241, 141, "Tr. Yellow");
		insert(lego_45, 180, 210, 227, "Light blue");
		insert(lego_47, 217, 133, 108, "Tr. Flu. Reddish orange");
		insert(lego_48, 132, 182, 141, "Tr. Green");
		insert(lego_49, 248, 241, 132, "Tr. Flu. Green");
		insert(lego_50, 236, 232, 222, "Phosph. White");
		insert(lego_100, 238, 196, 182, "Light red");
		insert(lego_101, 218, 134, 121, "Medium red");
		insert(lego_102, 110, 153, 201, "Medium blue");
		insert(lego_103, 199, 193, 183, "Light grey");
		insert(lego_104, 107, 50, 123, "Bright violet");
		insert(lego_105, 226, 155, 63, "Br. yellowish orange");
		insert(lego_106, 218, 133, 64, "Bright orange");
		insert(lego_107, 0, 143, 155, "Bright bluish green");
		insert(lego_108, 104, 92, 67, "Earth yellow");
		insert(lego_110, 67, 84, 147, "Bright bluish violet");
		insert(lego_111, 191, 183, 177, "Tr. Brown");
		insert(lego_112, 104, 116, 172, "Medium bluish violet");
		insert(lego_113, 228, 173, 200, "Tr. Medi. reddish violet");
		insert(lego_115, 199, 210, 60, "Med. yellowish green");
		insert(lego_116, 85, 165, 175, "Med. bluish green");
		insert(lego_118, 183, 215, 213, "Light bluish green");
		insert(lego_119, 164, 189, 70, "Br. yellowish green");
		insert(lego_120, 217, 228, 167, "Lig. yellowish green");
		insert(lego_121, 231, 172, 88, "Med. yellowish orange");
		insert(lego_123, 211, 111, 76, "Br. reddish orange");
		insert(lego_124, 146, 57, 120, "Bright reddish violet");
		insert(lego_125, 234, 184, 145, "Light orange");
		insert(lego_126, 165, 165, 203, "Tr. Bright bluish violet");
		insert(lego_127, 220, 188, 129, "Gold");
		insert(lego_128, 174, 122, 89, "Dark nougat");
		insert(lego_131, 156, 163, 168, "Silver");
		insert(lego_133, 213, 115, 61, "Neon orange");
		insert(lego_134, 216, 221, 86, "Neon green");
		insert(lego_135, 116, 134, 156, "Sand blue");
		insert(lego_136, 135, 124, 144, "Sand violet");
		insert(lego_137, 224, 152, 100, "Medium orange");
		insert(lego_138, 149, 138, 115, "Sand yellow");
		insert(lego_140, 32, 58, 86, "Earth blue");
		insert(lego_141, 39, 70, 44, "Earth green");
		insert(lego_143, 207, 226, 247, "Tr. Flu. Blue");
		insert(lego_145, 121, 136, 161, "Sand blue metallic");
		insert(lego_146, 149, 142, 163, "Sand violet metallic");
		insert(lego_147, 147, 135, 103, "Sand yellow metallic");
		insert(lego_148, 87, 88, 87, "Dark grey metallic");
		insert(lego_149, 22, 29, 50, "Black metallic");
		insert(lego_150, 171, 173, 172, "Light grey metallic");
		insert(lego_151, 120, 144, 129, "Sand green");
		insert(lego_153, 149, 121, 118, "Sand red");
		insert(lego_154, 123, 46, 47, "Dark red");
		insert(lego_157, 255, 246, 123, "Tr. Flu. Yellow");
		insert(lego_158, 225, 164, 194, "Tr. Flu. Red");
		insert(lego_168, 117, 108, 98, "Gun metallic");
		insert(lego_176, 151, 105, 91, "Red flip/flop");
		insert(lego_178, 180, 132, 85, "Yellow flip/flop");
		insert(lego_179, 137, 135, 136, "Silver flip/flop");
		insert(lego_180, 215, 169, 75, "Curry");
		insert(lego_190, 249, 214, 46, "Fire Yellow");
		insert(lego_191, 232, 171, 45, "Flame yellowish orange");
		insert(lego_192, 105, 64, 39, "Reddish brown");
		insert(lego_193, 207, 96, 36, "Flame reddish orange");
		insert(lego_194, 163, 162, 164, "Medium stone grey");
		insert(lego_195, 70, 103, 164, "Royal blue");
		insert(lego_196, 35, 71, 139, "Dark Royal blue");
		insert(lego_198, 142, 66, 133, "Bright reddish lilac");
		insert(lego_199, 99, 95, 97, "Dark stone grey");
		insert(lego_200, 130, 138, 93, "Lemon metalic");
		insert(lego_208, 229, 228, 222, "Light stone grey");
		insert(lego_209, 176, 142, 68, "Dark Curry");
		insert(lego_210, 112, 149, 120, "Faded green");
		insert(lego_211, 121, 181, 181, "Turquoise");
		insert(lego_212, 159, 195, 233, "Light Royal blue");
		insert(lego_213, 108, 129, 183, "Medium Royal blue");
		insert(lego_216, 143, 76, 42, "Rust");
		insert(lego_217, 124, 92, 69, "Brown");
		insert(lego_218, 150, 112, 159, "Reddish lilac");
		insert(lego_219, 107, 98, 155, "Lilac");
		insert(lego_220, 167, 169, 206, "Light lilac");
		insert(lego_221, 205, 98, 152, "Bright purple");
		insert(lego_222, 228, 173, 200, "Light purple");
		insert(lego_223, 220, 144, 149, "Light pink");
		insert(lego_224, 240, 213, 160, "Light brick yellow");
		insert(lego_225, 235, 184, 127, "Warm yellowish orange");
		insert(lego_226, 253, 234, 140, "Cool yellow");
		insert(lego_232, 125, 187, 221, "Dove blue");
		insert(lego_268, 52, 43, 117, "Medium lilac");

		insertPaletteItem(lego_1);
		insertPaletteItem(lego_208);
		insertPaletteItem(lego_194);
		insertPaletteItem(lego_199);
		insertPaletteItem(lego_26);
		insertPaletteItem(lego_21);
		insertPaletteItem(lego_24);
		insertPaletteItem(lego_226);
		insertPaletteItem(lego_23);
		insertPaletteItem(lego_107);
		insertPaletteItem(lego_102);
		insertPaletteItem(lego_11);
		insertPaletteItem(lego_45);
		insertPaletteItem(lego_135);
		insertPaletteItem(lego_106);
		insertPaletteItem(lego_105);
		insertPaletteItem(lego_141);
		insertPaletteItem(lego_28);
		insertPaletteItem(lego_37);
		insertPaletteItem(lego_119);
		insertPaletteItem(lego_29);
		insertPaletteItem(lego_151);
		insertPaletteItem(lego_38);
		insertPaletteItem(lego_192);
		insertPaletteItem(lego_104);
		insertPaletteItem(lego_9);
		insertPaletteItem(lego_101);
		insertPaletteItem(lego_5);
		insertPaletteItem(lego_153);
		insertPaletteItem(lego_217);
		insertPaletteItem(lego_18);
		insertPaletteItem(lego_125);

		generatePaletteMap();
	}

	BrickColor::BrickMap& BrickColor::BrickMap::singleton()
	{
		static BrickMap s;
		return s;
	}

	const std::vector<BrickColor>& BrickColor::colorPalette()
	{
		return BrickMap::singleton().colorPalette;
	}

	unsigned BrickColor::getClosestPaletteIndex() const
	{
		return BrickMap::singleton().paletteMap[number];
	}

	BrickColor BrickColor::parse(const char* name)
	{
		std::map<enum Number, std::string>::const_iterator iter = BrickMap::singleton().map3.begin();
		std::map<enum Number, std::string>::const_iterator end = BrickMap::singleton().map3.end();

		for (; iter != end; iter++)
		{
			if (iter->second == name)
				return BrickColor(iter->first);
		}

		return BrickColor(lego_194);
	}

	BrickColor BrickColor::random()
	{
		int index = G3D::iRandom(0, (int)BrickMap::singleton().colorPalette.size() - 1);
		return BrickMap::singleton().colorPalette[index];
	}

	BrickColor BrickColor::closest(G3D::Color3 color)
	{
		G3D::Color4 c4;
		c4.r = color.r;
		c4.g = color.g;
		c4.b = color.b;

		return closest(c4);
	}

	BrickColor BrickColor::closest(G3D::Color4 color)
	{
		float closestDistance = 10000.0f;
		Number number = lego_194;

		std::map<enum Number, G3D::Color4>::iterator iter = BrickMap::singleton().map2.begin();
		std::map<enum Number, G3D::Color4>::iterator end = BrickMap::singleton().map2.end();

		for (; iter != end; iter++)
		{
			float distance = fabs(iter->second.r - color.r);
			distance += fabs(iter->second.g - color.g);
			distance += fabs(iter->second.b - color.b);

			if (distance < closestDistance)
			{
				number = iter->first;
				if (distance == 0.0f)
					break;
				closestDistance = distance;
			}
		}

		return BrickColor(number);
	}

	BrickColor::BrickColor(int number)
	{
		std::map<enum Number, G3D::Color4uint8>::iterator it = BrickMap::singleton().map1.find((Number)number);
		if (it != BrickMap::singleton().map1.end())
			this->number = it->first;
		else
			this->number = lego_194;
	}

	const std::string& BrickColor::name() const
	{
		return BrickMap::singleton().map3.find(number)->second;
	}

	G3D::Color4 BrickColor::color4() const
	{
		return BrickMap::singleton().map2.find(number)->second;
	}

	G3D::Color3 BrickColor::color3() const
	{
		return G3D::Color3(color4());
	}
}
