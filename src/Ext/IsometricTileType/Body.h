#pragma once

#include <IsometricTileTypeClass.h>

#include "../_Container.hpp"
#include "../../Phobos.h"

class IsometricTileTypeExt
{
public:
	using base_type = IsometricTileTypeClass;

	class ExtData final : public Extension<IsometricTileTypeClass>
	{
	public:
		char PaletteBuffer[32];
		BytePalette* Palette;
		ConvertClass* Convert;

		ExtData(IsometricTileTypeClass* OwnerObject) : Extension<IsometricTileTypeClass>(OwnerObject),
			PaletteBuffer(""),
			Palette(nullptr),
			Convert(nullptr)
		{ }

		void SetConvert();

		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual ~ExtData() = default;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override {}

		virtual void LoadFromStream(IStream* Stm);
		virtual void SaveToStream(IStream* Stm);
	};

	class ExtContainer final : public Container<IsometricTileTypeExt> {
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;
};
