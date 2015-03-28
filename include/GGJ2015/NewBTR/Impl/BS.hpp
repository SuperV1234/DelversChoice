#ifndef GGJ2015_NEWBTR_IMPL_BTR
#define GGJ2015_NEWBTR_IMPL_BTR

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

#include "../../NewBTR/Impl/BTREffect.hpp"
#include "../../NewBTR/Impl/BTRChunk.hpp"
#include "../../NewBTR/Impl/BTREWave.hpp"
#include "../../NewBTR/Impl/BTREColor.hpp"
#include "../../NewBTR/Impl/BTRDrawState.hpp"
#include "../../NewBTR/Impl/BTRRoot.hpp"

namespace ssvs
{
	namespace BTR
	{
		struct Tracking { };
		struct Leading { };
		struct HChunkSpacing { };
		struct Pulse { };
		struct PulseDef { };
		using Color = Impl::BTREColor;
		using Wave = Impl::BTREWave;
		using Chunk = Impl::BTRChunk;
	}

	using BitmapTextRich = BTR::Impl::BTRRoot;
}

#endif
