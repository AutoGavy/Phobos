#pragma once
#include <TechnoTypeClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

class TechnoTypeExt
{
public:
	using base_type = TechnoTypeClass;

	class ExtData final : public Extension<TechnoTypeClass>
	{
	public:
		PhobosFixedString<0x20> GroupAs;
		//Valueable<int> RadarJamRadius;
		//Valueable<bool> VirtualUnit;
		//Valueable<bool> RemoveVolume;
		Valueable<bool> SameTargetAsTransporter;
		Valueable<bool> LowSelectionPriority;
		//Nullable<int> InhibitorRange;

		ExtData(TechnoTypeClass* OwnerObject) : Extension<TechnoTypeClass>(OwnerObject)
			, GroupAs { NONE_STR }
			//, RadarJamRadius { 0 }
			//, VirtualUnit { false }
			//, RemoveVolume { false }
			, SameTargetAsTransporter { false }
			, LowSelectionPriority { false }
			//, InhibitorRange {}
		{ }

		virtual ~ExtData() = default;
		virtual void LoadFromINIFile(CCINIClass* pINI) override;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

		// Ares 0.A
		const char* GetSelectionGroupID() const;

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TechnoTypeExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;
};
