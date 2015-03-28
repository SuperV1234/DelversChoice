#ifndef GGJ2015_NEWBTR_IMPL_BTRROOT
#define GGJ2015_NEWBTR_IMPL_BTRROOT

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

#include "../../NewBTR/Impl/BTREffect.hpp"
#include "../../NewBTR/Impl/BTRChunk.hpp"
#include "../../NewBTR/Impl/BTREWave.hpp"
#include "../../NewBTR/Impl/BTREColor.hpp"
#include "../../NewBTR/Impl/BTRDrawState.hpp"

namespace ssvs
{
	namespace Impl
	{
		class BTRRoot : public sf::Transformable, public sf::Drawable
		{
			friend class BTRChunk;
			template<typename> friend struct Impl::EffectHelper;

			private:
				const BitmapFont* bitmapFont{nullptr};
				const sf::Texture* texture{nullptr};
				mutable VertexVector<sf::PrimitiveType::Quads> vertices, verticesOriginal;
				mutable sf::FloatRect bounds, globalBounds;
				mutable bool mustRefreshGeometry{true};

				BTRChunkRecVector chunks;
				BTREffectRecVector effects;

				BTRChunk* baseChunk{&mkChunk()};
				BTRChunk* lastChunk{baseChunk};

				float alignMult{0.f};

				mutable BTRDrawState bdd;

				inline void refreshIfNeeded() const
				{
					refreshGeometryIfNeeded();
					baseChunk->refreshEffects();
				}

				inline void refreshGeometryIfNeeded() const
				{
					if(!mustRefreshGeometry) return;
					mustRefreshGeometry = false;

					refreshGeometryStart();
					baseChunk->refreshGeometry();
					verticesOriginal = vertices;
					refreshGeometryFinish();
				}

				template<typename... TArgs> inline BTRChunk& mkChunk(TArgs&&... mArgs)
				{
					auto& c(chunks.create(*this, FWD(mArgs)...));
					lastChunk = &c;
					return c;
				}
				template<typename T, typename... TArgs> inline T& mkEffect(TArgs&&... mArgs)
				{
					return effects.create<T>(FWD(mArgs)...);
				}

				inline void pushRowData() const
				{
					bdd.rDatas.emplace_back(vertices.back().position.x, bdd.iX);
				}

				inline void refreshGeometryStart() const noexcept
				{
					SSVU_ASSERT(bitmapFont != nullptr);

					vertices.clear();
					verticesOriginal.clear();
					bdd.reset(*bitmapFont);
				}

				inline void refreshGeometryFinish() const
				{
					// Push last row data
					pushRowData();

					// Recalculate bounds
					auto width(bdd.xMax - bdd.xMin);
					bounds = {bdd.xMin, bdd.yMin, width, bdd.yMax - bdd.yMin};
					globalBounds = getTransform().transformRect(bounds);

					// Apply horizontal alignment
					SizeT lastVIdx{0};
					for(const auto& rd : bdd.rDatas)
					{
						auto targetVIdx(lastVIdx + rd.cells * 4);
						auto offset(width - rd.width);

						for(; lastVIdx < targetVIdx; ++lastVIdx) vertices[lastVIdx].position.x += offset * alignMult;
					}
				}

				inline void mkVertices(BTRChunk& mChunk) const
				{
					const auto& str(mChunk.str);
					bdd.nextHChunkSpacing = mChunk.getHChunkSpacing();
					mChunk.idxHierarchyBegin = vertices.size();

					for(const auto& c : str)
					{
						switch(c)
						{
							case L'\n':  ++bdd.nl;		continue;
							case L'\t':  ++bdd.htab;	continue;
							case L'\v':  ++bdd.vtab;	continue;
						}

						const auto& tracking(mChunk.getTracking());
						const auto& leading(mChunk.getLeading());
						const auto& rect(bitmapFont->getGlyphRect(c));

						auto newPos(vertices.empty() ? Vec2f(0.f, bdd.height) : vertices.back().position);

						newPos.x += bdd.nextHChunkSpacing;
						bdd.nextHChunkSpacing = 0.f;

						if(bdd.nl > 0)
						{
							pushRowData();

							bdd.iX = 0;
							newPos.x = 0;

							for(; bdd.nl > 0; --bdd.nl) newPos.y += bdd.height + leading;
						}

						newPos.x += tracking;
						for(; bdd.htab > 0; --bdd.htab) newPos.x += 4 * (bdd.width + tracking);
						for(; bdd.vtab > 0; --bdd.vtab) newPos.y += 4 * (bdd.height + leading);

						auto gLeft(newPos.x);
						auto gBottom(newPos.y);
						auto gRight(gLeft + bdd.width);
						auto gTop(gBottom - bdd.height);

						ssvu::clampMax(bdd.xMin, gLeft);
						ssvu::clampMin(bdd.xMax, gRight);
						ssvu::clampMax(bdd.yMin, gTop);
						ssvu::clampMin(bdd.yMax, gBottom);

						vertices.emplace_back(Vec2f(gRight, gTop),		Vec2f(rect.left + rect.width,	rect.top));
						vertices.emplace_back(Vec2f(gLeft, gTop),		Vec2f(rect.left,				rect.top));
						vertices.emplace_back(Vec2f(gLeft, gBottom),	Vec2f(rect.left,				rect.top + rect.height));
						vertices.emplace_back(Vec2f(gRight, gBottom),	Vec2f(rect.left + rect.width,	rect.top + rect.height));

						++bdd.iX;
					}

					mChunk.idxHierarchyEnd = vertices.size();
				}

			public:
				inline BTRRoot() noexcept { }
				inline BTRRoot(const BitmapFont& mBF) noexcept : bitmapFont{&mBF}, texture{&bitmapFont->getTexture()} { }

				inline void clear()
				{
					mustRefreshGeometry = true;
					chunks.clear();
					effects.clear();
					baseChunk = &mkChunk();
					lastChunk = baseChunk;
				}
				inline void update(FT mFT) noexcept
				{
					baseChunk->update(mFT);
				}

				template<typename... TArgs> inline decltype(auto) in(TArgs&&... mArgs)
				{
					return baseChunk->in(FWD(mArgs)...);
				}
				template<typename T, typename... TArgs> inline decltype(auto) eff(TArgs&&... mArgs)
				{
					return baseChunk->eff<T>(FWD(mArgs)...);
				}

				inline void setAlign(TextAlign mX) noexcept
				{
					auto newAlignMult(ssvu::toFloat(ssvu::castEnum(mX)) * 0.5f);

					if(alignMult == newAlignMult) return;

					alignMult = newAlignMult;
					mustRefreshGeometry = true;
				}

				inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override
				{
					SSVU_ASSERT(bitmapFont != nullptr && texture != nullptr);

					refreshIfNeeded();

					mRenderStates.texture = texture;
					mRenderStates.transform *= getTransform();
					mRenderTarget.draw(vertices, mRenderStates);
				}

				inline auto& getRoot() noexcept { return *baseChunk; }
				inline auto& getLast() noexcept { return *lastChunk; }

				inline const auto& getBitmapFont() const noexcept	{ return bitmapFont; }
				inline const auto& getLocalBounds() const			{ refreshGeometryIfNeeded(); return bounds; }
				inline auto getGlobalBounds() const					{ refreshGeometryIfNeeded(); return globalBounds; }
		};
	}
}

#endif
