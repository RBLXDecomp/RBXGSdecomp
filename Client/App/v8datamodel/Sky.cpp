#include "v8datamodel/Sky.h"

namespace RBX
{
	Sky::Sky()
		: drawCelestialBodies(true),
		  numStars(3000)
	{
		setName("Sky");

		skyUp = ContentId::fromAssets("Sky\\null_plainsky512_up.jpg");
		skyLf = ContentId::fromAssets("Sky\\null_plainsky512_lf.jpg");
		skyRt = ContentId::fromAssets("Sky\\null_plainsky512_rt.jpg");
		skyBk = ContentId::fromAssets("Sky\\null_plainsky512_bk.jpg");
		skyFt = ContentId::fromAssets("Sky\\null_plainsky512_ft.jpg");
		skyDn = ContentId::fromAssets("Sky\\null_plainsky512_dn.jpg");
	}
}
