#include "Wireframe.h"

#define INTERIOR_DOOR_WIDTH 0.8
#define EXTERIOR_DOOR_WIDTH 0.9

Wireframe GenerateProcedural(int floorCount, float wallHeight)
{
	assert(floorCount > 1);
	Wireframe result;

	float bedroomSizeX = 4.0f;
	float bedroomSizeY = 3.0f;
	float bedroomBathSizeX = 2.0f;
	float smallBathroomSize = 2.0f;

	// ground floor
	{
		int a = result.AddVertex({0.0f, 0.0f, 0.0f});
		int b = result.AddVertex({smallBathroomSize + 6.0f, 0.0f, 0.0f});
		int c = result.AddVertex({smallBathroomSize + 6.0f, 0.0f, 4.0f + 3.0f});
		int d = result.AddVertex({0.0f, 0.0f, 4.0f + 3.0f});

		int batha = a;
		int bathb = result.AddVertex({smallBathroomSize, 0.0f, 0.0f});
		int bathc = result.AddVertex({smallBathroomSize, 0.0f, smallBathroomSize});
		int bathd = result.AddVertex({0.0f, 0.0f, smallBathroomSize});

		int livinga = bathb;
		int livingb = b;
		int livingc = result.AddVertex({smallBathroomSize + 6.0f, 0.0f, 4.0f});
		int livingd = result.AddVertex({smallBathroomSize, 0.0f, 4.0f});

		int kitchena = livingd;
		int kitchenb = livingc;
		int kitchenc = c;
		int kitchend = result.AddVertex({smallBathroomSize, 0.0f, 4.0f + 3.0f});


		int entrance0 = result.AddVertex({0.0f, 0.0f, 2.7f + EXTERIOR_DOOR_WIDTH * 0.5f});
		int entrance1 = result.AddVertex({0.0f, 0.0f, 2.7f - EXTERIOR_DOOR_WIDTH * 0.5f});

		int bathentrance0 = result.AddVertex({1.0f - INTERIOR_DOOR_WIDTH * 0.5f, 0.0f, smallBathroomSize});
		int bathentrance1 = result.AddVertex({1.0f + INTERIOR_DOOR_WIDTH * 0.5f, 0.0f, smallBathroomSize});

		int livingentrance0 = result.AddVertex({2.0f, 0.0f, 2.7f + INTERIOR_DOOR_WIDTH * 0.5f});
		int livingentrance1 = result.AddVertex({2.0f, 0.0f, 2.7f - INTERIOR_DOOR_WIDTH * 0.5f});

		int kitchenentrance0 = result.AddVertex({2.0f + 1.0f - INTERIOR_DOOR_WIDTH * 0.5f, 0.0f, 4.0f});
		int kitchenentrance1 = result.AddVertex({2.0f + 1.0f + INTERIOR_DOOR_WIDTH * 0.5f, 0.0f, 4.0f});

		int livingwindowa0 = result.AddVertex({3.0f, 0.0f, 0.0f});
		int livingwindowa1 = result.AddVertex({7.0f, 0.0f, 0.0f});
		int livingwindowb0 = result.AddVertex({8.0f, 0.0f, 1.0f});
		int livingwindowb1 = result.AddVertex({8.0f, 0.0f, 3.0f});
		int kitchenwindowa0 = result.AddVertex({8.0f, 0.0f, 5.0f});
		int kitchenwindowa1 = result.AddVertex({8.0f, 0.0f, 6.0f});
		int kitchenwindowb0 = result.AddVertex({4.0f, 0.0f, 7.0f});
		int kitchenwindowb1 = result.AddVertex({6.0f, 0.0f, 7.0f});

		int staird = result.AddVertex({0.5f, 0.0f, 3.5f});
		int stairc = result.AddVertex({0.5f, 0.0f, 6.5f});
		int stairb = result.AddVertex({1.5f, 0.0f, 6.5f});
		int staira = result.AddVertex({1.5f, 0.0f, 3.5f});

		result.Connect(a, bathb, EdgeType::Wall);
		result.Connect(bathb, livingwindowa0, EdgeType::Wall);
		result.Connect(livingwindowa0, livingwindowa1, EdgeType::Window);
		result.Connect(livingwindowa1, b, EdgeType::Wall);
		result.Connect(b, livingwindowb0, EdgeType::Wall);
		result.Connect(livingwindowb0, livingwindowb1, EdgeType::Window);
		result.Connect(livingwindowb1, livingc, EdgeType::Wall);
		result.Connect(livingc, kitchenwindowa0, EdgeType::Wall);
		result.Connect(kitchenwindowa0, kitchenwindowa1, EdgeType::Window);
		result.Connect(kitchenwindowa1, c, EdgeType::Wall);
		result.Connect(c, kitchenwindowb1, EdgeType::Wall);
		result.Connect(kitchenwindowb0, kitchenwindowb1, EdgeType::Window);
		result.Connect(kitchenwindowb0, kitchend, EdgeType::Wall);
		result.Connect(kitchend, d, EdgeType::Wall);
		result.Connect(d, entrance0, EdgeType::Wall);
		result.Connect(entrance0, entrance1, EdgeType::Door);
		result.Connect(entrance1, a, EdgeType::Wall);

		result.Connect(bathd, bathentrance0, EdgeType::Wall);
		result.Connect(bathentrance0, bathentrance1, EdgeType::Door);
		result.Connect(bathentrance1, bathc, EdgeType::Wall);
		result.Connect(bathc, bathb, EdgeType::Wall);

		result.Connect(bathc, livingentrance1, EdgeType::Wall);
		result.Connect(livingentrance0, livingentrance1, EdgeType::Door);
		result.Connect(livingentrance0, livingd, EdgeType::Wall);
		result.Connect(livingd, kitchend, EdgeType::Wall);

		result.Connect(kitchena, kitchenentrance0, EdgeType::Wall);
		result.Connect(kitchenentrance0, kitchenentrance1, EdgeType::Door);
		result.Connect(kitchenentrance1, kitchenb, EdgeType::Wall);

		result.Connect(staira, stairb, EdgeType::StandardStairs);
		result.Connect(stairb, stairc, EdgeType::StandardStairs);
		result.Connect(stairc, staird, EdgeType::StandardStairs);
	}

	// bedroom floor
	for (int i = 1; i < floorCount; i++)
	{
		float currentHeight = wallHeight * i;
		int a = result.AddVertex({0.0f, currentHeight, 0.0f});
		int b = result.AddVertex({smallBathroomSize + 6.0f, currentHeight, 0.0f});
		int c = result.AddVertex({smallBathroomSize + 6.0f, currentHeight, 4.0f + 3.0f});
		int d = result.AddVertex({0.0f, currentHeight, 4.0f + 3.0f});

		int holea = result.AddVertex({0.01f, currentHeight, 3.5f});
		int holeb = result.AddVertex({1.0f, currentHeight, 3.5f});
		int holec = result.AddVertex({1.0f, currentHeight, 6.99f});
		int holed = result.AddVertex({0.01f, currentHeight, 6.99f});

		int bed0a = result.AddVertex({2.0f, currentHeight, 3.5f});
		int bed0i0 = result.AddVertex({3.0f, currentHeight, 3.5f});
		int bed0b = result.AddVertex({6.0f, currentHeight, 3.5f});
		int bed0c = result.AddVertex({6.0f, currentHeight, 7.0f});
		int bed0d = result.AddVertex({2.0f, currentHeight, 7.0f});

		int bed0entrance0 = result.AddVertex({2.5f - INTERIOR_DOOR_WIDTH * 0.5f, currentHeight, 3.5f});
		int bed0entrance1 = result.AddVertex({2.5f + INTERIOR_DOOR_WIDTH * 0.5f, currentHeight, 3.5f});

		int bed1a = result.AddVertex({2.0f, currentHeight, 0.0f});
		int bed1b = result.AddVertex({6.0f, currentHeight, 0.0f});
		int bed1c = result.AddVertex({6.0f, currentHeight, 3.5f});
		int bed1d = bed0i0;
		int bed1e = result.AddVertex({3.0f, currentHeight, 2.5f});
		int bed1f = result.AddVertex({2.0f, currentHeight, 2.5f});

		int bed1entrance0 = result.AddVertex({2.5f - INTERIOR_DOOR_WIDTH * 0.5f, currentHeight, 2.5f});
		int bed1entrance1 = result.AddVertex({2.5f + INTERIOR_DOOR_WIDTH * 0.5f, currentHeight, 2.5f});

		int bath0a = bed0b;
		int bath0b = result.AddVertex({8.0f, currentHeight, 3.5f});
		int bath0c = c;
		int bath0d = bed0c;

		int bath1a = bed1b;
		int bath1b = b;
		int bath1c = bath0b;
		int bath1d = bed0b;

		int bed0win0 = result.AddVertex({3.0f, currentHeight, 7.0f});
		int bed0win1 = result.AddVertex({5.0f, currentHeight, 7.0f});

		int bed1win0 = result.AddVertex({3.0f, currentHeight, 0.0f});
		int bed1win1 = result.AddVertex({5.0f, currentHeight, 0.0f});

		int bath0win0 = result.AddVertex({8.0f, currentHeight, 5.0f});
		int bath0win1 = result.AddVertex({8.0f, currentHeight, 6.0f});

		int bath1win0 = result.AddVertex({8.0f, currentHeight, 1.0f});
		int bath1win1 = result.AddVertex({8.0f, currentHeight, 2.0f});

		// result.ConnectLoop({a, bed1a, bed1b, b, bath1c, c, bath0d, bed0d, d}, EdgeType::Wall);
		result.Connect(a, bed1a, EdgeType::Wall);
		result.Connect(bed1a, bed1win0, EdgeType::Wall);
		result.Connect(bed1win0, bed1win1, EdgeType::Window);
		result.Connect(bed1win1, bed1b, EdgeType::Wall);
		result.Connect(bed1b, b, EdgeType::Wall);
		result.Connect(b, bath1win0, EdgeType::Wall);
		result.Connect(bath1win1, bath1win0, EdgeType::Window);
		result.Connect(bath1win1, bath1c, EdgeType::Wall);
		result.Connect(bath1c, bath0win0, EdgeType::Wall);
		result.Connect(bath0win0, bath0win1, EdgeType::Window);
		result.Connect(bath0win1, c, EdgeType::Wall);
		result.Connect(c, bath0d, EdgeType::Wall);
		result.Connect(bath0d, bed0win1, EdgeType::Wall);
		result.Connect(bed0win1, bed0win0, EdgeType::Window);
		result.Connect(bed0win0, bed0d, EdgeType::Wall);
		result.Connect(bed0d, d, EdgeType::Wall);
		result.Connect(d, a, EdgeType::Wall);

		result.ConnectChain({bed1d, bed1c, bath1c}, EdgeType::Wall);
		result.Connect(bed1a, bed1f, EdgeType::Wall);
		result.Connect(bed0d, bed0a, EdgeType::Wall);

		result.Connect(bed0a, bed0entrance0, EdgeType::Wall);
		result.Connect(bed0entrance1, bed0i0, EdgeType::Wall);
		result.Connect(bed0entrance0, bed0entrance1, EdgeType::Door);

		result.Connect(bed1d, bed1e, EdgeType::Wall);
		result.Connect(bed1e, bed1entrance1, EdgeType::Wall);
		result.Connect(bed1entrance1, bed1entrance0, EdgeType::Door);
		result.Connect(bed1entrance0, bed1f, EdgeType::Wall);

		result.ConnectLoop({holea, holeb, holec, holed}, EdgeType::Hole);

		if (i < floorCount - 1)
		{
			int staird = result.AddVertex({0.5f, currentHeight, 3.5f});
			int stairc = result.AddVertex({0.5f, currentHeight, 6.5f});
			int stairb = result.AddVertex({1.5f, currentHeight, 6.5f});
			int staira = result.AddVertex({1.5f, currentHeight, 3.5f});

			result.Connect(staira, stairb, EdgeType::StandardStairs);
			result.Connect(stairb, stairc, EdgeType::StandardStairs);
			result.Connect(stairc, staird, EdgeType::StandardStairs);
		}
	}

	return result;
}