#include "tweaker.hpp"

game::Tweaker::Tweaker(g::asset::store& assets)
{
	objects["player"] = g::game::object{&assets, "data/player.yaml", {
        { 
        	"traits", 
        	{
                { "hp", 99.f },
                { "roll_speed", 1.f },
                { "thrust", 1.f} 
            }
        },
        { 
        	"textures", 
        	{
                { "sprite", "ufo.png" }
            }
        },
    }};

	objects["world"] = g::game::object{&assets, "data/world.yaml", {
        { 
        	"traits", 
        	{
	                { "gravity", -1.f }
	        }
    	}
    }};
}