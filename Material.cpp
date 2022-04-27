#include "Material.h"

void Material::Transfer(MaterialBuffer* target)
{
	target->color = color;
}
