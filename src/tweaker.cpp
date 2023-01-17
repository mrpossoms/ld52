#include "tweaker.hpp"

game::Tweaker::Tweaker(g::asset::store& assets)
{
	objects["player"] = g::game::object{&assets, "data/player.yaml", {
		{ 
			"traits", 
			{
				{ "hp", 99.f },
				{ "roll_speed", 1.f },
				{ "thrust", 1.f},
				{ "tractor", 1.f}
			}
		},
		{ 
			"textures", 
			{
				{ "sprite", "ufo.pixelated.png" }
			}
		},
	}};

	objects["world"] = g::game::object{&assets, "data/world.yaml", {
		{ 
			"traits", 
			{
				{ "gravity", -1.f }
			}
		},
		{ 
			"textures", 
			{
				{ "underground", "world.underground.pixelated.repeating.png" },
				{ "ground", "world.ground.pixelated.repeating.png" },
			}
		},
	}};

	objects["abductee.dog"] = g::game::object{&assets, "abductee.dog.yaml", {
		{ 
			"traits", 
			{
				{ "speed", 1.f },
			}
		},
		{ 
			"sprites", 
			{
				{ "sprite", "abductee.dog.pixelated.json" }
			}
		},
	}};

	objects["abductee.cow"] = g::game::object{&assets, "abductee.cow.yaml", {
		{ 
			"traits", 
			{
				{ "speed", 1.f },
			}
		},
		{ 
			"sprites", 
			{
				{ "sprite", "abductee.cow.pixelated.json" }
			}
		},
	}};

	objects["abductee.chicken"] = g::game::object{&assets, "abductee.chicken.yaml", {
		{ 
			"traits", 
			{
				{ "speed", 1.f },
			}
		},
		{ 
			"sprites", 
			{
				{ "sprite", "abductee.chicken.pixelated.json" }
			}
		},
	}};

	objects["abductee.chicken2"] = g::game::object{ &assets, "abductee.chicken2.yaml", {
		{
			"traits",
			{
				{ "speed", 1.f },
			}
		},
		{
			"sprites",
			{
				{ "sprite", "abductee.chicken2.pixelated.json" }
			}
		},
	} };

	objects["abductee.farmer"] = g::game::object{&assets, "abductee.farmer.yaml", {
		{ 
			"traits", 
			{
				{ "speed", 1.f },
			}
		},
		{ 
			"sprites", 
			{
				{ "sprite", "abductee.farmer.pixelated.json" }
			}
		},
	}};
}